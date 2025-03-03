/**
 * @file esa_updaters.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_UPDATERS_H
#define ESA_UPDATERS_H

#include "esa.h"

namespace esa
{

    template<typename Table>
    class table_updater
    {

        u32 _tag;

        protected:

        /**
         * @brief A reference to the `entity_table` this updater works on.
         * 
         */
        Table& table;


        public:


        /**
         * @brief Constructor.
         * 
         * @param t A reference to the `entity_table` associated to this updater.
         * @param tag The tag to assign to this udpater.
         */
        table_updater(Table& t, u32 tag)
            : table(t), _tag(tag)
        {

        }


        /**
         * @brief Get the tag assigned to this updater.
         * 
         */
        virtual u32 tag()
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
         * @brief Perform operations on the `entitiy_table` associated to this updater.
         * 
         */
        virtual void update()
        {

        }


        /**
         * @brief Destructor.
         * 
         */
        virtual ~table_updater()
        {

        }

    };


    template<typename Table>
    class entity_updater
    {

        u32 _tag;

        u32 _mmask;
        u32 _fmask;
        u32 _imask;
        u32 _bmask;
        u32 _efmask;

        u32 _emask [8];


        /**
         * @brief Tells if an entity can be subscribed to this updater.
         * 
         * @param e The ID of the entity to check.
         * @return true 
         * @return false 
         */
        bool subscribable(u32 e)
        {
            u32 model    = table.models.get(e);
            bool mmatch  = (_mmask == EM_NULL || _mmask == model);            
            bool fmatch  = table.fixed.matching(model, _fmask);
            bool imatch  = table.intgs.matching(model, _imask);
            bool bmatch  = table.bools.matching(model, _bmask);
            bool efmatch = table.enums.matching(model, _efmask);
            return mmatch && fmatch && imatch && bmatch && efmatch;
        }

        
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
         * @param table A reference to the `entity_table` associated to this updater.
         * @param tag The tag to assign to this updater.
         */
        entity_updater(Table& t, u32 tag) 
            : table(t), _tag(tag)
        {
            _mmask  = EM_NULL;
            _fmask  = 0;
            _imask  = 0;
            _bmask  = 0;
            _efmask = 0;
            for (u32 i = 0; i < 8; i++)
                *(_emask + i)= 0;
        }


        /**
         * @brief Get the tag assigned to this updater.
         * 
         */
        virtual u32 tag()
        {
            return _tag;
        }


        /**
         * @brief Implement this function to specify which fields the entity needs
         * to possess in order to be processed by this udpater.
         * 
         */
        virtual void require()
        {

        }


        /**
         * @brief Initialize the updater.
         * 
         */
        virtual void init()
        {

        }


        /**
         * @brief Perform operations on each individual entity processed by this updater.
         * 
         * @param e The ID of the entity that is currently being processed.
         */
        virtual void update(u32 e)
        {

        }


        /**
         * @brief To be used inside `entity_updater::require()`, to specify that the 
         * updater requires a specific field of type `bn::fixed` from entities.
         * 
         * @tparam field The ID of the field to require.
         */
        template<u32 Field>
        void require_fixed()
        {
            _fmask |= (1 << Field);
        }


        /**
         * @brief To be used inside `require()`, to specify that the 
         * updater requires a specific field of type `int` from entities.
         * 
         * @tparam field The ID of the field to require.
         */
        template<u32 Field>
        void require_int()
        {
            _imask |= (1 << Field);
        }

        
        /**
         * @brief To be used inside `require()`, to specify that the 
         * updater requires a specific field of type `bool` from entities.
         * 
         * @tparam field The ID of the field to require.
         */
        template<u32 Field>
        void require_bool()
        {
            _bmask |= (1 << Field);
        }


        /**
         * @brief To be used inside `require()`, to specify that the 
         * updater requires a specific field of type `enum` from entities.
         * 
         * @tparam field The ID of the field to require.
         * @tparam size The size (in bits) of the field to require.
         */
        template<u32 Field, u32 Size>
        void require_enum()
        {
            _efmask |= (((1 << Size) - 1) << Field);
        }


        /**
         * @brief To be used inside `require()`, to specify that this updater 
         * will process only entities of this specific model.
         * 
         * @tparam type The ID of the model to require.
         */
        template<u32 Model>
        void require_model()
        {
            _mmask = Model;
        }


        /**
         * @brief Tells whether this updater currently has no entity.
         * 
         * @return true 
         * @return false 
         */
        bool empty()
        {
            for (int i = 0; i < 8; i++)
            {
                if ( *(_emask + i) != 0)
                    return false;
            }
            return true;
        }


        /**
         * @brief Subscribe an entity to this udpater, if applicable. 
         * This function is called automatically by `entity_table::subscribe(u32 e)`.
         * 
         * @param e The ID of the entity to subscribe.
         */
        void subscribe(u32 e)
        {
            if (subscribable(e))
            {
                *(_emask + e / 32) |= (1 << (e % 32));
            }
        }


        /**
         * @brief Unsubscribe an entity from this updater. 
         * This function is called automatically by `entity_table::destroy(u32 e)`.
         * 
         * @param e The ID of the entity.
         */
        void unsubscribe(u32 e)
        {
            *(_emask + e / 32) &= ~(1 << (e % 32));
        }


        /**
         * @brief Tells if an entity is currently subscribed to this updater.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        bool subscribed(u32 e)
        {
            return (( *(_emask + e / 32) >> (e % 32)) & 1) == 1; 
        }


        /**
         * @brief Destructor.
         * 
         */
        virtual ~entity_updater() 
        {

        }

    };


}

#endif