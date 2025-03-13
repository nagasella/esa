#ifndef TG_U_BACKGROUND_H
#define TG_U_BACKGROUND_H

#include "tg_definitions.h"

#include "bn_regular_bg_ptr.h"


namespace tg
{
    class u_background : public table_updater
    {
        bn::regular_bg_ptr bg;

        public:

        u_background(entity_table & t);
        void init() override;
        void update() override;
    };
}


#endif