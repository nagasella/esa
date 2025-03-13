#include "cs_entities.h"

#include "bn_sprite_items_squares.h"


void cs::entities::red_square(entity_table& table)
{
    entity e = table.create<models::RED_SQUARE>();
    table.fixed.set<fields::X>(e, 0);
    table.fixed.set<fields::Y>(e, 0);
    table.fixed.set<fields::VX>(e, 0.5);
    table.fixed.set<fields::VY>(e, 0.5);
    table.uints.set<fields::SCALE, fields::SCALE_SZ>(e, 1);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 0);
}

void cs::entities::blue_square(entity_table& table)
{
    entity e = table.create<models::BLUE_SQUARE>();
    table.fixed.set<fields::X>(e, 0);
    table.fixed.set<fields::Y>(e, 0);
    table.fixed.set<fields::VX>(e, -0.5);
    table.fixed.set<fields::VY>(e, 0.5);
    table.intgs.set<fields::ANGLE>(e, 0);
    table.bools.set<fields::VISIBLE>(e, true);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 1);
}

void cs::entities::yellow_square(entity_table& table)
{
    entity e = table.create<models::YELLOW_SQUARE>();
    table.fixed.set<fields::X>(e, 0);
    table.fixed.set<fields::Y>(e, 0);
    table.fixed.set<fields::VX>(e, -0.5);
    table.fixed.set<fields::VY>(e, -0.5);
    table.intgs.set<fields::ANGLE>(e, 0);
    table.uints.set<fields::SCALE, fields::SCALE_SZ>(e, 1);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 2);
}

void cs::entities::flashing_square(entity_table& table)
{
    entity e = table.create<models::FLASHING_SQUARE>();
    table.fixed.set<fields::X>(e, 0);
    table.fixed.set<fields::Y>(e, 0);
    table.fixed.set<fields::VX>(e, 0.5);
    table.fixed.set<fields::VY>(e, -0.5);
    table.intgs.set<fields::ANGLE>(e, 0);
    table.uints.set<fields::SCALE, fields::SCALE_SZ>(e, 1);
    table.uints.set<fields::ANIM_CURR, fields::ANIM_CURR_SZ>(e, 0);
    table.uints.set<fields::ANIM_FIRST, fields::ANIM_FIRST_SZ>(e, 0);
    table.uints.set<fields::ANIM_LAST, fields::ANIM_LAST_SZ>(e, 2);
    table.uints.set<fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>(e, 10);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 0);
}