#include "bn_core.h"
#include "bn_display.h"
#include "bn_optional.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_squares.h"

#include "esa.h"

// components tags
#define VELOCITY 0
#define SPRITE 1

// updaters tags
#define MOVEMENT 0

// components data structures
struct velocity
{
    bn::fixed x, y;
};

using sprite = bn::optional<bn::sprite_ptr>;

// parametrization of an entity table and its updaters
using entity_table = esa::entity_table<2, 2, 1, 0, 0>;
using entity_updater = esa::entity_updater<2>;
using entity = esa::entity;

// this updater changes the the (x, y) on-screen coordinates 
// of each entity's sprite, based on the entity's velocity
class u_movement : public entity_updater
{
    entity_table & table;

    public:

    u_movement(entity_table& t) 
        : entity_updater::entity_updater(MOVEMENT),
        table(t)
    {

    }

    // select only entities that have both a sprite and velocity component
    bool select(entity e) override
    {
        return table.has<SPRITE>(e) 
            && table.has<VELOCITY>(e);
    }

    // initialization (if needed...)
    void init() override
    {

    }

    // update each entity processed by this updater
    void update() override
    {
        for (entity e : this->subscribed())
        {
            // read the entity's components from the table
            bn::sprite_ptr & spr = table.get<sprite, SPRITE>(e).value();
            velocity & vel = table.get<velocity, VELOCITY>(e);

            // get the sprite position
            bn::fixed x = spr.x();
            bn::fixed y = spr.y();

            // bounce off screen edges
            if (x < -bn::display::width() / 2)
            {
                x = -bn::display::width() / 2;
                vel.x *= -1;
            }
            else if (x > bn::display::width() / 2)
            {
                x = bn::display::width() / 2;
                vel.x *= -1;
            }
            if (y < -bn::display::height() / 2)
            {
                y = -bn::display::height() / 2;
                vel.y *= -1;
            }
            else if (y > bn::display::height() / 2)
            {
                y = bn::display::height() / 2;
                vel.y *= -1;
            }

            // update the sprite position
            spr.set_x(x + vel.x);
            spr.set_y(y + vel.y);
        }
    }

};


// implements a query that finds all the entities moving towards the right
bool find_entities_moving_right(entity_table & table, entity e)
{
    velocity & vel = table.get<velocity, VELOCITY>(e);
    if (vel.x > 0)
        return true;
    return false;
}


int main()
{
    bn::core::init();

    // define an entity table
    entity_table table;

    // define the available components
    // for each component, a column in the table is created
    table.add_component<velocity>(VELOCITY);
    table.add_component<sprite>(SPRITE);

    // add an updater
    table.add_updater(new u_movement(table));

    // initialize all the updaters
    table.init();

    // add an entity to the table, and give it some components
    esa::entity e = table.create();
    table.add<velocity, VELOCITY>(e, { 0.5, 0.5 });
    table.add<sprite, SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.subscribe(e); // REQUIRED to subscribe the entitiy to the relevant updaters

    // add another entity to the table, and give it some components
    e = table.create();
    table.add<velocity, VELOCITY>(e, { -0.5, -0.5 });
    table.add<sprite, SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
    table.subscribe(e);
    
    while (true)
    {
        // update all the updaters previously added to this table
        table.update();

        // when A is pressed, run the query that finds entities moving 
        // towards the right, and reverse their x direction
        if (bn::keypad::a_pressed())
        {
            esa::vector<entity, 2> ids = table.query<2>(&find_entities_moving_right);
            for (entity e : ids)
            {
                velocity & vel = table.get<velocity, VELOCITY>(e);
                vel.x *= -1;
            }       
        }

        bn::core::update();
    }

}