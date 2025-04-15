#include "cs_u_scaling.h"

#include "bn_keypad.h"


cs::u_scaling::u_scaling(entity_table& t) :
    entity_updater(tags::SCALING),
    table(t)
{
    
}

bool cs::u_scaling::select(entity e)
{
    return table.has<tags::SCALE>(e);
}

void cs::u_scaling::init()
{

}

void cs::u_scaling::update()
{
    for (entity e : this->subscribed())
    {
        sprite & spr = table.get<sprite, tags::SPRITE>(e);
        int & scale = table.get<int, tags::SCALE>(e);

        if (bn::keypad::a_pressed())
        {
            if (scale < 3)
                scale++;
            else
                scale = 1;
        }

        if (spr.has_value())
            spr.value().set_scale((bn::fixed) 1 / scale);
    }
        
}