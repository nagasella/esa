/**
 * @file esa.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_H
#define ESA_H

/**
 * @brief ESA namespace.
 * 
 */
namespace esa
{

    using u32 = unsigned int;

    /**
     * @brief Indicates that no entity was found.
     * 
     */
    const u32 E_NULL  = 0xffffffff;


    /**
     * @brief Null entity model.
     * 
     */
    const u32 EM_NULL = 0xffffffff;


    /**
     * @brief The main ESA data structure. It allows to organize entities (game objects)
     * into a table, where each row is the unique ID of an entity, and each column is a field (a variable for the Entity).
     * The template must be parametrized in order to optimize its size in memory.
     * 
     * @tparam Entities The maximum number of entities (game objects) this `entity_table` can store (must be > 1 and <= 256).
     * @tparam Models The maximum number of entity models (types) this `entity_table` can store (must be > 1 and <= 32).
     * @tparam Fixed The maximum number of fields of type `bn::fixed` this `entity_table` can store for each entity (must be < 32).
     * @tparam Ints The maximum number of fields of type `int` this `entity_table` can store for each entity (must be < 32).
     * @tparam EntityUpdaters The maximum number of `entity_updater` objects for this `entity_table`.
     * @tparam TableUpdaters The maximum number of `table_updater` objects for this `entity_table`.
     */
    template<u32 Entities, u32 Models, u32 Fixed, u32 Ints, u32 EntityUpdaters, u32 TableUpdaters>
    class entity_table;


    /**
     * @brief A `table_updater` is an updater that works on an `entity_table`, but does not process specific entities.
     * It has to be parametrized with its associated `entity_table`
     * 
     * @tparam Table The type of `entity_table` this `entity_updater` works on.
     */
    template<typename Table>
    class table_updater;


    /**
     * @brief An `entity_updater` is an updater that can process entities that possess specific fields.
     * 
     * @tparam Table The type of `entity_table` this `entity_updater` works on.
     */
    template<typename Table>
    class entity_updater;


}

#include "esa_arrays.h"
#include "esa_table.h"
#include "esa_updaters.h"

#endif