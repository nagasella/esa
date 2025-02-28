#ifndef CS_U_SCALING_H
#define CS_U_SCALING_H

#include "cs_definitions.h"

namespace cs
{
    class u_scaling : public entity_updater
    {
        public:

        u_scaling(entity_table& t);
        void require() override;
        void init() override;
        void update(u32 e) override;
    };
}


#endif