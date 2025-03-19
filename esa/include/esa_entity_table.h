/**
 * @file esa_entity_table.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_TABLE_H
#define ESA_TABLE_H

#include "esa.h"
#include "esa_arrays.h"
#include "esa_table_updater.h"
#include "esa_entity_updater.h"
#include "esa_cached_query.h"

#include "bn_assert.h"
#include "bn_vector.h"

namespace esa
{

    template<u32 Entities, u32 Models, u32 Fixed, u32 Ints>
    class entity_table
    {

        using table          = entity_table<Entities, Models, Fixed, Ints>;
        using entity_updater = entity_updater<table>;
        using table_updater  = table_updater<table>;
        using cached_query   = cached_query<table>;
        using cached_apply   = cached_apply<table>;

        static_assert(Entities <= 256, "ESA ERROR: table cannot have more than 256 entities!");
        static_assert(Models   <= 32, "ESA ERROR: table cannot have more than 32 models!");
        static_assert(Fixed    <= 32, "ESA ERROR: table cannot have more than 32 bn::fixed fields!");
        static_assert(Ints     <= 32, "ESA ERROR: table cannot have more than 32 int fields!");
        static_assert(Entities > 0, "ESA ERROR: table must have at least 1 entity!");
        static_assert(Models   > 0, "ESA ERROR: table must have at least 1 model!");

        u32 _size;
        
        u32 _used;

        u32 _emask [ (Entities - 1) / 32 + 1 ];

        bn::vector<entity_updater *, 128> * _entity_updaters;

        bn::vector<table_updater *, 128> * _table_updaters;
        
        bn::vector<cached_query *, 128> * _cached_queries;
        
        bn::vector<cached_apply *, 128> * _cached_applys;
        
        bn::vector<u32, Entities>* _pooled_ids;

        public:

        /**
         * @brief Used to access/modify `bn::fixed` fields for each entity in the table.
         * 
         */
        field_array<bn::fixed, Entities, Models, Fixed> fixed;

        /**
         * @brief Used to access/modify `int` fields for each entity in the table.
         * 
         */
        field_array<int, Entities, Models, Ints> intgs;

        /**
         * @brief Used to access/modify `bool` fields for each entity in the table.
         * 
         */
        bool_array<Entities, Models> bools;

        /**
         * @brief Used to access/modify `unitn_t` fields for each entity in the table.
         * 
         */
        uintn_array<Entities, Models> uints;

        /**
         * @brief Used to access/modify/remove the sprite of each entity in the table.
         * 
         */
        sprite_array<Entities> sprites;

        /**
         * @brief Used to obtain the entity model for each entity in the table.
         * 
         */
        model_array<Entities>  models;


        /**
         * @brief Constructor.
         * 
         */
        entity_table() : _size(0), _used(0)
        {
            for (entity e = 0; e < (Entities - 1) / 32 + 1; e++)
                *(_emask + e) = 0;
                
            _entity_updaters = new bn::vector<entity_updater *, 128>();
            _table_updaters  = new bn::vector<table_updater *, 128>();
            _cached_queries  = new bn::vector<cached_query *, 128>();
            _cached_applys   = new bn::vector<cached_apply *, 128>();
            _pooled_ids      = new bn::vector<u32, Entities>();
        }


        /**
         * @brief Create a new entity, assign it a model and return its ID.
         * 
         * @tparam Model The model to assign to the entity.
         * @return u32 The ID of the newly created entity.
         */
        template<entity_model Model>
        entity create()
        {
            BN_ASSERT(!full(), "ESA ERROR: maximum entities capacity exceeded!");
            BN_ASSERT(Model < Models, "ESA ERROR: maximum number of entity models exceeeded!");

            entity e = _used;

            if (!_pooled_ids->empty())
            {
                e = _pooled_ids->back();
                _pooled_ids->pop_back();
            }

            *(_emask + (e >> 5)) |= (1 << (e & 31));
            _size++;
            if (e == _used)
                _used++;

            models.template add<Model>(e);
            
            subscribe(e);

            return e;
        }


        /**
         * @brief Deletes an entity from the table.
         * Also takes care of clearing all the entity's fields and sprite,
         * and unsubscribe the entity from all updaters, cached queries and chached apply objects.
         * 
         * @param e The ID of the entity.
         */
        void destroy(entity e)
        {
            unsubscribe(e);
            models.clear(e);
            fixed.clear(e);
            intgs.clear(e);
            bools.clear(e);
            uints.clear(e);
            sprites.clear(e);
            *(_emask + (e >> 5)) &= ~(1 << (e & 31));
            _size--;
            if (e == _used - 1)
                _used--;
            if (!_pooled_ids->full())
                _pooled_ids->push_back(e);
        }


        /**
         * @brief Deletes all entities from the table.
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
         * @brief Tells the total number of entities currently present in the table.
         * 
         * @return u32
         */
        u32 size()
        {
            return _size;
        }


        /**
         * @brief Returns the last row of the table (last entitiy ID).
         * In general, it will not coincide with the number of entities in the table.
         * 
         * @return u32 
         */
        u32 used()
        {
            return _used;
        }


        /**
         * @brief Tells if the table is full (that is, if all entity IDs are used).
         * 
         * @return true 
         * @return false 
         */
        bool full()
        {
            return _size == Entities;
        }


        /**
         * @brief Tells if the table contains an entity.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        bool contains(entity e)
        {
            return (( *(_emask + (e >> 5)) >> (e & 31) ) & 1) == 1;
        }


        /**
         * @brief Add an entity updater to the table. Each table can have up to 128 entity updaters.
         * 
         * @param u A pointer to the entity updater, created using `new`.
         */
        void add_updater(entity_updater * u)
        {
            _entity_updaters->push_back(u);
        }


        /**
         * @brief Add a table updater to this table. Each table can have up to 128 table updaters.
         * 
         * @param u A pointer to the table updater, created using `new`.
         */
        void add_updater(table_updater * u)
        {
            _table_updaters->push_back(u);
        }


        /**
         * @brief Add a cached query to this table. Each table can have up to 128 cached queries.
         * 
         * @param q A pointer to the cached_query, created using `new`.
         */
        void add_query(cached_query * q)
        {
            _cached_queries->push_back(q);
        }


        /**
         * @brief Add a cached apply to this table. Each table can have up to 128 cached apply.
         * 
         * @param q A pointer to the cached apply, created using `new`.
         */
        void add_apply(cached_apply * a)
        {
            _cached_applys->push_back(a);
        }


        /**
         * @brief Retrieve an entity updater based on its tag.
         * 
         * @tparam Tag The unique tag of the entity updater.
         * @return entity_updater* A pointer to the entity updater.
         */
        template<tag_t Tag>
        entity_updater * get_entity_updater()
        {
            for (auto u : *_entity_updaters)
            {
                if (u->tag() == Tag)    
                    return u;
            }
            BN_ASSERT(1 == 2, "ESA ERROR: entity updater not found!");
            return nullptr;
        }


        /**
         * @brief Retrieve a table updater based on its tag.
         * 
         * @tparam Tag The unique tag of the table updater.
         * @return table_updater* A pointer to the table updater.
         */
        template<tag_t Tag>
        table_updater * get_table_updater()
        {
            for (auto u : *_table_updaters)
            {
                if (u->tag() == Tag)    
                    return u;
            }
            BN_ASSERT(1 == 2, "ESA ERROR: table updater not found!");
            return nullptr;
        }


        /**
         * @brief Retrieve a cached query based on its tag.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @return cached_query* A pointer to the cached query.
         */
        template<tag_t Tag>
        cached_query * get_cached_query()
        {
            for (auto q : *_cached_queries)
            {
                if (q->tag() == Tag)    
                    return q;
            }
            BN_ASSERT(1 == 2, "ESA ERROR: cached query not found!");
            return nullptr;
        }


        /**
         * @brief Retrieve a cached apply object based on its tag.
         * 
         * @tparam Tag The unique tag of the cached apply object.
         * @return cached_apply* A pointer to the cached apply object.
         */
        template<tag_t Tag>
        cached_apply * get_cached_apply()
        {
            for (auto a : *_cached_applys)
            {
                if (a->tag() == Tag)    
                    return a;
            }
            BN_ASSERT(1 == 2, "ESA ERROR: cached apply not found!");
            return nullptr;
        }
        

        /**
         * @brief Subscribe an entity to all entity updaters, cached queries
         * and cached apply objects.
         * 
         * @param e The ID of the entity.
         */
        void subscribe(entity e)
        {
            for (auto u : *_entity_updaters)
                u->subscribe(e);
            for (auto q : *_cached_queries)
                q->subscribe(e);
            for (auto a : *_cached_applys)
                a->subscribe(e);
        }


        /**
         * @brief Unsubscribe an entity from all entity updaters, cached queries
         * and cached apply objects.
         * 
         * @param e The ID of the entity.
         */
        void unsubscribe(entity e)
        {
            for (auto u : *_entity_updaters)
                u->unsubscribe(e);
            for (auto q : *_cached_queries)
                q->unsubscribe(e);
            for (auto a : *_cached_applys)
                a->unsubscribe(e);
        }


        /**
         * @brief Execute the `init()` function for all the updaters, cached queries
         * and cached apply objects.
         * 
         */
        void init()
        {
            for (auto u : *_entity_updaters)
                u->init();
            for (auto u : *_table_updaters)
                u->init();
            for (auto q : *_cached_queries)
                q->init();
            for (auto a : *_cached_applys)
                a->init();
        }

        /**
         * @brief Execute the `update()` or `update(u32 e)` function for all the updaters.
         * The processing order is: first, all entity updaters are processed, in order of insertion;
         * then, all the table updaters are processed, in order of insertion.
         * 
         */
        void update()
        {
            for (auto u : *_entity_updaters)
            {
                for (entity e = 0; e < used(); e++)
                {
                    if (u->subscribed(e))
                        u->update(e);
                }
            }
            for (auto u : *_table_updaters)
                u->update();
        }


        /**
         * @brief Run a cached query on the table.
         * 
         * @tparam Tag The unique tag of the cached query.
         * @tparam Size The expected maximum number of entity IDs the query will return.
         * @return bn::vector<u32, size> A vector with the IDs of the entities that satisfy the query.
         */
        template<tag_t Tag, u32 Size>
        bn::vector<entity, Size> query()
        {
            bn::vector<entity, Size> ids;
            cached_query * q = get_cached_query<Tag>();
            
            for (entity e = 0; e < used(); e++)
            {
                if (q->subscribed(e) && q->where(e))
                    ids.push_back(e);
            }

            return ids;
        }


        /**
         * @brief Run a user defined query function on the table.
         * 
         * @tparam Size The expected maximum number of entity IDs the query will return.
         * @param func A pointer to the function implementing the query.
         * @return bn::vector<u32, size> A vector with the IDs of the entities that satisfy the query.
         */
        template<u32 Size>
        bn::vector<entity, Size> query(bool (*func) (entity_table<Entities, Models, Fixed, Ints>&, entity))
        {
            bn::vector<entity, Size> ids;

            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)(*this, e))
                    ids.push_back(e);
                if (ids.size() == Size)
                    break;
            }

            return ids;
        }


        /**
         * @brief Run a user defined query on the table, passing some parameter for dynamic filtering.
         * 
         * @tparam Size The expected maximum number of entity IDs the query will return.
         * @tparam T The type of the parameter passed to the query function.
         * @param func A pointer to the function implementing the query.
         * @param parameters The object to pass as a parameter (must be a reference).
         * @return bn::vector<entity, size> A vector with the IDs of the entities that satisfy the query.
         */
        template<u32 Size, typename T>
        bn::vector<entity, Size> query(bool (*func) (entity_table<Entities, Models, Fixed, Ints>&, entity, T&), T& parameters)
        {
            bn::vector<entity, Size> ids;

            for (entity e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)(*this, e, parameters))
                    ids.push_back(e);
                if (ids.size() == Size)
                    break;
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
            cached_apply * apply = get_cached_apply<Tag>();

            for (entity e = 0; e < used(); e++)
            {
                if (apply->subscribed(e))
                {
                    if (apply->apply(e))
                        break;
                }
            }
        }


        /**
         * @brief Apply a user-defined function iteratively the entire table.
         * 
         * @param func A pointer to the function to apply.
         */
        void apply(bool (*func) (entity_table<Entities, Models, Fixed, Ints>&, entity))
        {
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e))
                {
                    if ((*func)(*this, e))
                        break;
                }
            }
        }


        /**
         * @brief Apply a user-defined function iteratively the entire table,
         * passing some parameter for dynamic behavior.
         * 
         * @tparam T The type of the parameter passed to the function.
         * @param func A pointer to the function to be applied.
         * @param parameter The object to pass as a parameter (must be a reference).
         */
        template<typename T>
        void apply(bool (*func) (entity_table<Entities, Models, Fixed, Ints>&, entity, T&), T& parameter)
        {
            for (entity e = 0; e < used(); e++)
            {
                if (contains(e))
                {
                    if ((*func)(*this, e, parameter))
                        break;
                }
            }
        }


        /**
         * @brief Destructor.
         * 
         */
        ~entity_table()
        {
            for (u32 i = 0; i < _entity_updaters->size(); i++)
                delete (*_entity_updaters)[i];
            for (u32 i = 0; i < _table_updaters->size(); i++)
                delete (*_table_updaters)[i];
            for (u32 i = 0; i < _cached_queries->size(); i++)
                delete (*_cached_queries)[i];
            for (u32 i = 0; i < _cached_applys->size(); i++)
                delete (*_cached_applys)[i];

            delete _entity_updaters;
            delete _table_updaters;
            delete _cached_queries;
            delete _cached_applys;
            delete _pooled_ids;
        }

    };

}

#endif