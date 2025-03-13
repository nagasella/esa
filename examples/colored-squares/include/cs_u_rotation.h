#ifndef CS_U_ROTATION_H
#define CS_U_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    class u_rotation : public entity_updater
    {
        public:

        u_rotation(entity_table& t);
        bool select(entity_model model) override;
        void init() override;
        void update(entity e) override;
    };
}


#endif