#include "bn_core.h"
#include "bn_display.h"
#include "esa.h"
#include "bn_sprite_items_squares.h"

// Fields of type "bn::fixed"
#define VX 0
#define VY 1

// Entity models
#define SQUARE 0

// Updaters tags
#define MOVEMENT 0

using u32 = esa::u32;

// Parametrization of an entity table and its updaters
using entity_table = esa::entity_table<2, 1, 2, 1, 1, 1>;
using entity_updater = esa::entity_updater<entity_table>;
using table_updater = esa::table_updater<entity_table>;

// This updater changes the x, y coordinates of entities based on their velocity
class u_movement : public entity_updater
{

    public:

    u_movement(entity_table& t) 
        : entity_updater::entity_updater(t, MOVEMENT)
    {

    }

    // Will process only entities with the fields defined here
    void require() override
    {
        require_fixed<VX>();
        require_fixed<VY>();
    }

    // Initialize
    void init() override
    {

    }

    // Update each entity
    void update(u32 e) override
    {
        // Read this entity's fields from the table
        bn::fixed x  = table.sprites.get(e).x();
        bn::fixed y  = table.sprites.get(e).y();
        bn::fixed vx = table.fixed.get<VX>(e);
        bn::fixed vy = table.fixed.get<VY>(e);

        // Modify the position
        x += vx;
        y += vy;

        // Bounce off screen edges
        if (x < -bn::display::width() / 2)
        {
            x = -bn::display::width() / 2;
            vx *= -1;
        }
        else if (x > bn::display::width() / 2)
        {
            x = bn::display::width() / 2;
            vx *= -1;
        }
        
        if (y < -bn::display::height() / 2)
        {
            y = -bn::display::height() / 2;
            vy *= -1;
        }
        else if (y > bn::display::height() / 2)
        {
            y = bn::display::height() / 2;
            vy *= -1;
        }

        // Update the fields for this entity
        table.sprites.get(e).set_x(x);
        table.sprites.get(e).set_y(y);
        table.fixed.set<VX>(e, vx);
        table.fixed.set<VY>(e, vy);
    }

};


int main()
{
    bn::core::init();

    // Define an entity table
    entity_table table;

    // Define an entity model
    table.fixed.add<SQUARE, VX>();
    table.fixed.add<SQUARE, VY>();

    // Set an updater
    table.add_updater(new u_movement(table));

    // Initialize all the updaters
    table.init();

    // Add an entity to the table, according to a previously defined model
    u32 e = table.create<SQUARE>();
    table.fixed.set<VX>(e, 0.5);
    table.fixed.set<VY>(e, 0.5);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));

    // Add another entity to the table
    e = table.create<SQUARE>();
    table.fixed.set<VX>(e, -0.5);
    table.fixed.set<VY>(e, -0.5);
    table.sprites.set(e, bn::sprite_items::squares.create_sprite(0, 0));

    while (true)
    {
        // Update all the updaters previously added to this table
        table.update();
        bn::core::update();
    }

}