#ifndef CS_U_ANIMATION_H
#define CS_U_ANIMATION_H

#include "cs_definitions.h"

namespace cs
{
    class u_animation : public entity_updater
    {
        public:

        u_animation(entity_table& t);
        void require() override;
        void init() override;
        void update(u32 e) override;
    };
}


#endif