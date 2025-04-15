#ifndef CS_U_ENTITY_MANAGER_H
#define CS_U_ENTITY_MANAGER_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This updater takes care of adding/removing/modifying
     * the entities on screen based on keypad input.
     * 
     */
    class u_entity_manager : public esa::table_updater
    {
        entity_table & table;
        
        public:

        u_entity_manager(entity_table& t);
        void init() override;
        void update() override;
    };
}

#endif