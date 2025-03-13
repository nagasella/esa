#ifndef TG_U_ORBIT_H
#define TG_U_ORBIT_H

#include "tg_definitions.h"

namespace tg
{
    class u_orbit : public entity_updater
    {
        public:

        u_orbit(entity_table & t);
        bool select(entity_model model) override;
        void init() override;
        void update(entity e) override;
    };
}


#endif