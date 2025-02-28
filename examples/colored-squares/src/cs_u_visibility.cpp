#include "cs_u_visibility.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"


cs::u_visibility::u_visibility(entity_table& t) :
    entity_updater::entity_updater(t, tags::VISIBILITY)
{
    
}

void cs::u_visibility::require()
{
    // This updater will process only entities that have a VISIBLE bool field
    require_bool<fields::VISIBLE>();
}

void cs::u_visibility::init()
{

}

void cs::u_visibility::update(u32 e)
{
    // read the fields
    bool visible = table.bools.get<fields::VISIBLE>(e);

    // modify the fields
    if (bn::keypad::b_pressed())
    {
        if (visible)
        {
            table.sprites.clear(e);
            visible = false;
        }
        else
        {
            bn::fixed x = table.fixed.get<fields::X>(e);
            bn::fixed y = table.fixed.get<fields::Y>(e);
            table.sprites.set(e, bn::sprite_items::squares.create_sprite(x, y));
            table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), 1);
            visible = true;
        }
    }

    // update the fields
    table.bools.set<fields::VISIBLE>(e, visible);
}