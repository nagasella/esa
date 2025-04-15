#ifndef CS_U_ROTATION_H
#define CS_U_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This updater takes care of handling the
     * sprite rotation for entity with the ANGLE component.
     * 
     */
    class u_rotation : public esa::entity_updater<96>
    {
        entity_table & table;
        
        public:

        u_rotation(entity_table& t);
        bool select(entity e) override;
        void init() override;
        void update() override;
    };
}

#endif