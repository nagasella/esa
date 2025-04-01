#ifndef CS_U_VISIBILITY_H
#define CS_U_VISIBILITY_H

#include "cs_definitions.h"

namespace cs
{
    class u_visibility : public entity_updater
    {
        entity_table & table;

        public:

        u_visibility(entity_table& t);
        bool select(entity e) override;
        void init() override;
        void update(entity e) override;
    };
}

#endif