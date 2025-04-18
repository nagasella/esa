#ifndef ESA_ENTITY_MASK_H
#define ESA_ENTITY_MASK_H

#include <cassert>

#include "esa.h"


namespace esa
{
    template<uint32_t Entities>
    class entity_mask
    {
        /**
         * @brief The entity mask.
         * 
         */
        uint32_t _mask [ (Entities - 1) / 32 + 1 ];
        

        public:


        /**
         * @brief Constructor.
         * 
         */
        entity_mask()
        {
            for (uint32_t i = 0; i < ((Entities - 1) >> 5) + 1; i++)
                _mask[i] = 0;
        }


        /**
         * @brief Marks a certain entity as present.
         * 
         * @param e The ID of the entity.
         */
        void add(entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            _mask[ e >> 5 ] |= (1 << (e & 31));
        }


        /**
         * @brief Marks a certain entity as absent.
         * 
         * @param e The ID of the entity.
         */
        void remove(entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            _mask[e >> 5] &= ~(1 << (e & 31));
        }


        /**
         * @brief Tells if the entity is present in the mask.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool contains(entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            return ( (_mask[e >> 5] >> (e & 31)) & 1 ) == 1;
        }

    };
}

#endif