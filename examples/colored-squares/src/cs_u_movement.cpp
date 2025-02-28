#include "cs_u_movement.h"

cs::u_movement::u_movement(entity_table& t) :
    entity_updater::entity_updater(t, tags::MOVEMENT)
{
    
}

void cs::u_movement::require()
{
    // This updater will process only entities that have X, Y VX and VY bn::fixed fields
    require_fixed<fields::X>();
    require_fixed<fields::Y>();
    require_fixed<fields::VX>();
    require_fixed<fields::VY>();
}

void cs::u_movement::init()
{

}

void cs::u_movement::update(u32 e)
{
    // read the fields
    bn::fixed x = table.fixed.get<fields::X>(e);
    bn::fixed y = table.fixed.get<fields::Y>(e);
    bn::fixed vx = table.fixed.get<fields::VX>(e);
    bn::fixed vy = table.fixed.get<fields::VY>(e);

    // modify the fields
    x += vx;
    y += vy;

    // bounce off screen
    if (x < -120)
    {
        x = -120;
        vx *= -1;
    }
    else if (x > 120)
    {
        x = 120;
        vx *= -1;
    }
    
    if (y < -80)
    {
        y = -80;
        vy *= -1;
    }
    else if (y > 80)
    {
        y = 80;
        vy *= -1;
    }

    // sprite position
    if (table.sprites.has(e))
    {
        table.sprites.get(e).set_x(x);
        table.sprites.get(e).set_y(y);
    }

    // update the fields
    table.fixed.set<fields::X>(e, x);
    table.fixed.set<fields::Y>(e, y);
    table.fixed.set<fields::VX>(e, vx);
    table.fixed.set<fields::VY>(e, vy);
}