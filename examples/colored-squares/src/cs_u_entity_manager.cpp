#include "cs_u_entity_manager.h"
#include "cs_entities.h"
#include "cs_queries.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"


cs::u_entity_manager::u_entity_manager(entity_table& t) :
    table_updater::table_updater(t, tags::ENTITY_MANAGER)
{
    
}

void cs::u_entity_manager::init()
{

}

void cs::u_entity_manager::update()
{
    // generate new squares
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
        // apply a function to the table
        table.apply(&queries::incr_blue_squares_velocity);
    }
    // delete all the squares
    else if (bn::keypad::down_pressed())
    {
        // delete all entities in the table
        table.clear();
    }
    // delete all the rotating squares
    else if (bn::keypad::left_pressed())
    {
        // get all the entities processed by the updater with tag ROTATION
        bn::vector<u32, 128> rotating_squares = table.query<tags::ROTATION, 128>();

        // modify all the entities returned by the query
        for (auto e : rotating_squares)
            table.destroy(e);
    }
    // freeze all the yellow squares with -64 < x < 64
    else if (bn::keypad::r_pressed())
    {
        // will be passed as a parameter to the query
        queries::x_boundaries boundaries = { .min = -64, .max = 64 };

        // run the query
        bn::vector<u32, 128> yellow_squares = table.query<128, queries::x_boundaries>(&queries::find_yellow_squares_within, boundaries);
        
        // modify all entities returned by the query
        for (auto e : yellow_squares)
        {
            table.fixed.set<fields::VX>(e, 0);
            table.fixed.set<fields::VY>(e, 0);
        }
    }
    // Destroy the first blue square found in the table, and all the red squares
    else if (bn::keypad::l_pressed())
    {
        // destroy the first blue square
        table.apply(&queries::destroy_first_blue_square);

        // find all the red squares and destroy them
        bn::vector<u32, 128> red_squares = table.query<128>(&queries::find_red_squares);

        for (auto e : red_squares)
            table.destroy(e);
    }
}