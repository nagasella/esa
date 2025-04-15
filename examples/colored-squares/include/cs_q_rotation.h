#ifndef CS_Q_ROTATION_H
#define CS_Q_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This query retrieves the IDs of all entities 
     * that have an ANGLE component, and that have a rotation > 180 degrees.
     * 
     */
    class q_rotation : public esa::cached_query<96>
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