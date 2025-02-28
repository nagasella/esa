#ifndef CS_U_VISIBILITY_H
#define CS_U_VISIBILITY_H

#include "cs_definitions.h"

namespace cs
{
    class u_visibility : public entity_updater
    {
        public:

        u_visibility(entity_table& t);
        void require() override;
        void init() override;
        void update(u32 e) override;
    };
}


#endif