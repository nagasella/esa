#include "cs_u_animation.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"


cs::u_animation::u_animation(entity_table& t) :
    entity_updater::entity_updater(t, tags::ANIMATION)
{
    
}

void cs::u_animation::require()
{
    // This updater will process only entities that have animation related fields
    require_enum<fields::ANIM_CURR, fields::ANIM_CURR_SZ>();
    require_enum<fields::ANIM_FIRST, fields::ANIM_FIRST_SZ>();
    require_enum<fields::ANIM_LAST, fields::ANIM_LAST_SZ>();
    require_enum<fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>();
}

void cs::u_animation::init()
{

}

void cs::u_animation::update(u32 e)
{
    // get the value of the fields for the entity
    u32 curr  = table.enums.get<fields::ANIM_CURR, fields::ANIM_CURR_SZ>(e);
    u32 first = table.enums.get<fields::ANIM_FIRST, fields::ANIM_FIRST_SZ>(e);
    u32 last  = table.enums.get<fields::ANIM_LAST, fields::ANIM_LAST_SZ>(e);
    u32 timer = table.enums.get<fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>(e);

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
    table.enums.set<fields::ANIM_CURR, fields::ANIM_CURR_SZ>(e, curr);
    table.enums.set<fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>(e, timer);

}