#include "tg_u_scenegraph.h"


tg::u_scenegraph::u_scenegraph(entity_table & t)
    : entity_updater::entity_updater(tags::SCENEGRAPH),
    table(t)
{

}

bool tg::u_scenegraph::select(entity e)
{
    return table.has<tags::PARENT>(e)
        && table.has<tags::POSITION>(e);
}

void tg::u_scenegraph::init()
{

}

void tg::u_scenegraph::update()
{
    for (entity e : this->subscribed())
    {
        // entity components
        position & pos = table.get<position, tags::POSITION>(e);
        bn::sprite_ptr & spr = table.get<sprite, tags::SPRITE>(e).value();
        entity parent = table.get<entity, tags::PARENT>(e);

        // resolve the scengraph to get the absolute position of each entity
        bn::fixed abs_x = pos.x;
        bn::fixed abs_y = pos.y;

        while (true)
        {
            // add the parent's relative position
            position & parent_pos = table.get<position, tags::POSITION>(parent);
            abs_x += parent_pos.x;
            abs_y += parent_pos.y;

            // if the parent does not have a parent, break
            if (!table.has<tags::PARENT>(parent))
                break;
            parent = table.get<entity, tags::PARENT>(parent);
            if (!table.has<tags::PARENT>(parent))
                break;
        }

        // the entity's sprite position will be the absolute position on screen
        spr.set_x(abs_x);
        spr.set_y(abs_y);
    }
}