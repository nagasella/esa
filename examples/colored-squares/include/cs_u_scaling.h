#ifndef CS_U_SCALING_H
#define CS_U_SCALING_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This udpater takes care of handling the scale
     * of the sprites of entities with the SCALE compoennt.
     * 
     */
    class u_scaling : public esa::entity_updater<64>
    {
        entity_table & table;
        
        public:

        u_scaling(entity_table& t);
        bool select(entity e) override;
        void init() override;
        void update() override;
    };
}

#endif