#ifndef CS_U_MOVEMENT_H
#define CS_U_MOVEMENT_H

#include "cs_definitions.h"

namespace cs
{
    class u_movement : public entity_updater
    {
        public:

        u_movement(entity_table& t);
        void require() override;
        void init() override;
        void update(u32 e) override;
    };
}


#endif