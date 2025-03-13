#include "cs_u_animation.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"


cs::u_animation::u_animation(entity_table& t) :
    entity_updater::entity_updater(t, tags::ANIMATION)
{
    
}

bool cs::u_animation::select(entity_model model)
{
    return table.uints.has<fields::ANIM_CURR, fields::ANIM_CURR_SZ>(model)
        && table.uints.has<fields::ANIM_FIRST, fields::ANIM_FIRST_SZ>(model)
        && table.uints.has<fields::ANIM_LAST, fields::ANIM_LAST_SZ>(model)
        && table.uints.has<fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>(model);
}

void cs::u_animation::init()
{

}

void cs::u_animation::update(entity e)
{
    // get the value of the fields for the entity
    uintn_t curr  = table.uints.get<fields::ANIM_CURR, fields::ANIM_CURR_SZ>(e);
    uintn_t first = table.uints.get<fields::ANIM_FIRST, fields::ANIM_FIRST_SZ>(e);
    uintn_t last  = table.uints.get<fields::ANIM_LAST, fields::ANIM_LAST_SZ>(e);
    uintn_t timer = table.uints.get<fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>(e);

    // update animation timer and current animation index
    if (timer > 0)
        timer--;
    else
    {
        if (curr < last)
            curr++;
        else
            curr = first;
        timer = 10;
    }

    // apply the correct animation to the sprite
    if (timer == 0 && table.sprites.has(e))
        table.sprites.get(e).set_tiles(bn::sprite_items::squares.tiles_item(), curr);
    
    // update the fields
    table.uints.set<fields::ANIM_CURR, fields::ANIM_CURR_SZ>(e, curr);
    table.uints.set<fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>(e, timer);

}