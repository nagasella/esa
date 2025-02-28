#include "cs_entities.h"

#include "bn_sprite_items_squares.h"


void cs::entities::red_square(entity_table& table)
{
    u32 e = table.create<models::RED_SQUARE>();

    table.fixed.add<models::RED_SQUARE, fields::X>(e, 0);
    table.fixed.add<models::RED_SQUARE, fields::Y>(e, 0);
    table.fixed.add<models::RED_SQUARE, fields::VX>(e, 0.5);
    table.fixed.add<models::RED_SQUARE, fields::VY>(e, 0.5);
    table.enums.add<models::RED_SQUARE, fields::SCALE, fields::SCALE_SZ>(e, 1);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 0);
    
    table.subscribe(e);
}

void cs::entities::blue_square(entity_table& table)
{
    u32 e = table.create<models::BLUE_SQUARE>();

    table.fixed.add<models::BLUE_SQUARE, fields::X>(e, 0);
    table.fixed.add<models::BLUE_SQUARE, fields::Y>(e, 0);
    table.fixed.add<models::BLUE_SQUARE, fields::VX>(e, -0.5);
    table.fixed.add<models::BLUE_SQUARE, fields::VY>(e, 0.5);
    table.intgs.add<models::BLUE_SQUARE, fields::ANGLE>(e, 0);
    table.bools.add<models::BLUE_SQUARE, fields::VISIBLE>(e, true);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 1);
    
    table.subscribe(e);
}

void cs::entities::yellow_square(entity_table& table)
{
    u32 e = table.create<models::YELLOW_SQUARE>();

    table.fixed.add<models::YELLOW_SQUARE, fields::X>(e, 0);
    table.fixed.add<models::YELLOW_SQUARE, fields::Y>(e, 0);
    table.fixed.add<models::YELLOW_SQUARE, fields::VX>(e, -0.5);
    table.fixed.add<models::YELLOW_SQUARE, fields::VY>(e, -0.5);
    table.intgs.add<models::YELLOW_SQUARE, fields::ANGLE>(e, 0);
    table.enums.add<models::YELLOW_SQUARE, fields::SCALE, fields::SCALE_SZ>(e, 1);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 2);

    table.subscribe(e);
}