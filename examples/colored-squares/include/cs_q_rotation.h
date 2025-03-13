#ifndef CS_Q_ROTATION_H
#define CS_Q_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    class q_rotation : public cached_query
    {
        public:

        q_rotation(entity_table& t);
        bool select(entity_model model) override;
        void init() override;
        bool where(entity e) override;
    };
}


#endif