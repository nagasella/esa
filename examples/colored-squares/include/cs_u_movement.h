#ifndef CS_U_MOVEMENT_H
#define CS_U_MOVEMENT_H

#include "cs_definitions.h"

namespace cs
{
    class u_movement : public entity_updater
    {
        public:

        u_movement(entity_table& t);
        bool select(entity_model model) override;
        void init() override;
        void update(entity e) override;
    };
}


#endif