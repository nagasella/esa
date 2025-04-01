#include "cs_u_movement.h"

cs::u_movement::u_movement(entity_table& t) :
    entity_updater::entity_updater(tags::MOVEMENT),
    table(t)
{
    
}

bool cs::u_movement::select(entity e)
{
    return table.has<tags::POSITION>(e)
        && table.has<tags::VELOCITY>(e);
}

void cs::u_movement::init()
{

}

void cs::u_movement::update(entity e)
{
    sprite & spr   = table.get<sprite, tags::SPRITE>(e);
    position & pos = table.get<position, tags::POSITION>(e);
    velocity & vel = table.get<velocity, tags::VELOCITY>(e);

    pos.x += vel.x;
    pos.y += vel.y;

    if (pos.x < -120)
    {
        pos.x = -120;
        vel.x *= -1;
    }
    else if (pos.x > 120)
    {
        pos.x = 120;
        vel.x *= -1;
    }
    
    if (pos.y < -80)
    {
        pos.y = -80;
        vel.y *= -1;
    }
    else if (pos.y > 80)
    {
        pos.y = 80;
        vel.y *= -1;
    }

    if (spr.has_value())
    {
        spr.value().set_x(pos.x);
        spr.value().set_y(pos.y);
    }
}