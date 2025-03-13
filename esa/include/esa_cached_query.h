/**
 * @file esa_cached_query.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_CACHED_QUERY_H
#define ESA_CACHED_QUERY_H

#include "bn_vector.h"

#include "esa.h"

namespace esa
{

    template<typename Table>
    class cached_query
    {
        tag_t _tag;
        u16 _emask [8];

        protected:


        /**
         * @brief A reference to the table associated to this cached query.
         * 
         */
        Table& table;


        public:


        /**
         * @brief Constructor.
         * 
         * @param table A reference to the table associated to this cached query.
         * @param tag_t A unique tag to assign to this updater.
         */
        cached_query(Table& t, tag_t tag)
            : table(t), _tag(tag)
        {
            for (u16 i = 0; i < 8; i++)
                *(_emask + i)= 0;
        }


        /**
         * @brief Get the unique tag assigned to this cached query.
         * 
         */
        tag_t tag()
        {
            return _tag;
        }


        /**
         * @brief Initialize the cached query.
         * 
         */
        virtual void init()
        {

        }


        /**
         * @brief Implements a `select` clause: filters the entities
         * based on their model or fields.
         * 
         */
        virtual bool select(entity_model model)
        {

        }


        /**
         * @brief Implements a `where` clause: filters the enetities based
         * on the values of their fields. This function is executed for each entity
         * that satisfies the `select` clause. This function should return `true` if the
         * entity satisfies the query, otherwise `false`.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        virtual bool where(entity e)
        {
            return true;
        }


        /**
         * @brief Tells whether this cached query currently has no subscribed entity.
         * 
         * @return true 
         * @return false 
         */
        bool empty()
        {
            for (u16 i = 0; i < 8; i++)
            {
                if ( *(_emask + i) != 0)
                    return false;
            }
            return true;
        }


        /**
         * @brief Subscribe an entity to this cached query, if applicable.
         * 
         * @param e The ID of the entity.
         */
        void subscribe(entity e, entity_model model)
        {
            if (select(model))
                *(_emask + (e >> 4)) |= (1 << (e & 15));
        }


        /**
         * @brief Unsubscribe an entity from this cached query.
         * 
         * @param e The ID of the entity.
         */
        void unsubscribe(entity e)
        {
            *(_emask + (e >> 4)) &= ~(1 << (e & 15));
        }


        /**
         * @brief Tells if an entity is currently subscribed to this cached query.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        bool subscribed(entity e)
        {
            return (( *(_emask + (e >> 4)) >> (e & 15)) & 1) == 1; 
        }


        /**
         * @brief Destructor.
         * 
         */
        virtual ~cached_query() = default;

    };


}

#endif