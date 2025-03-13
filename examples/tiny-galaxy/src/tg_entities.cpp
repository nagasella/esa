#include "tg_entities.h"

#include "bn_math.h"

#include "bn_sprite_items_black_hole.h"
#include "bn_sprite_items_star.h"
#include "bn_sprite_items_planet.h"
#include "bn_sprite_items_moon.h"


void tg::entities::galaxy(entity_table & table, bn::fixed x, bn::fixed y)
{
    // create a black hole
    entity black_hole = table.create<models::BLACK_HOLE>();
    table.fixed.set<fields::X>(black_hole, x);
    table.fixed.set<fields::Y>(black_hole, y);
    table.sprites.set(black_hole, bn::sprite_items::black_hole.create_sprite(x, y));

    // add 8 solar systems orbiting around the black hole
    bn::random rnd;
    for (int a = 0; a < 8; a++)
        entities::solar_system(table, 360.0 / 8 * a, black_hole, rnd);
        
}

void tg::entities::solar_system(entity_table & table, bn::fixed angle, uintn_t parent, bn::random & rnd)
{
    // distance from black hole
    bn::fixed distance = rnd.get_fixed(48, 120);

    // create a star
    entity star = table.create<models::STAR>();
    table.fixed.set<fields::X>(star, 0); // will be updated by the scenegraph
    table.fixed.set<fields::Y>(star, 0); // will be updated by the scenegraph
    table.fixed.set<fields::DISTANCE>(star, distance); // relative to the black hole
    table.fixed.set<fields::ANGLE>(star, angle);
    table.fixed.set<fields::VANGULAR>(star, 0.1);
    table.uints.set<fields::PARENT, fields::PARENT_SZ>(star, parent);
    table.sprites.set(star, bn::sprite_items::star.create_sprite(0, 0));
    bn::fixed scale = 1.0 / rnd.get_int(1, 3);
    table.sprites.get(star).set_scale(scale); // also applied to children!

    // add 3 planets orbiting each star
    for (int a = 0; a < 3; a++)
        entities::planet(table, 360.0 / 3 * a, star, rnd);
}

void tg::entities::planet(entity_table & table, bn::fixed angle, uintn_t parent, bn::random & rnd)
{
    // distance from the star
    bn::fixed distance = rnd.get_fixed(8, 16);

    // create a planet
    entity planet = table.create<models::PLANET>();
    table.fixed.set<fields::X>(planet, 0); // will be updated by the scenegraph
    table.fixed.set<fields::Y>(planet, 0); // will be updated by the scenegraph
    table.fixed.set<fields::DISTANCE>(planet, distance); // relative to the star
    table.fixed.set<fields::ANGLE>(planet, angle);
    table.fixed.set<fields::VANGULAR>(planet, 16 / distance);
    table.uints.set<fields::PARENT, fields::PARENT_SZ>(planet, parent);
    table.sprites.set(planet, bn::sprite_items::planet.create_sprite(0, 0));
    bn::fixed scale = table.sprites.get(parent).vertical_scale(); // get scale from parent
    table.sprites.get(planet).set_scale(scale);

    // create a moon for some planets
    if (rnd.get_int(100) < 40)
        entities::moon(table, 30, planet, rnd);
}


void tg::entities::moon(entity_table & table, bn::fixed angle, uintn_t parent, bn::random & rnd)
{
    // create a moon
    entity moon = table.create<models::MOON>();
    table.fixed.set<fields::X>(moon, 0); // will be updated by the scenegraph
    table.fixed.set<fields::Y>(moon, 0); // will be updated by the scenegraph
    table.fixed.set<fields::DISTANCE>(moon, 3); // relative to the planet
    table.fixed.set<fields::ANGLE>(moon, angle);
    table.fixed.set<fields::VANGULAR>(moon, -1.0);
    table.uints.set<fields::PARENT, fields::PARENT_SZ>(moon, parent);
    table.sprites.set(moon, bn::sprite_items::moon.create_sprite(0, 0));
}