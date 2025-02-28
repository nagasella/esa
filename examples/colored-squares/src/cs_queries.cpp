#include "cs_queries.h"


bool cs::queries::find_red_squares(entity_table& table, u32 e)
{
    if (table.models.get(e) == models::RED_SQUARE)
        return true;
    return false;
}


bool cs::queries::find_yellow_squares_within(entity_table& table, u32 e, x_boundaries& boundaries)
{
    u32 model = table.models.get(e);
    bn::fixed x = table.fixed.get<fields::X>(e);

    if (model == models::YELLOW_SQUARE && x < boundaries.max && x > boundaries.min)
        return true;

    return false;
}


bool cs::queries::destroy_first_blue_square(entity_table& table, u32 e)
{
    if (table.models.get(e) == models::BLUE_SQUARE)
    {
        table.destroy(e);
        return true;
    }
    return false;
}


bool cs::queries::incr_blue_squares_velocity(entity_table& table, u32 e)
{
    if (table.models.get(e) == models::BLUE_SQUARE)
    {
        bn::fixed vx = table.fixed.get<fields::VX>(e);
        bn::fixed vy = table.fixed.get<fields::VY>(e);
        
        if (vx > 0)
            table.fixed.set<fields::VX>(e, vx + 1);
        else
            table.fixed.set<fields::VX>(e, vx - 1);
        
        if (vy > 0)
            table.fixed.set<fields::VY>(e, vy + 1);
        else
            table.fixed.set<fields::VY>(e, vy - 1);
    }

    return false;
}