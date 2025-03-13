#include "cs_u_scaling.h"

#include "bn_keypad.h"


cs::u_scaling::u_scaling(entity_table& t) :
    entity_updater::entity_updater(t, tags::SCALING)
{
    
}

bool cs::u_scaling::select(entity_model model)
{
    return table.uints.has<fields::SCALE, fields::SCALE_SZ>(model);
}

void cs::u_scaling::init()
{

}

void cs::u_scaling::update(entity e)
{
    // read the fields
    uintn_t scale = table.uints.get<fields::SCALE, fields::SCALE_SZ>(e);

    // modify the scale
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
    table.uints.set<fields::SCALE, fields::SCALE_SZ>(e, scale);
}