/**
 * @file esa_cached_apply.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_CACHED_APPLY_H
#define ESA_CACHED_APPLY_H

#include "bn_vector.h"

#include "esa.h"

namespace esa
{

    template<typename Table>
    class cached_apply
    {
        tag_t _tag;
        u16 _emask [16];

        protected:


        /**
         * @brief A reference to the table associated to this updater.
         * 
         */
        Table& table;


        public:


        /**
         * @brief Constructor.
         * 
         * @param table A reference to the table associated to this updater.
         * @param tag_t A unique tag to assign to this updater.
         */
        cached_apply(Table& t, tag_t tag) 
            : table(t), _tag(tag)
        {
            for (u16 i = 0; i < 16; i++)
                *(_emask + i)= 0;
        }


        /**
         * @brief Get the unique tag of this updater.
         * 
         */
        tag_t tag()
        {
            return _tag;
        }


        /**
         * @brief Initialize the updater.
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
         * @brief Applies the content of this function to all entities
         * that satisfy the `select` clause. This function should return `true` if
         * the execution of the apply needs to be interrupted at the current entity ID,
         * otherwise `false`.
         * 
         * @param e The ID of each entity.
         * @return true 
         * @return false 
         */
        virtual bool apply(entity e)
        {
            return true;
        }


        /**
         * @brief Tells whether this cached apply currently has no subscribed entity.
         * 
         * @return true 
         * @return false 
         */
        bool empty()
        {
            for (u16 i = 0; i < 16; i++)
            {
                if ( *(_emask + i) != 0)
                    return false;
            }
            return true;
        }


        /**
         * @brief Subscribe an entity to this cached apply, if applicable.
         * 
         * @param e The ID of the entity.
         */
        void subscribe(entity e, entity_model model)
        {
            if (select(model))
                *(_emask + (e >> 4)) |= (1 << (e & 15));
        }


        /**
         * @brief Unsubscribe an entity from this cached apply.
         * 
         * @param e The ID of the entity.
         */
        void unsubscribe(entity e)
        {
            *(_emask + (e >> 4)) &= ~(1 << (e & 15));
        }


        /**
         * @brief Tells if an entity is currently subscribed to this cached apply.
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
        virtual ~cached_apply() = default;

    };


}

#endif