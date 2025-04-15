#ifndef CS_U_ANIMATION_H
#define CS_U_ANIMATION_H

#include "cs_definitions.h"
#include "esa_components.h"


namespace cs
{
    /**
     * @brief This updater takes care of udpating
     * the animation for entities with ANIM_SET component.
     * 
     */
    class u_animation : public esa::entity_updater<64>
    {
        entity_table & table;

        public:

        u_animation(entity_table& t);
        bool select(entity e) override;
        void init() override;
        void update() override;
    };
}

#endif