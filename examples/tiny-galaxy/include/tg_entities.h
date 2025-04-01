#ifndef TG_ENTITIES_H
#define TG_ENTITIES_H

#include "tg_definitions.h"
#include "bn_random.h"

namespace tg::entities
{
    void galaxy(entity_table & table, bn::fixed x, bn::fixed y);
    void solar_system(entity_table & table, bn::fixed angle, entity parent, bn::random & rnd);
    void planet(entity_table & table, bn::fixed angle, entity parent, bn::random & rnd);
    void moon(entity_table & table, bn::fixed angle, entity parent, bn::random & rnd);
}

#endif