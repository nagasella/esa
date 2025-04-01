#ifndef ESA_H
#define ESA_H


namespace esa
{

    /**
     * @brief 32 bits unsigned integer.
     * 
     */
    using uint32_t = unsigned int;

    /**
     * @brief `entity` alias.
     * 
     */
    using entity = unsigned int;


    /**
     * @brief `tag_t` alias, used for tags.
     * 
     */
    using tag_t = unsigned int;


    /**
     * @brief A minimal, custom implemnetation of `std::array`.
     * 
     * @tparam T the data type of the array elements.
     * @tparam Size The size of the array.
     */
    template<typename T, uint32_t Size>
    class array;


    /**
     * @brief A minimal, custom implementation of `std::vector`.
     * Its maximum capacity is defined at compile time.
     * Integer indexing is privileged compared to iterator indexing,
     * however the vector allows for range-based for loops.
     * 
     * @tparam T The data type of the vector elements.
     * @tparam MaxSize The maximum number of elements the vector can contain.
     */
    template<typename T, uint32_t MaxSize>
    class vector;


    /**
     * @brief Base class of `esa::series` (used for polymorphic behavior).
     * 
     * @tparam Entities 
     */
    template<uint32_t Entities>
    class iseries;


    /**
     * @brief A series is an array of components. 
     * It can be used as a column inside a table.
     * 
     * @tparam ComponentType The type of the component.
     * @tparam Entities The maximum number of entities the series can work with. (same as the associated entity table)
     */
    template<typename ComponentType, uint32_t Entities>
    class series;


    /**
     * @brief Entity table is the main data structure of ESA. Entities
     * represent row-IDs inside the table, while components are the column IDs.
     * 
     * @tparam Entities The maximum number of entities the table can work with.
     * @tparam Components The maximum number of components the table can work with.
     * @tparam Updaters The maximum number of updaters associated to this table. (table updaters + entity updaters)
     * @tparam Queries The maximum number of cached queries associated to this table.
     * @tparam Applys The maximum number of cached apply objects associated to this table.
     */
    template<uint32_t Entities, uint32_t Components, uint32_t Updaters, uint32_t Queries, uint32_t Applys>
    class entity_table;


    /**
     * @brief An entity mask.
     * 
     */
    template<uint32_t Entities>
    class entity_mask;


    /**
     * @brief Type of updater (enumaration).
     * 
     */
    enum class udpater_type
    {
        ENTITY_UPDATER,
        TABLE_UPDATER
    };


    /**
     * @brief Base class for entity updaters and table updaters (used for polymorphic behavior).
     * 
     */
    class iupdater;


    /**
     * @brief An entity updater is an updater that works only on specific entities.
     * 
     * @tparam Entities The maximum number of entities the updater will work with (must be the same as in its corresponding table).
     */
    template<uint32_t Entities>
    class entity_updater;


    /**
     * @brief  A table updater is an updater that does not work on any specific entity.
     * 
     */
    class table_updater;


    /**
     * @brief A cached query is used to find entities that satisfy a specific condition.
     * 
     * @tparam Entities The maximum number of entities the query will work with (must be the same as in its corresponding table).
     */
    template<uint32_t Entities>
    class cached_query;


    /**
     * @brief A cached apply can perform operations on entities that satisfy a specific condition.
     * 
     * @tparam Entities The maximum number of entities the apply object will work with (must be the same as in its corresponding table).
     */
    template<uint32_t Entities>
    class cached_apply;


    /**
     * @brief A set of 32 boolean values, implmented in a single unsigned integer.
     * Can be used as a component for memory efficiency in exchange for some performance loss. 
     * 
     */
    class bool_set;


    /**
     * @brief A set of `unisgned int` values of `n` bits. Each of the stored values
     * can take up any number of bits (but the sum of all the bits taken up by each individual
     * entry must be <= 32). It can be used as a component for memory efficiency in exchange
     * for some performance loss.
     * 
     */
    class uintn_set;


}

#include "esa_array.h"
#include "esa_vector.h"
#include "esa_entity_mask.h"
#include "esa_series.h"
#include "esa_entity_updater.h"
#include "esa_table_updater.h"
#include "esa_cached_query.h"
#include "esa_cached_apply.h"
#include "esa_entity_table.h"
#include "esa_components.h"

#endif