#ifndef CS_U_ANIMATION_H
#define CS_U_ANIMATION_H

#include "cs_definitions.h"

namespace cs
{
    class u_animation : public entity_updater
    {
        public:

        u_animation(entity_table& t);
        bool select(entity_model model) override;
        void init() override;
        void update(entity e) override;
    };
}


#endif