#include "cs_u_visibility.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"


cs::u_visibility::u_visibility(entity_table& t) :
    entity_updater(tags::VISIBILITY),
    table(t)
{
    
}

bool cs::u_visibility::select(entity e)
{
    return table.has<tags::VISIBLE>(e);
}

void cs::u_visibility::init()
{

}

void cs::u_visibility::update()
{
    for (entity e : this->subscribed())
    {
        sprite & spr   = table.get<sprite, tags::SPRITE>(e);
        bool & visible = table.get<bool, tags::VISIBLE>(e);

        if (bn::keypad::b_pressed())
        {
            if (visible)
            {
                spr.reset();
                visible = false;
            }
            else
            {
                position & pos = table.get<position, tags::POSITION>(e);
                spr = bn::sprite_items::squares.create_sprite(pos.x, pos.y);
                spr.value().set_tiles(bn::sprite_items::squares.tiles_item(), 1);
                visible = true;
            }
        }
    }
}