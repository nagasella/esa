/**
 * @file esa_entity_updater.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_ENTITY_UPDATER_H
#define ESA_ENTITY_UPDATER_H

#include "bn_vector.h"

#include "esa.h"

namespace esa
{

    template<typename Table>
    class entity_updater
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
        entity_updater(Table& t, tag_t tag) 
            : table(t), _tag(tag)
        {
            for (u16 i = 0; i < 16; i++)
                *(_emask + i)= 0;
        }


        /**
         * @brief Get the unique tag assigned to this updater.
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
         * @brief Update each entity subscribed to this updater 
         * (the ones that satisfy the `select` clause).
         * 
         * @param e The ID of the entity.
         */
        virtual void update(entity e)
        {

        }


        /**
         * @brief Tells whether this updater currently has no entity.
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
         * @brief Subscribe an entity to this udpater, if applicable. 
         * 
         * @param e The ID of the entity.
         */
        void subscribe(entity e)
        {
            if (select(table.models.get(e)))
                *(_emask + (e >> 4)) |= (1 << (e & 15));
        }


        /**
         * @brief Unsubscribe an entity from this updater. 
         * 
         * @param e The ID of the entity.
         */
        void unsubscribe(entity e)
        {
            *(_emask + (e >> 4)) &= ~(1 << (e & 15));
        }


        /**
         * @brief Tells if an entity is currently subscribed to this updater.
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
        virtual ~entity_updater() = default;

    };


}

#endif