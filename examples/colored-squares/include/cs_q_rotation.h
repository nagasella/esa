#ifndef CS_Q_ROTATION_H
#define CS_Q_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    class q_rotation : public cached_query
    {
        entity_table & table;

        public:

        q_rotation(entity_table & t);
        bool select(entity e) override;
        void init() override;
        bool where(entity e) override;
    };
}

#endif