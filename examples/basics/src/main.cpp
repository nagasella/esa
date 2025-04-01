#include "bn_core.h"
#include "bn_display.h"
#include "bn_optional.h"
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

// this updater changes the x, y coordinates of each entity's sprite based on its velocity
class u_movement : public entity_updater
{
    entity_table & table;

    public:

    // get a reference to the entity table through the constructor
    u_movement(entity_table& t) 
        : entity_updater::entity_updater(MOVEMENT),
        table(t)
    {

    }

    // select only entities that have both a sprite and velocity component
    bool select(esa::entity e) override
    {
        return table.has<SPRITE>(e) 
            && table.has<VELOCITY>(e);
    }

    // initialize the updater
    void init() override
    {

    }

    // update each entity processed by this updater
    void update(esa::entity e) override
    {
        // read this entity's components from the table
        bn::sprite_ptr & spr = table.get<sprite, SPRITE>(e).value();
        velocity & vel = table.get<velocity, VELOCITY>(e);

        // sprite position
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

        // update the sprite position for this entity
        spr.set_x(x + vel.x);
        spr.set_y(y + vel.y);
    }

};


int main()
{
    bn::core::init();

    // define an entity table
    entity_table table;

    // define the available components (these are like the columns of the table)
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
        bn::core::update();
    }

}