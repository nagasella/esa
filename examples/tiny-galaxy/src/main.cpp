#include "bn_core.h"
#include "bn_keypad.h"

#include "tg_definitions.h"
#include "tg_entities.h"
#include "tg_u_orbit.h"
#include "tg_u_scenegraph.h"
#include "tg_u_background.h"


int main()
{
    bool paused = false;

    bn::core::init();

    // create an entity table
    tg::entity_table table;

    // define some of the table's columns in IWRAM (stack)
    // this gives some performance advantage for the
    // computation of the scene graph's absolute positions
    esa::series<tg::position, 128> positions;
    esa::series<tg::entity, 128> parents;
    esa::series<tg::sprite, 128> sprites;

    table.add_series(&positions, tg::tags::POSITION);
    table.add_series(&parents, tg::tags::PARENT);
    table.add_series(&sprites, tg::tags::SPRITE);

    // define one of the columns in EWRAM 
    // (this one is not used in performance-critical tasks)
    table.add_component<tg::orbit>(tg::tags::ORBIT);
    
    // attach the updaters to the table
    table.add_updater(new tg::u_orbit(table));
    table.add_updater(new tg::u_scenegraph(table));
    table.add_updater(new tg::u_background());

    // initialize the updaters
    table.init();

    // spawn a galaxy complex
    tg::entities::galaxy(table, 0, 0);

    while (true)
    {
        // pause/unpause game
        if (bn::keypad::start_pressed())
        {
            if (!paused)
            {
                table.deactivate_all_updaters();
                paused = true;
            }
            else
            {
                table.activate_all_updaters();
                paused = false;
            }
        }

        // Update all the updaters in the table
        table.update();

        bn::core::update();
    }

}