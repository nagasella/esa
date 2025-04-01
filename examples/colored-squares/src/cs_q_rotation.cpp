#include "cs_q_rotation.h"

cs::q_rotation::q_rotation(entity_table& t) :
    cached_query::cached_query(tags::QRY_ROTATION),
    table(t)
{
    
}

bool cs::q_rotation::select(entity e)
{
    return table.has<tags::ANGLE>(e);
}

void cs::q_rotation::init()
{
    
}

bool cs::q_rotation::where(entity e)
{
    if (table.get<int, tags::ANGLE>(e) > 180)
        return true;
    return false;
}