#include "cs_q_rotation.h"

cs::q_rotation::q_rotation(entity_table& t) :
    cached_query::cached_query(t, tags::QRY_ROTATION)
{
    
}

bool cs::q_rotation::select(entity_model model)
{
    return table.intgs.has<fields::ANGLE>(model);
}

void cs::q_rotation::init()
{
    
}

bool cs::q_rotation::where(entity e)
{
    if (table.intgs.get<fields::ANGLE>(e) > 180)
        return true;
        
    return false;
}