#include "tg_u_orbit.h"

#include "bn_math.h"

tg::u_orbit::u_orbit(entity_table & t)
    : entity_updater::entity_updater(tags::UPDATE_ORBIT),
    table(t)
{

}

bool tg::u_orbit::select(entity e)
{
    return table.has<tags::ORBIT>(e);
}

void tg::u_orbit::init()
{

}

void tg::u_orbit::update(entity e)
{
    // entity components
    position & pos = table.get<position, tags::POSITION>(e);
    orbit & orb    = table.get<orbit, tags::ORBIT>(e);

    // update angle
    if (orb.angle < 360)
        orb.angle += orb.v_angular;
    else
        orb.angle = 0;

    // orbital motion: the rotation is relative to the parent's (x, y) position
    pos.x = orb.distance * bn::degrees_cos(orb.angle);
    pos.y = orb.distance * bn::degrees_sin(orb.angle);
}