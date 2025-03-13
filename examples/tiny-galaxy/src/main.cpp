#include "bn_core.h"
#include "bn_regular_bg_ptr.h"

#include "tg_definitions.h"
#include "tg_entities.h"
#include "tg_u_orbit.h"
#include "tg_u_scenegraph.h"
#include "tg_u_background.h"


int main()
{
    bn::core::init();

    // create an entity table
    tg::entity_table table;

    // define the models for this table
    table.fixed.add<tg::models::BLACK_HOLE, tg::fields::X>();
    table.fixed.add<tg::models::BLACK_HOLE, tg::fields::Y>();

    table.fixed.add<tg::models::STAR, tg::fields::X>();
    table.fixed.add<tg::models::STAR, tg::fields::Y>();
    table.fixed.add<tg::models::STAR, tg::fields::ANGLE>();
    table.fixed.add<tg::models::STAR, tg::fields::VANGULAR>();
    table.uints.add<tg::models::STAR, tg::fields::PARENT, tg::fields::PARENT_SZ>();

    table.fixed.add<tg::models::PLANET, tg::fields::X>();
    table.fixed.add<tg::models::PLANET, tg::fields::Y>();
    table.fixed.add<tg::models::PLANET, tg::fields::ANGLE>();
    table.fixed.add<tg::models::PLANET, tg::fields::VANGULAR>();
    table.uints.add<tg::models::PLANET, tg::fields::PARENT, tg::fields::PARENT_SZ>();

    table.fixed.add<tg::models::MOON, tg::fields::X>();
    table.fixed.add<tg::models::MOON, tg::fields::Y>();
    table.fixed.add<tg::models::MOON, tg::fields::ANGLE>();
    table.fixed.add<tg::models::MOON, tg::fields::VANGULAR>();
    table.uints.add<tg::models::MOON, tg::fields::PARENT, tg::fields::PARENT_SZ>();

    // attach some updaters to the table
    table.add_updater(new tg::u_orbit(table));
    table.add_updater(new tg::u_scenegraph(table));
    table.add_updater(new tg::u_background(table));

    // initialize the updaters
    table.init();

    // spawn a galaxy complex
    tg::entities::galaxy(table, 0, 0);

    while (true)
    {
        // Update all the updaters in the table
        table.update();
        bn::core::update();
    }

}