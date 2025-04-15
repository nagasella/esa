#ifndef CS_U_MOVEMENT_H
#define CS_U_MOVEMENT_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This updater takes care of the movement
     * of entities on screen.
     * 
     */
    class u_movement : public esa::entity_updater<128>
    {
        entity_table & table;

        public:

        u_movement(entity_table & t);
        bool select(entity e) override;
        void init() override;
        void update() override;
    };
}

#endif