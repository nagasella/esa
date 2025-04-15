#include "cs_u_entity_manager.h"
#include "cs_entities.h"
#include "cs_functions.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"

#include "bn_log.h"

cs::u_entity_manager::u_entity_manager(entity_table& t) :
    table_updater(tags::ENTITY_MANAGER),
    table(t)
{
    
}

void cs::u_entity_manager::init()
{

}

void cs::u_entity_manager::update()
{
    // generate four new squares
    if (bn::keypad::up_pressed())
    {
        entities::red_square(table);
        entities::blue_square(table);
        entities::yellow_square(table);
        entities::flashing_square(table);
    }
    // increase the velocity of the blue squares on screen
    else if (bn::keypad::right_pressed())
    {
        table.apply(&functions::incr_blue_squares_velocity);
    }
    // delete all the squares
    else if (bn::keypad::down_pressed())
    {
        table.apply(&functions::remove_all_sprites); // deallocate sprite resources first
        table.clear(); // clear the table (destroy all entities)
    }
    // delete all the rotating squares with angle > 180 degrees
    else if (bn::keypad::left_pressed())
    {
        esa::vector<entity, 96> ids = table.query<tags::QRY_ROTATION, 96>();
        for (entity e : ids)
        {
            table.get<sprite, tags::SPRITE>(e).reset(); // deallocate sprite resources
            table.destroy(e);
        }
    }
    // freeze all the yellow squares with -64 < x < 64
    else if (bn::keypad::r_pressed())
    {
        // define the x boundaries (will be passed to the query)
        functions::x_boundaries boundaries = { 
            .min = -64, 
            .max = 64 
        };

        // run the query
        esa::vector<entity, 128> yellow_squares = table.query<128, functions::x_boundaries>(&functions::find_yellow_squares_within, boundaries);
        
        // modify all entities returned by the query
        for (entity e : yellow_squares)
        {
            velocity & vel = table.get<velocity, tags::VELOCITY>(e);
            vel.x = 0;
            vel.y = 0;
        }
    }
    // destroy the first blue square found in the table, and all the red squares
    else if (bn::keypad::l_pressed())
    {
        // destroy the first blue square
        table.apply(&functions::destroy_first_blue_square);

        // find all the red squares and destroy them
        esa::vector<entity, 128> red_squares = table.query<128>(&functions::find_red_squares);
        for (entity e : red_squares)
        {
            table.get<sprite, tags::SPRITE>(e).reset(); // deallocate sprite resources
            table.destroy(e);
        }
            
    }
}