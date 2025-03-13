#ifndef TG_U_SCENEGRAPH_H
#define TG_U_SCENEGRAPH_H

#include "tg_definitions.h"

namespace tg
{
    class u_scenegraph : public entity_updater
    {
        public:

        u_scenegraph(entity_table & t);
        bool select(entity_model model) override;
        void init() override;
        void update(entity e) override;
    };
}


#endif