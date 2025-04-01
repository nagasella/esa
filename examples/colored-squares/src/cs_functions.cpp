#include "cs_functions.h"


bool cs::functions::find_red_squares(entity_table& table, entity e)
{
    color & col = table.get<color, tags::COLOR>(e);
    if (col == color::RED)
        return true;
    return false;
}


bool cs::functions::find_yellow_squares_within(entity_table& table, entity e, x_boundaries& boundaries)
{
    color & col = table.get<color, tags::COLOR>(e);
    position & pos = table.get<position, tags::POSITION>(e);

    if (col == color::YELLOW && pos.x < boundaries.max && pos.x > boundaries.min)
        return true;

    return false;
}


bool cs::functions::destroy_first_blue_square(entity_table& table, entity e)
{
    color & col = table.get<color, tags::COLOR>(e);
    if (col == color::BLUE)
    {
        table.get<sprite, tags::SPRITE>(e).reset(); // deallocate sprite resources
        table.destroy(e);
        return true;
    }
    return false;
}


bool cs::functions::incr_blue_squares_velocity(entity_table& table, entity e)
{
    color & col = table.get<color, tags::COLOR>(e);

    if (col == color::BLUE)
    {
        velocity & vel = table.get<velocity, tags::VELOCITY>(e);
        
        if (vel.x > 0)
            vel.x += 1;
        else
            vel.x -= 1;
        
        if (vel.y > 0)
            vel.y += 1;
        else
            vel.y -= 1;
    }

    return false;
}


bool cs::functions::remove_all_sprites(entity_table& table, entity e)
{
    table.get<sprite, tags::SPRITE>(e).reset();
    return false;
}