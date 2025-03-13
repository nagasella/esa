#include "cs_u_rotation.h"

cs::u_rotation::u_rotation(entity_table& t) :
    entity_updater::entity_updater(t, tags::ROTATION)
{
    
}

bool cs::u_rotation::select(entity_model model)
{
    return table.intgs.has<fields::ANGLE>(model);
}

void cs::u_rotation::init()
{

}

void cs::u_rotation::update(entity e)
{
    // read the fields
    int angle = table.intgs.get<fields::ANGLE>(e);

    // modify the fields
    angle++;

    if (angle == 360)
        angle = 0;
    
    // apply rotation to sprite
    if (table.sprites.has(e))
        table.sprites.get(e).set_rotation_angle(angle);

    // update the fields
    table.intgs.set<fields::ANGLE>(e, angle);
}