#ifndef TG_U_ORBIT_H
#define TG_U_ORBIT_H

#include "tg_definitions.h"

namespace tg
{
    /**
     * @brief This updater implements the orbit of each
     * star, planet and moon, relative to its parent.
     * 
     */
    class u_orbit : public entity_updater
    {
        entity_table & table;

        public:

        u_orbit(entity_table & t);
        bool select(entity e) override;
        void init() override;
        void update() override;
    };
}

#endif