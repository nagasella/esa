#ifndef TG_U_SCENEGRAPH_H
#define TG_U_SCENEGRAPH_H

#include "tg_definitions.h"

namespace tg
{
    /**
     * @brief This updater computes the absolute position
     * of each entity, absed on its parent position.
     * 
     */
    class u_scenegraph : public entity_updater
    {
        entity_table & table;
        
        public:

        u_scenegraph(entity_table & t);
        bool select(entity e) override;
        void init() override;
        void update() override;
    };
}

#endif