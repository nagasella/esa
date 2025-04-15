#ifndef CS_ENTITIES_H
#define CS_ENTITIES_H

#include "cs_definitions.h"


/**
 * @brief This namespace contains functions that generate entities
 * of different type.
 * 
 */
namespace cs::entities
{
    void red_square(entity_table& table);
    void blue_square(entity_table& table);
    void yellow_square(entity_table& table);
    void flashing_square(entity_table& table);
}

#endif