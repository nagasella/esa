#ifndef ESA_ENTITY_TABLE_H
#define ESA_ENTITY_TABLE_H

#include <cassert>

#include "esa.h"


namespace esa
{
    
    template<uint32_t Entities, uint32_t Components, uint32_t Updaters, uint32_t Queries, uint32_t Applys>
    class entity_table
    {
        /**
         * @brief The number of currently allocated entities.
         * 
         */
        uint32_t _size;
        

        /**
         * @brief The number of rows used. (may not coincide with the number of allocated entities)
         * 
         */
        uint32_t _used;


        /**
         * @brief Keeps track of the entities in teh table.
         * 
         */
        entity_mask<Entities> _emask;


        /**
         * @brief Entity mask storing entities marked for destruction.
         * 
         */
        entity_mask<Entities> _destroyed;


        /**
         * @brief Entity IDs that are pooled upon entity's destruction.
         * 
         */
        vector<entity, Entities> * _pooled_ids;


        /**
         * @brief Tells where each type of component series (table column) is located.
         * 
         */
        array<ram, Components> * _components_location;


        /**
         * @brief The table's columns. (components)
         * 
         */
        array<iseries *, Components> _columns;


        /**
         * @brief Updaters.
         * 
         */
        vector<iupdater *, Updaters> * _updaters;


        /**
         * @brief Cached queries.
         * 
         */
        vector<icached_query *, Queries> * _queries;


        /**
         * @brief Cached apply obejcts.
         * 
         */
        vector<icached_apply *, Applys> * _applys;


        /**
         * @brief Destroy an entity previousy marked for destruction.
         * 
         * @param e The ID of the entity.
         */
        void _destory(entity e)
        {
            unsubscribe(e, true);
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


        public:


        /**
         * @brief Constructor.
         * 
         */
        entity_table() : _columns(nullptr)
        {
            _used = 0;
            _size = 0;
            _pooled_ids = new vector<entity, Entities>();
            _components_location = new array<ram, Components>();
            _updaters = new vector<iupdater *, Updaters>();
            _queries = new vector<icached_query *, Queries>();
            _applys = new vector<icached_apply *, Applys>();
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
         * @brief Tells the number of rows used in the table up to now.
         * This may be different from the total number of entities in the table,
         * as some rows may be currently unused (destroyed entities).
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
         * @brief Mark an entity for destruction. The entity
         * wil be destroyed at the end of the current update
         * iteration. If some of the entity's component allocates resources,
         * you should remember to deallocate them inside their destructors.
         * 
         * @param e The ID of the entity.
         */
        void destroy(entity e)
        {
            _destroyed.add(e);
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
         * @brief Add a new column of a certain data type to the table. A column is
         * just an array of components. The components data is allocated in EWRAM: if you want it to
         * be allocated in IWRAM for performance reasons, use `entity_table::add_series` instead.
         * 
         * @tparam ComponentType The data type of the component.
         * @param tag The unique tag to assign to the component.
         */
        template<typename ComponentType>
        void add_component(tag_t tag)
        {
            assert(_columns[tag] == nullptr);
            (*_components_location)[tag] = ram::EWRAM;
            _columns[tag] = new series<ComponentType, Entities>();
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
         * @brief Add a new column to the table. A column is an array of components. 
         * In this case, the column must have been previously
         * allocated on the stack (IWRAM), as a `esa::series<ComponentType, Entities>` object.
         * The `Entities` template parameter of the series should match the one of the entity table.
         * 
         * @param column A pointer to the series to add (created on the stack, and NOT with `new`).
         * @param tag The unique tag to associate to this component.
         */
        void add_series(iseries * s, tag_t tag)
        {
            assert(_columns[tag] == nullptr);
            (*_components_location)[tag] = ram::IWRAM;
            _columns[tag] = s;
        }


        /**
         * @brief Obtain a reference to one of the table's columns.
         * This is of type `esa::series<ComponentType, Entities>`.
         * 
         * @tparam ComponentType The data type of the component.
         * @param tag The unique tag of the component.
         * @return series<ComponentType, Entities>& 
         */
        template<typename ComponentType, tag_t Tag>
        series<ComponentType, Entities> & get_series()
        {
            return static_cast<series<ComponentType, Entities> &>(*(_columns[Tag]));
        }


        /**
         * @brief Add an indexed component to the table. This is a
         * column in the table that uses an underying `esa::indexed_series` to
         * store components (to be used for memory efficiency).
         * 
         * @tparam ComponentType The data type of the component.
         * @tparam Size The maximum number of entities that can own this component.
         * @param tag The unique tag to associate to this component.
         */
        template<typename ComponentType, uint32_t Size>
        void add_component(tag_t tag)
        {
            assert(_columns[tag] == nullptr);
            (*_components_location)[tag] = ram::EWRAM;
            _columns[tag] = new indexed_series<ComponentType, Size>();
        }


        /**
         * @brief Obtain a reference to an entity's indexed component, based on its data type and tag.
         * 
         * @tparam ComponentType The data type of the component.
         * @tparam Size The size of the underlying indexed series.
         * @tparam Tag The unique tag of the component.
         * @param e The ID of the entity.
         * @return ComponentType& 
         */
        template<typename ComponentType, uint32_t Size, tag_t Tag>
        [[nodiscard]] ComponentType & get(entity e)
        {
            return static_cast<ComponentType &>(static_cast<indexed_series<ComponentType, Size>*>(_columns[Tag])->lookup(e));
        }


        /**
         * @brief Add a new indexed column to the table. This must be a pointer
         * to an `esa::indexed_series` object created on the stack (not using `new`).
         * 
         * @param column A pointer to the indexed series to add.
         * @param tag The unique tag to associate to this component.
         */
        void add_indexed_series(iseries * s, tag_t tag)
        {
            assert(_columns[tag] == nullptr);
            (*_components_location)[tag] = ram::IWRAM;
            _columns[tag] = s;
        }


        /**
         * @brief Obtain a reference to one of the table's indexed columns.
         * This is of type `esa::series<ComponentType, Entities>`.
         * 
         * @tparam ComponentType The data type of the component.
         * @tparam Size the Size of the indexed series.
         * @param tag The unique tag of the component.
         * @return series<ComponentType, Entities>& 
         */
        template<typename ComponentType, uint32_t Size, tag_t Tag>
        indexed_series<ComponentType, Size> & get_series()
        {
            return static_cast<indexed_series<ComponentType, Size> &>(*(_columns[Tag]));
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
        void add(entity e, const ComponentType & c)
        {
            static_cast<series<ComponentType, Entities>*>(_columns[Tag])->add(e, c);
        }


        /**
         * @brief Add an idnexed component to an entity.
         * 
         * @tparam ComponentType The type of the component.
         * @tparam Size The size of the underline indexed series.
         * @tparam Tag The unique tag of the component.
         * @param e The ID of the entity.
         * @param c An instance of the component.
         */
        template<typename ComponentType, uint32_t Size, tag_t Tag>
        void add(entity e, const ComponentType & c)
        {
            static_cast<indexed_series<ComponentType, Size>*>(_columns[Tag])->add(e, c);
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
                if (!(u->active()))
                    continue;
                u->update();
            }
            for (entity e = 0; e < _used; e++)
            {
                if (_destroyed.contains(e))
                {
                    _destory(e);
                    _destroyed.remove(e);
                }
            }
        }


        /**
         * @brief Subscribe an entity to all the relevant entity updaters, cached queries 
         * and cached apply objects.
         * 
         * @param e The ID of the entity.
         */
        void subscribe(entity e)
        {
            for (auto u : *_updaters)
            {
                if (u->subscribable())
                {
                    isubscribable_updater * su = static_cast<isubscribable_updater *>(u);
                    su->subscribe(e);
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
            unsubscribe(e, false);
        }


        void unsubscribe(entity e, bool destroy)
        {
            for (auto u : *_updaters)
            {
                if (u->subscribable())
                {
                    isubscribable_updater * su = static_cast<isubscribable_updater *>(u);
                    su->unsubscribe(e, destroy);
                }
            }
            for (auto q : *_queries)
                q->unsubscribe(e);
            for (auto a : *_applys)
                a->unsubscribe(e);
        }


        /**
         * @brief Attach an updater to the table.
         * 
         * @param u A pointer to the updater, created with `new`.
         */
        void add_updater(iupdater * u)
        {
            _updaters->push_back(u);
        }


        /**
         * @brief Attach an updater to the table, specifying if it
         * will start active or not.
         * 
         * @param u A pointer to the updater, created with `new`.
         * @param active If true, the entity updater will start active, otherwise inactive.
         */
        void add_updater(iupdater * u, bool active)
        {
            if (!active)
                u->deactivate();
            _updaters->push_back(u);
        }


        /**
         * @brief Attach a cached query to the table.
         * 
         * @param u A pointer to the cached query, created with `new`.
         */
        void add_query(icached_query* q)
        {
            _queries->push_back(q);
        }


        /**
         * @brief Attach a cached apply object to the table.
         * 
         * @param a A pointer to the cached apply object, created with `new`.
         */
        void add_apply(icached_apply* a)
        {
            _applys->push_back(a);
        }


        /**
         * @brief Get an updater based on its unique tag.
         * 
         * @tparam Tag The unique tag of the updater.
         * @return iupdater* 
         */
        template<tag_t Tag>
        [[nodiscard]] iupdater* get_updater()
        {
            for (auto u : *_updaters)
            {
                if (u->tag() == Tag)
                    return u;
            }
            assert(1 == 2 && "ESA ERROR: updater could not be found!");
            return nullptr;
        }


        /**
         * @brief Make an updater active (its `update` function will be executed).
         * 
         * @tparam Tag The unique tag of the updater.
         */
        template<tag_t Tag>
        void activate_updater()
        {
            for (auto u : *_updaters)
            {
                if (u->tag() == Tag)
                {
                    u->activate();
                    return;
                }       
            }
            assert(1 == 2 && "ESA ERROR: updater could not be found!");
        }


         /**
         * @brief Make an updater inactive (its `update` function will not be executed).
         * 
         * @tparam Tag The unique tag of the updater.
         */
        template<tag_t Tag>
        void deactivate_updater()
        {
            for (auto u : *_updaters)
            {
                if (u->tag() == Tag)
                {
                    u->deactivate();
                    return;
                }
            }
            assert(1 == 2 && "ESA ERROR: updater could not be found!");
        }


        /**
         * @brief Make all updaters attached to the table active (their `update` function will be executed).
         * 
         */
        void activate_all_updaters()
        {
            for (auto u : *_updaters)
                u->activate();
        }


        /**
         * @brief Make all updaters attached to the table inactive (their `update` function will not be executed).
         * 
         */
        void deactivate_all_updaters()
        {
            for (auto u : *_updaters)
                u->deactivate();
        }


        /**
         * @brief Unsubscribe an entity from an entity udpater.
         * 
         * @tparam Tag The unique tag of the updater.
         * @param e he ID of the entity.
         */
        template<tag_t Tag>
        void unsubscribe_from_updater(entity e)
        {
            for (auto u : *_updaters)
            {
                if (u->tag() == Tag)
                {
                    isubscribable_updater * su = static_cast<isubscribable_updater *>(u);
                    su->unsubscribe(e);
                    return;
                }
            }
            assert(1 == 2 && "ESA ERROR: updater could not be found!");
        }


        /**
         * @brief Subscribe an entity to an entity udpater.
         * 
         * @tparam Tag The unique tag of the updater.
         * @param e he ID of the entity.
         */
        template<tag_t Tag>
        void subscribe_to_updater(entity e)
        {
            for (auto u : *_updaters)
            {
                if (u->tag() == Tag)
                {
                    isubscribable_updater* su = static_cast<isubscribable_updater*>(u);
                    su->subscribe(e);
                    return;
                }
            }
            assert(1 == 2 && "ESA ERROR: updater could not be found!");
        }


        /**
         * @brief Retrieve a cached query by its unique tag.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @return icached_query* 
         */
        template<tag_t Tag>
        [[nodiscard]] icached_query * get_query()
        {
            for (auto q : *_queries)
            {
                if (q->tag() == Tag)
                    return q;
            }
            assert(1 == 2 && "ESA ERROR: cached query could not be found!");
            return nullptr;
        }


        /**
         * @brief Unsubscribe an entity from a cached query.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @param e he ID of the entity.
         */
        template<tag_t Tag>
        void unsubscribe_from_query(entity e)
        {
            for (auto q : *_queries)
            {
                if (q->tag() == Tag)
                {
                    q->unsubscribe(e);
                    return;
                }
            }
            assert(1 == 2 && "ESA ERROR: cached query could not be found!");
        }


        /**
         * @brief Subscribe an entity to a cached query.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @param e he ID of the entity.
         */
        template<tag_t Tag>
        void subscribe_to_query(entity e)
        {
            for (auto q : *_queries)
            {
                if (q->tag() == Tag)
                {
                    q->subscribe(e);
                    return;
                }
            }
            assert(1 == 2 && "ESA ERROR: cached query could not be found!");
        }


        /**
         * @brief Retrieve a cached apply object by its unique tag.
         * 
         * @tparam Tag The unique tag of the cached apply object.
         * @return icached_apply* 
         */
        template<tag_t Tag>
        [[nodiscard]] icached_apply * get_apply()
        {
            for (auto a : *_applys)
            {
                if (a->tag() == Tag)
                    return a;
            }
            assert(1 == 2 && "ESA ERROR: cached apply could not be found!");
            return nullptr;
        }


        /**
         * @brief Unsubscribe an entity from a cached apply object.
         * 
         * @tparam Tag The unique tag of the cached apply object.
         * @param e he ID of the entity.
         */
        template<tag_t Tag>
        void unsubscribe_from_apply(entity e)
        {
            for (auto a : *_applys)
            {
                if (a->tag() == Tag)
                {
                    a->unsubscribe(e);
                    return;
                }
            }
            assert(1 == 2 && "ESA ERROR: cached query could not be found!");
        }


        /**
         * @brief Subscribe an entity to a cached apply object.
         * 
         * @tparam Tag The unique tag of the cached apply object.
         * @param e he ID of the entity.
         */
        template<tag_t Tag>
        void subscribe_to_apply(entity e)
        {
            for (auto a : *_applys)
            {
                if (a->tag() == Tag)
                {
                    a->subscribe(e);
                    return;
                }
            }
            assert(1 == 2 && "ESA ERROR: cached query could not be found!");
        }


        /**
         * @brief Run a cached query and get the IDs of the entities that satisfy it.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @tparam MaxEntities The expected maximum number of entities the query will find. (same as the `Entities` template parameter of the cached query)
         * @return esa::vector<entity, MaxEntities> 
         */
        template<tag_t Tag, uint32_t MaxEntities>
        [[nodiscard]] vector<entity, MaxEntities> query()
        {
            assert(MaxEntities <= Entities && "ESA ERROR: query cannot ask for more entities than the table contains!");
            vector<entity, MaxEntities> ids;
            cached_query<MaxEntities> * q = static_cast<cached_query<MaxEntities> *>(get_query<Tag>());
            assert(q != nullptr && "ESA ERROR: cached query could not be found!");
            for (auto e : q->subscribed())
            {
                if (q->where(e))
                    ids.push_back(e);
            }
            return ids;
        }


        /**
         * @brief Run a cached query and get the IDs of the entities that satisfy it.
         * Pass a reference to the vector that will be filled with the entity IDs.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @tparam MaxEntities The expected maximum number of entities the query will find. (same as the `Entities` template parameter of the cached query)
         * @param ids The vector to use to collect the entity IDs.
         */
        template<tag_t Tag, uint32_t MaxEntities>
        void query(vector<entity, MaxEntities> & ids)
        {
            assert(MaxEntities <= Entities && "ESA ERROR: query cannot ask for more entities than the table contains!");
            cached_query<MaxEntities> * q = static_cast<cached_query<MaxEntities> *>(get_query<Tag>());
            assert(q != nullptr && "ESA ERROR: cached query could not be found!");
            for (auto e : q->subscribed())
            {
                if (q->where(e))
                    ids.push_back(e);
            }
        }


        /**
         * @brief Run a query based on a user-defined function.
         * 
         * @tparam MaxEntities The expected maximum number of entities the query will find.
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
         * Pass a reference to the vector that will be filled with the entity IDs.
         * 
         * @tparam MaxEntities The expected maximum number of entities the query will find.
         * @param func A pointer to function implementing the query condition.
         * @param ids The vector to use to collect the entity IDs.
         */
        template<uint32_t MaxEntities>
        void query(bool (*func) (entity_table<Entities, Components, Updaters, Queries, Applys>&, entity), vector<entity, MaxEntities> & ids)
        {
            assert(MaxEntities <= Entities && "ESA ERROR: query cannot ask for more entities than the table contains!");
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)((*this), e))
                    ids.push_back(e);
            }
        }


        /**
         * @brief Run a query based on a user-defined function. 
         * Allows to pass a parameter of any type for dynamic filtering.
         * 
         * @tparam MaxEntities The expected maximum number of entities the query will find.
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
         * @brief Run a query based on a user-defined function. 
         * Allows to pass a parameter of any type for dynamic filtering, and 
         * a reference to the vector that will be filled with the entity IDs.
         * 
         * @tparam MaxEntities The expected maximum number of entities the query will find.
         * @tparam T The type of the parameter passed to the query.
         * @param func The function implementing the query condition.
         * @param parameter The parameter to pass to the query function.
         * @param ids The vector to use to collect the entity IDs.
         */
        template<uint32_t MaxEntities, typename T>
        void query(bool (*func) (entity_table<Entities, Components, Updaters, Queries, Applys>&, entity, T&), T& parameter, vector<entity, MaxEntities> & ids)
        {
            assert(MaxEntities <= Entities && "ESA ERROR: query cannot ask for more entities than the table contains!");
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)((*this), e, parameter))
                    ids.push_back(e);
            }
        }


        /**
         * @brief Run a cached apply on the table.
         * 
         * @tparam Tag The unique tag of the cached apply.
         * @tparam MaxEntities The expected maximum number of entities the apply will work on. (same as the `Entities` template parameter of the cached apply)
         */
        template<tag_t Tag, uint32_t MaxEntities>
        void apply()
        {
            cached_apply<MaxEntities> * a = static_cast<cached_apply<MaxEntities> *>(get_apply<Tag>());
            assert(a != nullptr && "ESA ERROR: cached apply object could not be found!");
            for (auto e : a->subscribed())
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
                if ((*_components_location)[i] == ram::EWRAM && _columns[i] != nullptr)
                    delete _columns[i];
            }

            delete _components_location;
        }

    };

}


#endif