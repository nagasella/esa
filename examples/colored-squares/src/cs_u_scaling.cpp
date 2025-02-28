#include "cs_u_scaling.h"

#include "bn_keypad.h"


cs::u_scaling::u_scaling(entity_table& t) :
    entity_updater::entity_updater(t, tags::SCALING)
{
    
}

void cs::u_scaling::require()
{
    // This updater will process only entities that have a SCALE enum field
    require_enum<fields::SCALE, fields::SCALE_SZ>();
}

void cs::u_scaling::init()
{

}

void cs::u_scaling::update(u32 e)
{
    // read the fields
    u32 scale = table.enums.get<fields::SCALE, fields::SCALE_SZ>(e);

    // modify the fields
    if (bn::keypad::a_pressed())
    {
        if (scale < 3)
            scale++;
        else
            scale = 1;
    }

    // apply to sprite
    if (table.sprites.has(e))
        table.sprites.get(e).set_scale((bn::fixed) 1 / scale);

    // update the fields
    table.enums.set<fields::SCALE, fields::SCALE_SZ>(e, scale);
}