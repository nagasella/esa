#include "cs_u_rotation.h"

cs::u_rotation::u_rotation(entity_table& t) :
    entity_updater(tags::ROTATION),
    table(t)
{
    
}

bool cs::u_rotation::select(entity e)
{
    return table.has<tags::ANGLE>(e);
}

void cs::u_rotation::init()
{

}

void cs::u_rotation::update()
{
    for (entity e : this->subscribed())
    {
        sprite & spr = table.get<sprite, tags::SPRITE>(e);
        int & angle = table.get<int, tags::ANGLE>(e);

        angle++;
        if (angle == 360)
            angle = 0;
        
        if (spr.has_value())
            spr.value().set_rotation_angle(angle);
    }
}