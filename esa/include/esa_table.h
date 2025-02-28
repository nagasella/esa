/**
 * @file esa_table.h
 * @author nagasella
 * @brief An Entity System for GBA games.
 * @version 0.1
 * @date 2025-02-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_TABLE_H
#define ESA_TABLE_H

#include "esa.h"
#include "esa_arrays.h"
#include "esa_updaters.h"

#include "bn_assert.h"
#include "bn_vector.h"

namespace esa
{

    template<u32 Entities, u32 Models, u32 Fixed, u32 Ints, u32 EntityUpdaters, u32 TableUpdaters>
    class entity_table
    {

        static_assert(Entities <= 256, "ESA ERROR: table cannot have more than 256 entities!");
        static_assert(Models   <= 32, "ESA ERROR: table cannot have more than 32 models!");
        static_assert(Fixed    <= 32, "ESA ERROR: table cannot have more than 32 bn::fixed Fields!");
        static_assert(Ints     <= 32, "ESA ERROR: table cannot have more than 32 int Fields!");
        
        u32 _size;
        u32 _used;

        u32 _mmask;
        u32 _emask [ (Entities - 1) / 32 + 1 ];

        bn::vector<entity_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> *, TableUpdaters> _entity_updaters;
        bn::vector<table_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> *, EntityUpdaters> _table_updaters;


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
         * @brief Used to access/modify `enum` fields for each entity in the table.
         * 
         */
        enum_array<Entities, Models> enums;

        /**
         * @brief Used to access/modify/remove the sprite of each entity in the table.
         * 
         */
        sprite_array<Entities> sprites;

        /**
         * @brief Used to read the entity model for each entity in the table.
         * 
         */
        model_array<Entities>  models;


        /**
         * @brief Default constructor.
         * 
         */
        entity_table() : _size(0), _used(0)
        {
            for (u32 i = 0; i < (Entities - 1) / 32 + 1; i++)
                *(_emask + i) = 0;
            for (u32 e = 0; e < Entities; e++)
            {
                models.clear(e);
                fixed.clear(e);
                intgs.clear(e);
                bools.clear(e);
                enums.clear(e);
                sprites.clear(e);
            }
        }


        /**
         * @brief Create a new entity, assign it a model and return its ID.
         * 
         * @tparam Model The model to assign to the entity.
         * @return u32 The ID of the newly created entity.
         */
        template<u32 Model>
        u32 create()
        {
            BN_ASSERT(!full(), "ESA ERROR: maximum entities capacity exceeded!");
            BN_ASSERT(Model < Models, "ESA ERROR: maximum number of entity models exceeeded!");

            for (u32 e = 0; e <= _used; e++)
            {
                if (!contains(e))
                {
                    *(_emask + e / 32) |= (1 << (e % 32));
                    _size++;
                    if (e == _used)
                        _used++;
                    models.template add<Model>(e);
                    return e;
                }
            }
            return enull;
        }


        /**
         * @brief Deletes an entity from the table.
         * Also takes care of clearing all the entity's fields and sprite.
         * The deleted entity will also not be processed anymore by any updater.
         * 
         * @param e The ID of the entity to delete.
         */
        void destroy(u32 e)
        {
            unsubscribe(e);
            models.clear(e);
            fixed.clear(e);
            intgs.clear(e);
            bools.clear(e);
            enums.clear(e);
            sprites.clear(e);
            *(_emask + e / 32) &= ~(1 << (e % 32));
            _size--;
            if (e == _used - 1)
                _used--;
        }


        /**
         * @brief Deletes all entities from the table.
         * 
         */
        void clear()
        {
            for (u32 e = 0; e < used(); e++)
                destroy(e);
        }


        /**
         * @brief Tells the total number of entities currently present in the `entity_table`.
         * 
         * @return u32
         */
        u32 size()
        {
            return _size;
        }


        /**
         * @brief Returns the last row allocated inside the table (last entitiy ID).
         * Some of the IDs before this might currently be empty.
         * This function does not give information about the number of
         * entities currently in the table (use `entity_table::size()` for that).
         * 
         * @return u32 
         */
        u32 used()
        {
            return _used;
        }


        /**
         * @brief Tells if this `entity_table` is full (that is, if all entity IDs are taken).
         * 
         * @return true 
         * @return false 
         */
        bool full()
        {
            return _size == Entities;
        }


        /**
         * @brief Tells if the `entity_table` contains an entity with the specified ID.
         * 
         * @param e The ID to check for.
         * @return true 
         * @return false 
         */
        bool contains(u32 e)
        {
            return (( *(_emask + e / 32) >> (e % 32) ) & 1) == 1;
        }


        /**
         * @brief Add an `entity_updater` to the table.
         * 
         * @param u A pointer to the `entity_updater`.
         */
        void add_updater(entity_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> * u)
        {
            u->require();
            _entity_updaters.push_back(u);
        }


        /**
         * @brief Add a `table_updater` to this table.
         * 
         * @param u A pointer to the `table_updater`.
         */
        void add_updater(table_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> * u)
        {
            _table_updaters.push_back(u);
        }


        /**
         * @brief Retrieve an `entity_updater` associated to the table, based on its tag.
         * 
         * @tparam Tag The tag assigned to the `entity_updater`.
         * @return entity_updater<Table, table_processor<Table, EntityUpdaters, TableUpdaters>>* A pointer to the `entity_updater`
         */
        template<u32 Tag>
        entity_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> * get_entity_updater()
        {
            for (auto u : _entity_updaters)
            {
                if (u->tag() == Tag)    
                    return u;
            }
        }


        /**
         * @brief Retrieve a `table_updater` associated to the table, based on its tag.
         * 
         * @tparam Tag The tag assigned to the `table_updater`.
         * @return entity_updater<Table, table_processor<Table, EntityUpdaters, TableUpdaters>>* A pointer to the `table_updater`
         */
        template<u32 Tag>
        table_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> * get_table_updater()
        {
            for (auto u : _table_updaters)
            {
                if (u->tag() == Tag)    
                    return u;
            }
        }

        /**
         * @brief Subscribe an entity to all the relevant `entity_updaters` owned by this table. 
         * Must be called after having created an entity and having initialized its fields.
         * 
         * @param e The ID of the entity to subscribe.
         */
        void subscribe(u32 e)
        {
            for (auto u : _entity_updaters)
            {
                u->subscribe(e);
            }
        }

        /**
         * @brief Unsubscribe an entity from all the `entity_updaters` owned by this `table_processor`.
         * This is called automatically when `entity_table::desttoy(u32 e)` is used.
         * 
         * @param e 
         */
        void unsubscribe(u32 e)
        {
            for (auto u : _entity_updaters)
            {
                u->unsubscribe(e);
            }
        }

        /**
         * @brief Execute the `init()` function for all the updaters owned by the table.
         * 
         */
        void init()
        {
            for (auto u : _entity_updaters)
                u->init();
            for (auto u : _table_updaters)
                u->init();
        }

        /**
         * @brief Execute the `update()` or `update(u32 e)` function for all the updaters owned by the table.
         * 
         */
        void update()
        {
            for (u32 e = 0; e < used(); e++)
            {
                for (auto u : _entity_updaters)
                {
                    if (u->subscribed(e))
                    {
                        u->update(e);
                    }
                }
            }
            for (auto u : _table_updaters)
                u->update();
        }

        /**
         * @brief Run a query on the table, returning the IDs of the entities that satisfy the query condition.
         * 
         * @tparam size The expected maximum size of the bn::vector to return.
         * @param func A pointer to the function implementing the query condition.
         * @return bn::vector<u32, size> A vector with the IDs of the entities that satisfy the query condition.
         */
        template<u32 Size>
        bn::vector<u32, Size> query(bool (*func) (entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>&, u32))
        {
            bn::vector<u32, Size> ids;
            for (u32 e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)(*this, e))
                    ids.push_back(e);
            }
            return ids;
        }


        /**
         * @brief Run a query on the table, returning the IDs of the entities that satisfy the query condition.
         * This function allows to pass an object containing parameters to use in the query.
         * 
         * @tparam size The expected maximum size of the bn::vector to return.
         * @tparam T The type of the object that will be passed to the query function.
         * @param func A pointer to the function implementing the query condition.
         * @param parameters A reference to the object that will be passed as a parameter to the query function.
         * @return bn::vector<u32, size> A vector with the IDs of the entities that satisfy the query condition.
         */
        template<u32 size, typename T>
        bn::vector<u32, size> query(bool (*func) (entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>&, u32, T&), T& parameters)
        {
            bn::vector<u32, size> ids;
            for (u32 e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)(*this, e, parameters))
                    ids.push_back(e);
            }
            return ids;
        }


        /**
         * @brief Applies a function to a table. Differently from queries, the
         * applied function is supposed to modify entities and their fields in some way, based on a condition.
         * 
         * @param func A pointer to the function to apply.
         */
        void apply(bool (*func) (entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>&, u32))
        {
            for (u32 e = 0; e < used(); e++)
            {
                if (contains(e))
                {
                    if ((*func)(*this, e))
                        break;
                }
                    
            }
        }


        /**
         * @brief Apply a function to a table. Differently from queries, the
         * applied function is supposed to modify entities and their fields in some way, based on a condition.
         * 
         * @tparam T The type of the object that will be passed to the applied function.
         * @param func A pointer to the function to be applied.
         * @param parameters A reference to the object that will be passed as a parameter to the applied function.
         */
        template<typename T>
        void apply(bool (*func) (entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>&, u32, T&), T& parameters)
        {
            for (u32 e = 0; e < used(); e++)
            {
                if (contains(e))
                {
                    if ((*func)(*this, e, parameters))
                        break;
                }
            }
        }

    };

}

#endif