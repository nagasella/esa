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

    /**
     * @brief `u16` alias
     * 
     */
    using u16 = unsigned short;

    /**
     * @brief `u32` alias.
     * 
     */
    using u32 = unsigned int;

    /**
     * @brief `entity` alias.
     * 
     */
    using entity = unsigned int;

    /**
     * @brief `entity_model` alias.
     * 
     */
    using entity_model = unsigned int;


    /**
     * @brief `tag_t` alias, used for tags.
     * 
     */
    using tag_t = unsigned int;


    /**
     * @brief `field_t` alias, used for fields IDs.
     * 
     */
    using field_t = unsigned int;


    /**
     * @brief `uintn_t` alias, used for `uintn_t` fields (`unsigned int` of `n` bits).
     * 
     */
    using uintn_t = unsigned int;


    /**
     * @brief `uintn_size` alias, used for `uintn_t` fields.
     * 
     */
    using uintn_size = unsigned int;


    /**
     * @brief The main ESA data structure. It allows to organize entities (game objects) into a table, 
     * where each row is the unique ID of an entity, and each column is a field (a potential variable for the Entity).
     * 
     * @tparam Entities The maximum number of entities (game objects) this table can store (must be > 1 and <= 256).
     * @tparam Models The maximum number of entity models (types) this table can store (must be > 1 and <= 32).
     * @tparam Fixed The maximum number of fields of type `bn::fixed` this table can store for each entity (must be < 32).
     * @tparam Ints The maximum number of fields of type `int` this table can store for each entity (must be < 32).
     */
    template<u32 Entities, u32 Models, u32 Fixed, u32 Ints>
    class entity_table;


    /**
     * @brief An updater that works on a table, but does not process specific entities.
     * 
     * @tparam Table The type of `entity_table` this updater works on.
     */
    template<typename Table>
    class table_updater;


    /**
     * @brief An updater that can process entities of a specific model, or that possess specific fields.
     * 
     * @tparam Table The type of `entity_table` this updater works on.
     */
    template<typename Table>
    class entity_updater;


    /**
     * @brief A query that caches some information for imporved performance.
     * 
     * @tparam Table The type of `entity_table` this query works on.
     */
    template<typename Table>
    class cached_query;


    /**
     * @brief An apply object that caches some information for imporved performance.
     * 
     * @tparam Table The type of `entity_table` this apply object works on.
     */
    template<typename Table>
    class cached_apply;


}

#include "esa_arrays.h"
#include "esa_entity_table.h"
#include "esa_table_updater.h"
#include "esa_entity_updater.h"
#include "esa_cached_query.h"
#include "esa_cached_apply.h"

#endif