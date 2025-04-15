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
    using entity = unsigned short;


    /**
     * @brief `index` alias.
     * 
     */
    using index = unsigned short;


    /**
     * @brief `tag_t` alias, used for tags.
     * 
     */
    using tag_t = unsigned short;


    /**
     * @brief A minimal, custom implementation of `std::array`.
     * 
     * @tparam Type the data type of the array elements.
     * @tparam Size The size of the array.
     */
    template<typename Type, uint32_t Size>
    class array;


    /**
     * @brief A minimal, custom implementation of `std::vector`.
     * Its maximum capacity is defined at compile time.
     * Integer indexing is privileged compared to iterator indexing,
     * however the vector allows for range-based for loops.
     * 
     * @tparam Type The data type of the vector elements.
     * @tparam MaxSize The maximum number of elements the vector can contain.
     */
    template<typename Type, uint32_t MaxSize>
    class vector;


    /**
     * @brief IWRAM, EWRAM.
     * 
     */
    enum class ram
    {
        IWRAM,
        EWRAM
    };


    /**
     * @brief Base class for `esa::series` and `esa::indexed_series`.
     * 
     */
    class iseries;


    /**
     * @brief A series is an array of components. 
     * It is used as a column inside a table.
     * 
     * @tparam ComponentType The type of the component.
     * @tparam Entities The maximum number of entities the series can work with. (same as the associated entity table)
     */
    template<typename ComponentType, uint32_t Entities>
    class series;



    /**
     * @brief An indexed series is a vector of components with custom indexing. 
     * It is used as a column inside a table.
     * 
     * @tparam ComponentType The type of the component.
     * @tparam Size The maximum number of entities the series can work with. Should be smaller than the number of entities in the table.
     */
    template<typename ComponentType, uint32_t Size>
    class indexed_series;


    /**
     * @brief Entity table is the main data structure of ESA. The columns of the tables
     * are arrays of coponents, and the rows of the tables are the entities.
     * 
     * @tparam Entities The maximum number of entities for the table.
     * @tparam Components The maximum number of components for the table.
     * @tparam Updaters The maximum number of updaters that can be attached to the table. (table updaters + entity updaters)
     * @tparam Queries The maximum number of cached queries that can be attached to the table.
     * @tparam Applys The maximum number of cached apply objects that can be attached to the table.
     */
    template<uint32_t Entities, uint32_t Components, uint32_t Updaters, uint32_t Queries, uint32_t Applys>
    class entity_table;


    /**
     * @brief Implements a bit-mask used to keep track of entities.
     * 
     */
    template<uint32_t Entities>
    class entity_mask;


    /**
     * @brief Base class for any updater.
     * 
     */
    class iupdater;


    /**
     * @brief Base class for entity updaters and index updaters.
     * 
     */
    class isubscribable_updater;


    /**
     * @brief An entity updater is an updater that works only on specific entities.
     * 
     * @tparam Entities The maximum number of entities the updater is expected to work with.
     */
    template<uint32_t Entities>
    class entity_updater;


    /**
     * @brief An index updater is an updater that works on an indexed component.
     * 
     * @tparam Size The Size of the underlying indexed series.
     */
    template<typename ComponentType, uint32_t Size>
    class index_updater;


    /**
     * @brief  A table updater is an updater that does not work on any specific entity.
     * It is used to access an entity table and perform operations on it through other means (like queries).
     * 
     */
    class table_updater;


    /**
     * @brief Base class for cached query.
     * 
     */
    class icached_query;


    /**
     * @brief A cached query is used to find entities that satisfy a specific condition.
     * 
     * @tparam Entities The maximum number of entities the query is expected to work with.
     */
    template<uint32_t Entities>
    class cached_query;


    /**
     * @brief Base class for cached apply.
     * 
     */
    class icached_apply;


    /**
     * @brief A cached apply object is used to perform operations on all entities that satisfy a certain condition.
     * Differently from an entity updater, it does not work on entities at every frame, but only at specific moments,
     * upon request (usually as a response to some other event).
     * 
     * @tparam Entities The maximum number of entities the apply object is exepcted work with.
     */
    template<uint32_t Entities>
    class cached_apply;


    /**
     * @brief A set of 32 boolean values, implmented in a single unsigned integer.
     * Can be used as a component for memory efficiency in exchange for a small performance penalty. 
     * 
     */
    class bool_set;


    /**
     * @brief A set of `unisgned int` values of `n` bits. Each of the stored values
     * can take up any number of bits (but the sum of all the bits taken up by each individual
     * entry must be <= 32). It can be used as a component for memory efficiency in exchange
     * for a small performance penalty.
     * 
     */
    class uintn_set;


}

#include "esa_array.h"
#include "esa_vector.h"
#include "esa_entity_mask.h"
#include "esa_series.h"
#include "esa_indexed_series.h"
#include "esa_entity_updater.h"
#include "esa_index_updater.h"
#include "esa_table_updater.h"
#include "esa_cached_query.h"
#include "esa_cached_apply.h"
#include "esa_entity_table.h"
#include "esa_components.h"

#endif