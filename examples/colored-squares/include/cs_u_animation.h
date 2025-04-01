#ifndef CS_U_ANIMATION_H
#define CS_U_ANIMATION_H

#include "cs_definitions.h"
#include "esa_components.h"

namespace cs
{
    class u_animation : public entity_updater
    {
        entity_table & table;

        public:

        u_animation(entity_table& t);
        bool select(entity e) override;
        void init() override;
        void update(entity e) override;
    };
}

#endif