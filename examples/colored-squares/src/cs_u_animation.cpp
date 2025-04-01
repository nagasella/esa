#include "cs_u_animation.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"


cs::u_animation::u_animation(entity_table& t) :
    entity_updater::entity_updater(tags::ANIMATION),
    table(t)
{
    
}

bool cs::u_animation::select(entity e)
{
    return table.has<tags::ANIM_SET>(e);
}

void cs::u_animation::init()
{

}

void cs::u_animation::update(entity e)
{
    // get the components for the entity
    sprite & spr = table.get<sprite, tags::SPRITE>(e);
    uint_set & anim = table.get<uint_set, tags::ANIM_SET>(e);

    // get the values contained in the uint_set
    uint32_t curr = anim.get<tags::ANIM_CURR, tags::ANIM_CURR_SZ>();
    uint32_t first = anim.get<tags::ANIM_FIRST, tags::ANIM_FIRST_SZ>();
    uint32_t last = anim.get<tags::ANIM_LAST, tags::ANIM_LAST_SZ>();
    uint32_t timer = anim.get<tags::ANIM_TIMER, tags::ANIM_TIMER_SZ>();

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
    if (timer == 0 && spr.has_value())
        spr.value().set_tiles(bn::sprite_items::squares.tiles_item(), curr);
    
    // update the animation information in the uintn_set compoennt
    anim.set<tags::ANIM_CURR, tags::ANIM_CURR_SZ>(curr);
    anim.set<tags::ANIM_TIMER, tags::ANIM_TIMER_SZ>(timer);

}