#ifndef ESA_ENTITY_MASK_H
#define ESA_ENTITY_MASK_H

#include "esa.h"
#include <cassert>

#include "bn_log.h"

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
            for (uint32_t i = 0; i <= (Entities >> 5); i++)
                _mask[i] = 0;
        }


        /**
         * @brief Mark the entity as present in the mask.
         * 
         * @param e The ID of the entity.
         */
        void add(entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            _mask[ e >> 5 ] |= (1 << (e & 31));
        }


        /**
         * @brief Mark the entity as absent from the mask.
         * 
         * @param e The ID of the entity.
         */
        void remove(entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            _mask[e >> 5] &= ~(1 << (e & 31));
        }


        /**
         * @brief Tell if the entity is present in the mask.
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