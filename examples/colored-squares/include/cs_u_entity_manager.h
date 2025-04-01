#ifndef CS_U_ENTITY_MANAGER_H
#define CS_U_ENTITY_MANAGER_H

#include "cs_definitions.h"

namespace cs
{
    class u_entity_manager : public table_updater
    {
        entity_table & table;
        
        public:

        u_entity_manager(entity_table& t);
        void init() override;
        void update() override;
    };
}

#endif