#include "cs_entities.h"

#include "bn_sprite_items_squares.h"


void cs::entities::red_square(entity_table& table)
{
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { 0, 0 });
    table.add<velocity, tags::VELOCITY>(e, { 0.5, 0.5 });
    table.add<color, tags::COLOR>(e, color::RED);
    table.add<int, tags::SCALE>(e, 1);
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.subscribe(e);
}

void cs::entities::blue_square(entity_table& table)
{
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { 0, 0 });
    table.add<velocity, tags::VELOCITY>(e, { -0.5, 0.5 });
    table.add<color, tags::COLOR>(e, color::BLUE);
    table.add<int, tags::ANGLE>(e, 0);
    table.add<bool, tags::VISIBLE>(e, 0);
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.subscribe(e);

    table.get<sprite, tags::SPRITE>(e).value().set_tiles(bn::sprite_items::squares.tiles_item(), 1);
}

void cs::entities::yellow_square(entity_table& table)
{
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { 0, 0 });
    table.add<velocity, tags::VELOCITY>(e, { -0.5, -0.5 });
    table.add<color, tags::COLOR>(e, color::YELLOW);
    table.add<int, tags::SCALE>(e, 1);
    table.add<int, tags::ANGLE>(e, 0);
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.subscribe(e);

    table.get<sprite, tags::SPRITE>(e).value().set_tiles(bn::sprite_items::squares.tiles_item(), 2);
}

void cs::entities::flashing_square(entity_table& table)
{
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { 0, 0 });
    table.add<velocity, tags::VELOCITY>(e, { 0.5, -0.5 });
    table.add<color, tags::COLOR>(e, color::FLASHING);
    table.add<int, tags::SCALE>(e, 1);
    table.add<int, tags::ANGLE>(e, 0);
    table.add<uint_set, tags::ANIM_SET>(e, uint_set());
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.subscribe(e);

    // preparation of the uint_set component
    uint_set & anim = table.get<uint_set, tags::ANIM_SET>(e);
    anim.set<tags::ANIM_CURR, tags::ANIM_CURR_SZ>(0);
    anim.set<tags::ANIM_FIRST, tags::ANIM_FIRST_SZ>(0);
    anim.set<tags::ANIM_LAST, tags::ANIM_LAST_SZ>(2);
    anim.set<tags::ANIM_TIMER, tags::ANIM_TIMER_SZ>(10);
}