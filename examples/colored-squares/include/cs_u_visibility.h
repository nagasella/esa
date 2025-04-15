#ifndef CS_U_VISIBILITY_H
#define CS_U_VISIBILITY_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This updater takes care of toggling
     * the visibility of entities with the VISIBLE component.
     * 
     */
    class u_visibility : public esa::entity_updater<32>
    {
        entity_table & table;

        public:

        u_visibility(entity_table& t);
        bool select(entity e) override;
        void init() override;
        void update() override;
    };
}

#endif