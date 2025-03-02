#include "cs_u_rotation.h"

cs::u_rotation::u_rotation(entity_table& t) :
    entity_updater::entity_updater(t, tags::ROTATION)
{
    
}

void cs::u_rotation::require()
{
    // This updater will process only entities that have a VISIBLE int field
    require_int<fields::ANGLE>();
}

void cs::u_rotation::init()
{

}

void cs::u_rotation::update(u32 e)
{
    // read the fields
    int angle = table.intgs.get<fields::ANGLE>(e);

    // modify the fields
    angle += 1;

    if (angle == 360)
        angle = 0;
    
    // apply to sprite
    if (table.sprites.has(e))
        table.sprites.get(e).set_rotation_angle(angle);

    // update the fields
    table.intgs.set<fields::ANGLE>(e, angle);
}