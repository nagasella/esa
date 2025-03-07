/**
 * @file esa_table.h
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

#include "bn_assert.h"
#include "bn_vector.h"

namespace esa
{

    template<u32 Entities, u32 Models, u32 Fixed, u32 Ints, u32 EntityUpdaters, u32 TableUpdaters>
    class entity_table
    {

        static_assert(Entities <= 256, "ESA ERROR: table cannot have more than 256 entities!");
        static_assert(Models   <= 32, "ESA ERROR: table cannot have more than 32 models!");
        static_assert(Fixed    <= 32, "ESA ERROR: table cannot have more than 32 bn::fixed fields!");
        static_assert(Ints     <= 32, "ESA ERROR: table cannot have more than 32 int fields!");
        static_assert(Entities > 0, "ESA ERROR: table must have at least 1 entity!");
        static_assert(Models   > 0, "ESA ERROR: table must have at least 1 model!");
        static_assert(EntityUpdaters > 0, "ESA ERROR: table must have at least 1 entity updater!");
        static_assert(TableUpdaters > 0, "ESA ERROR: table must have at least 1 table updater!");
        
        u32 _size;
        u32 _used;

        u32 _mmask;
        u32 _emask [ (Entities - 1) / 32 + 1 ];

        bn::vector<entity_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> *, EntityUpdaters> _entity_updaters;
        bn::vector<table_updater<entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>> *, TableUpdaters> _table_updaters;

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
                    *(_emask + (e >> 5)) |= (1 << (e & 31));
                    _size++;
                    if (e == _used)
                        _used++;
                    models.template add<Model>(e);
                    subscribe(e);
                    return e;
                }
            }
            return E_NULL;
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
            *(_emask + (e >> 5)) &= ~(1 << (e & 31));
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
            return (( *(_emask + (e >> 5)) >> (e & 31) ) & 1) == 1;
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
                        u->update(e);
                }
            }
            for (auto u : _table_updaters)
                u->update();
        }


        /**
         * @brief Run a query to obtain the IDs of the entities processed by a certain `entity_updater`.
         * 
         * @tparam Tag The unique tag of the `entity_updater`.
         * @tparam Size The expected maximum size of the bn::vector to return.
         * @param func A pointer to the function implementing the query condition.
         * @return bn::vector<u32, size> A vector with the IDs of the entities that satisfy the query condition.
         */
        template<u32 Tag, u32 Size>
        bn::vector<u32, Size> query()
        {
            return get_entity_updater<Tag>()->template subscribed<Size>();
        }


        /**
         * @brief Run a user defined query on the table.
         * 
         * @tparam Size The expected maximum size of the bn::vector to return.
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
                if (ids.size() == Size)
                    break;
            }
            return ids;
        }


        /**
         * @brief Run a user defined query on the table, passing some extra parameter for dynamic filtering.
         * 
         * @tparam Size The expected maximum size of the bn::vector to return.
         * @tparam T The type of the object that will be passed to the query function.
         * @param func A pointer to the function implementing the query condition.
         * @param parameters A reference to the object passed as a parameter to the query function.
         * @return bn::vector<u32, size> A vector with the IDs of the entities that satisfy the query condition.
         */
        template<u32 Size, typename T>
        bn::vector<u32, Size> query(bool (*func) (entity_table<Entities, Models, Fixed, Ints, EntityUpdaters, TableUpdaters>&, u32, T&), T& parameters)
        {
            bn::vector<u32, Size> ids;
            for (u32 e = 0; e < used(); e++)
            {
                if (contains(e) && (*func)(*this, e, parameters))
                    ids.push_back(e);
                if (ids.size() == Size)
                    break;
            }
            return ids;
        }


        /**
         * @brief Applies a function to the table, to modify the entities in the table conditionally.
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
         * @brief Applies a function to the table, to modify the entities in the table conditionally,
         * passing some extra parameter for dynamic filtering.
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


        /**
         * @brief Destructor.
         * 
         */
        ~entity_table()
        {
            for (u32 i = 0; i < _entity_updaters.size(); i++)
                delete _entity_updaters[i];
            for (u32 i = 0; i < _table_updaters.size(); i++)
                delete _table_updaters[i];
        }

    };

}

#endif