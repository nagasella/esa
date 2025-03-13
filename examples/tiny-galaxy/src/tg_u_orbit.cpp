#include "tg_u_orbit.h"

#include "bn_math.h"

tg::u_orbit::u_orbit(entity_table & t)
    : entity_updater::entity_updater(t, tags::ORBIT)
{

}

bool tg::u_orbit::select(entity_model model)
{
    return table.fixed.has<fields::ANGLE>(model)
        && table.fixed.has<fields::VANGULAR>(model);
}

void tg::u_orbit::init()
{

}

void tg::u_orbit::update(entity e)
{
    // get entity fields
    bn::fixed x  = table.fixed.get<fields::X>(e);
    bn::fixed y  = table.fixed.get<fields::Y>(e);
    bn::fixed d  = table.fixed.get<fields::DISTANCE>(e);
    bn::fixed a  = table.fixed.get<fields::ANGLE>(e);
    bn::fixed va = table.fixed.get<fields::VANGULAR>(e);

    // update angle
    if (a < 360)
        a += va;
    else
        a = 0;

    // orbital motion: the rotation is relative to the parent's (x, y) position
    x = d * bn::degrees_cos(a);
    y = d * bn::degrees_sin(a);

    // update fields
    table.fixed.set<fields::X>(e, x);
    table.fixed.set<fields::Y>(e, y);
    table.fixed.set<fields::ANGLE>(e, a);
}