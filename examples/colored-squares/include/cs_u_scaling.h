#ifndef CS_U_SCALING_H
#define CS_U_SCALING_H

#include "cs_definitions.h"

namespace cs
{
    class u_scaling : public entity_updater
    {
        entity_table & table;
        
        public:

        u_scaling(entity_table& t);
        bool select(entity e) override;
        void init() override;
        void update(entity e) override;
    };
}

#endif