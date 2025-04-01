#include "tg_entities.h"

#include "bn_math.h"

#include "bn_sprite_items_black_hole.h"
#include "bn_sprite_items_star.h"
#include "bn_sprite_items_planet.h"
#include "bn_sprite_items_moon.h"


void tg::entities::galaxy(entity_table & table, bn::fixed x, bn::fixed y)
{
    // create a black hole
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { x, y });
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::black_hole.create_sprite(x, y));
    table.subscribe(e);

    // add 8 solar systems orbiting around the black hole
    bn::random rnd;
    for (int a = 0; a < 8; a++)
        entities::solar_system(table, 360.0 / 8 * a, e, rnd);
        
}

void tg::entities::solar_system(entity_table & table, bn::fixed angle, entity parent, bn::random & rnd)
{
    // distance from black hole
    bn::fixed distance = rnd.get_fixed(48, 120);

    // create a star
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { 0, 0 }); // will be updated by the scenegraph
    table.add<orbit, tags::ORBIT>(e, { distance, angle, 0.1 });
    table.add<entity, tags::PARENT>(e, parent);
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::star.create_sprite(0, 0));
    table.subscribe(e);

    // set a scale for the sprite
    bn::fixed scale = 1.0 / rnd.get_int(1, 3);
    table.get<sprite, tags::SPRITE>(e).value().set_scale(scale); // also applied to children!

    // add 3 planets orbiting each star
    for (int a = 0; a < 3; a++)
        entities::planet(table, 360.0 / 3 * a, e, rnd);
}

void tg::entities::planet(entity_table & table, bn::fixed angle, entity parent, bn::random & rnd)
{
    // distance from the star
    bn::fixed distance = rnd.get_fixed(8, 16);

    // create a planet
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { 0, 0 }); // will be updated by the scenegraph
    table.add<orbit, tags::ORBIT>(e, { distance, angle, 16 / distance });
    table.add<entity, tags::PARENT>(e, parent);
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::planet.create_sprite(0, 0));
    table.subscribe(e);

    // assign sprite scale based on parent's scale
    bn::fixed scale = table.get<sprite, tags::SPRITE>(parent).value().vertical_scale();
    table.get<sprite, tags::SPRITE>(e).value().set_scale(scale);

    // create a moon for some planets
    if (rnd.get_int(100) < 40)
        entities::moon(table, 30, e, rnd);
}


void tg::entities::moon(entity_table & table, bn::fixed angle, entity parent, bn::random & rnd)
{
    // create a moon
    entity e = table.create();
    table.add<position, tags::POSITION>(e, { 0, 0 }); // will be updated by the scenegraph
    table.add<orbit, tags::ORBIT>(e, { 3, angle, -1.0 });
    table.add<entity, tags::PARENT>(e, parent);
    table.add<sprite, tags::SPRITE>(e, bn::sprite_items::moon.create_sprite(0, 0));
    table.subscribe(e);
}