#ifndef ESA_ENTITY_TABLE_H
#define ESA_ENTITY_TABLE_H

#include "esa.h"
#include <cassert>


namespace esa
{
    
    template<uint32_t Entities, uint32_t Components, uint32_t Updaters, uint32_t Queries, uint32_t Applys>
    class entity_table
    {
        /**
         * @brief The number of allocated entities.
         * 
         */
        uint32_t _size;
        

        /**
         * @brief The number of rows used. (may not coincide with the number of allocated entities)
         * 
         */
        uint32_t _used;


        /**
         * @brief Entity mask.
         * 
         */
        entity_mask<Entities> _emask;


        /**
         * @brief Entity IDs that are pooled upon entity's destruction.
         * 
         */
        vector<entity, Entities> * _pooled_ids;


        /**
         * @brief The table's columns. (components)
         * 
         */
        array<iseries<Entities> *, Components> _columns;


        /**
         * @brief Updaters.
         * 
         */
        vector<iupdater *, (Updaters > 0 ? Updaters : 1)> * _updaters;


        /**
         * @brief Cached queries.
         * 
         */
        vector<cached_query<Entities> *, (Queries > 0 ? Queries : 1)> * _queries;


        /**
         * @brief Cached applys.
         * 
         */
        vector<cached_apply<Entities> *, (Applys > 0 ? Applys : 1)> * _applys;


        public:


        /**
         * @brief Constructor.
         * 
         */
        entity_table() : _columns(nullptr)
        {
            _pooled_ids = new vector<entity, Entities>();
            _updaters = new vector<iupdater *, (Updaters > 0 ? Updaters : 1)>();
            _queries = new vector<cached_query<Entities> *, (Queries > 0 ? Queries : 1)>();
            _applys = new vector<cached_apply<Entities> *, (Applys > 0 ? Applys : 1)>();
        }


        /**
         * @brief Tells the number of entities currently in the table.
         * 
         * @return uint32_t 
         */
        [[nodiscard]] uint32_t size()
        {
            return _size;
        }


        /**
         * @brief Tells the number of rows used in the table up to now
         * This may be different from the total number of entities in the table,
         * as some rows may be empty.
         * 
         * @return uint32_t 
         */
        [[nodiscard]] uint32_t used()
        {
            return _used;
        }


        /**
         * @brief Tells if the table is full. (that is, if all entity IDs are used)
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool full()
        {
            return _size == Entities;
        }


        /**
         * @brief Create a new entity and return its ID.
         * 
         * @return entity The ID of the newly creatd entity.
         */
        [[nodiscard]] entity create()
        {
            assert(!full() && "ECSA ERROR: all available entity IDs are allocated!");
            entity e = _used;
            if (!_pooled_ids->empty())
            {
                e = _pooled_ids->back();
                _pooled_ids->pop_back();
            }
            _emask.add(e);
            _size++;
            if (e == _used)
                _used++;
            return e;
        }


        /**
         * @brief Destroy an entity.
         * 
         * @param e The ID of the entity.
         */
        void destroy(entity e)
        {
            unsubscribe(e);
            _emask.remove(e);
            _size--;
            if (e == _used - 1)
                _used--;
            for (uint32_t i = 0; i < _columns.size(); i++)
            {
                if (_columns[i] != nullptr)
                    _columns[i]->remove(e);
            }
            if (!_pooled_ids->full())
                _pooled_ids->push_back(e);
        }


        /**
         * @brief Delete all the entities from the table.
         * 
         */
        void clear()
        {
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e))
                    destroy(e);
            }
        }


        /**
         * @brief Add a type of component to the table. Each component corresponds to a column
         * of the table, and it must have a data type and a tag: while the data type can be the 
         * same for different components, the tag must be unique for each one of them.
         * 
         * @tparam ComponentType The data type of the component.
         * @param tag The unique tag to associate to this component.
         */
        template<typename ComponentType>
        void add_component(tag_t tag)
        {
            assert(_columns[tag] == nullptr);
            _columns[tag] = new series<ComponentType, Entities>(tag);
        }


        /**
         * @brief Obtain a reference to an entity's component, based on its data type and tag.
         * 
         * @tparam ComponentType The data type of the component.
         * @tparam Tag The unique tag of the component.
         * @param e The ID of the entity.
         * @return ComponentType& 
         */
        template<typename ComponentType, tag_t Tag>
        [[nodiscard]] ComponentType & get(entity e)
        {
            return static_cast<ComponentType &>(static_cast<series<ComponentType, Entities>*>(_columns[Tag])->get(e));
        }


        /**
         * @brief Add a component to an entity.
         * 
         * @tparam ComponentType The type of the component.
         * @tparam Tag The unique tag of the component.
         * @param e The ID of the entity.
         * @param c An instance of the component.
         */
        template<typename ComponentType, tag_t Tag>
        void add(entity e, ComponentType c)
        {
            ((series<ComponentType, Entities>*)(_columns[Tag]))->add(e, c);
        }


        /**
         * @brief Tells if the entity has a certain component.
         * 
         * @tparam Tag The unique tag of the component.
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        template<tag_t Tag>
        [[nodiscard]] bool has(entity e)
        {
            return _columns[Tag]->has(e);
        }


        /**
         * @brief Tells if the table contains a certain entity.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool contains(entity e)
        {
            return _emask.contains(e);
        }


        /**
         * @brief Initialize all the updaters, cached queries and cached apply objects.
         * 
         */
        void init()
        {
            for (auto u : *_updaters)
                u->init();
            for (auto q : *_queries)
                q->init();
            for (auto a : *_applys)
                a->init();
        }


        /**
         * @brief Update all updaters, in order of insertion.
         * 
         */
        void update()
        {
            for (auto u : *_updaters)
            {
                if (u->type() == udpater_type::ENTITY_UPDATER)
                {
                    entity_updater<Entities> * eu = (entity_updater<Entities> *) u;
                    vector<entity, Entities> ids = eu->entities();
                    for (auto e : ids)
                        eu->update(e);
                }
                else
                {
                    table_updater * tu = (table_updater *) u;
                    tu->update();
                }
            }
        }


        /**
         * @brief Subscribe an entity to all the entity updaters, cached queries and cached apply objects.
         * 
         * @param e The ID of the entity.
         */
        void subscribe(entity e)
        {
            for (auto u : *_updaters)
            {
                if (u->type() == udpater_type::ENTITY_UPDATER)
                {
                    entity_updater<Entities> * eu = (entity_updater<Entities> *) u;
                    eu->subscribe(e);
                }
            }
            for (auto q : *_queries)
                q->subscribe(e);
            for (auto a : *_applys)
                a->subscribe(e);
        }


        /**
         * @brief Unsubscribe an entity to all the entity updaters, cached queries and cached apply objects.
         * 
         * @param e The ID of the entity.
         */
        void unsubscribe(entity e)
        {
            for (auto u : *_updaters)
            {
                if (u->type() == udpater_type::ENTITY_UPDATER)
                {
                    entity_updater<Entities> * eu = (entity_updater<Entities> *) u;
                    eu->unsubscribe(e);
                }
            }
            for (auto q : *_queries)
                q->unsubscribe(e);
            for (auto a : *_applys)
                a->unsubscribe(e);
        }


        /**
         * @brief Add an entity updater to the table.
         * 
         * @param u A pointer to the entity updater, created with `new`.
         */
        void add_updater(iupdater * u)
        {
            _updaters->push_back(u);
        }


        /**
         * @brief Add a cached query to the table.
         * 
         * @param u A pointer to the cached query, created with `new`.
         */
        void add_query(cached_query<Entities>* q)
        {
            _queries->push_back(q);
        }


        /**
         * @brief Add a cached apply object to the table.
         * 
         * @param a A pointer to the cached apply, created with `new`.
         */
        void add_apply(cached_apply<Entities>* a)
        {
            _applys->push_back(a);
        }


        /**
         * @brief Get an entity updater based on its unique tag.
         * 
         * @tparam Tag The unique tag of the entity updater.
         * @return entity_updater<Entities>* 
         */
        template<tag_t Tag>
        [[nodiscard]] entity_updater<Entities> * get_entity_updater()
        {
            for (auto u : *_updaters)
            {
                if (u->type() == udpater_type::ENTITY_UPDATER && u->tag() == Tag)
                    return (entity_updater<Entities> *) u;
            }
            assert(1 == 2 && "ESA ERROR: entity updater could not be found!");
            return nullptr;
        }


        /**
         * @brief Get a table updater based on its unique tag.
         * 
         * @tparam Tag The unique tag of the table updater.
         * @return table_updater* 
         */
        template<tag_t Tag>
        [[nodiscard]] table_updater * get_table_updater()
        {
            for (auto u : *_updaters)
            {
                if (u->type() == udpater_type::TABLE_UPDATER && u->tag() == Tag)
                    return (table_updater *) u;
            }
            assert(1 == 2 && "ESA ERROR: table updater could not be found!");
            return nullptr;
        }


        /**
         * @brief Retrieve a cached query object by its unique tag.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @return cached_query<Entities>* 
         */
        template<tag_t Tag>
        [[nodiscard]] cached_query<Entities> * get_query()
        {
            for (auto q : *_queries)
            {
                if (q->tag() == Tag)
                    return (cached_query<Entities> *) q;
            }
            assert(1 == 2 && "ESA ERROR: cached query could not be found!");
            return nullptr;
        }


        /**
         * @brief Retrieve a cached apply object by its unique tag.
         * 
         * @tparam Tag The unique tag of the cached apply object.
         * @return cached_apply<Entities>* 
         */
        template<tag_t Tag>
        [[nodiscard]] cached_apply<Entities> * get_apply()
        {
            for (auto a : *_applys)
            {
                if (a->tag() == Tag)
                    return (cached_apply<Entities> *) a;
            }
            assert(1 == 2 && "ESA ERROR: cached apply could not be found!");
            return nullptr;
        }


        /**
         * @brief Run a cached query and get the IDs of the entities that satisfy it.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @tparam MaxEntities The expected maximum number of entities.
         * @return esa::vector<entity, MaxEntities> 
         */
        template<tag_t Tag, uint32_t MaxEntities>
        [[nodiscard]] vector<entity, MaxEntities> query()
        {
            assert(MaxEntities <= Entities && "ESA ERROR: query cannot ask for more entities than the table contains!");
            vector<entity, MaxEntities> ids;
            cached_query<Entities> * q = get_query<Tag>();
            assert(q != nullptr && "ESA ERROR: cached query could not be found!");
            for (auto e : q->entities())
            {
                if (q->where(e))
                    ids.push_back(e);
            }
            return ids;
        }


        /**
         * @brief Run a query based on a user-defined function.
         * 
         * @tparam MaxEntities The expected maximum number of entities.
         * @param func A pointer to function implementing the query condition.
         * @return esa::vector<entity, MaxEntities> 
         */
        template<uint32_t MaxEntities>
        [[nodiscard]] vector<entity, MaxEntities> query(bool (*func) (entity_table<Entities, Components, Updaters, Queries, Applys>&, entity))
        {
            assert(MaxEntities <= Entities && "ESA ERROR: query cannot ask for more entities than the table contains!");
            vector<entity, MaxEntities> ids;
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)((*this), e))
                    ids.push_back(e);
            }
            return ids;
        }


        /**
         * @brief Run a query based on a user-defined function. 
         * Allows to pass a parameter of any type for dynamic filtering.
         * 
         * @tparam MaxEntities The expected maximum number of entities.
         * @tparam T The type of the parameter passed to the query.
         * @param func The function implementing the query condition.
         * @param parameter The parameter to pass to the query function.
         * @return esa::vector<entity, MaxEntities> 
         */
        template<uint32_t MaxEntities, typename T>
        [[nodiscard]] vector<entity, MaxEntities> query(bool (*func) (entity_table<Entities, Components, Updaters, Queries, Applys>&, entity, T&), T& parameter)
        {
            assert(MaxEntities <= Entities && "ESA ERROR: query cannot ask for more entities than the table contains!");
            vector<entity, MaxEntities> ids;
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)((*this), e, parameter))
                    ids.push_back(e);
            }
            return ids;
        }


        /**
         * @brief Run a cached apply on the table.
         * 
         * @tparam Tag The unique tag of the cached apply.
         */
        template<tag_t Tag>
        void apply()
        {
            cached_apply<Entities> * a = get_apply<Tag>();
            assert(a != nullptr && "ESA ERROR: cached apply object could not be found!");
            for (auto e : a->entities())
            {
                if (a->apply(e))
                    return;
            }
        }


        /**
         * @brief Apply a user-defined function iteratively to the entire table.
         * 
         * @param func A pointer to the function to apply.
         */
        void apply(bool (*func) (entity_table<Entities, Components, Updaters, Queries, Applys>&, entity))
        {
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)((*this), e))
                    return;
            }
        }


        /**
         * @brief Apply a user-defined function iteratively to the entire table,
         * passing some parameter for dynamic behavior.
         * 
         * @tparam T The type of the parameter passed to the function.
         * @param func A pointer to the function to be applied.
         * @param parameter The object to pass as a parameter (must be a reference).
         */
        template<typename T>
        void apply(bool (*func) (entity_table<Entities, Components, Updaters, Queries, Applys>&, entity, T&), T& parameter)
        {
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)((*this), e, parameter))
                    return;
            }
        }


        /**
         * @brief Destructor.
         * 
         */
        ~entity_table()
        {
            delete _pooled_ids;

            for (uint32_t i = 0; i < _updaters->size(); i++)
                delete (*_updaters)[i];;
            delete _updaters;

            for (uint32_t i = 0; i < _queries->size(); i++)
                delete (*_queries)[i];
            delete _queries;

            for (uint32_t i = 0; i < _applys->size(); i++)
                delete (*_applys)[i];
            delete _applys;

            for (uint32_t i = 0; i < _columns.size(); i++)
            {
                if (_columns[i] != nullptr)
                    delete _columns[i];
            }
        }

    };

}


#endif