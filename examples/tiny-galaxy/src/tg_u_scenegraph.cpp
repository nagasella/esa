#include "tg_u_scenegraph.h"


tg::u_scenegraph::u_scenegraph(entity_table & t)
    : entity_updater::entity_updater(t, tags::SCENEGRAPH)
{

}

bool tg::u_scenegraph::select(entity_model model)
{
    return table.uints.has<fields::PARENT, fields::PARENT_SZ>(model)
        && table.fixed.has<fields::X>(model)
        && table.fixed.has<fields::Y>(model);
}

void tg::u_scenegraph::init()
{

}

void tg::u_scenegraph::update(entity e)
{
    // entity fields
    bn::fixed x = table.fixed.get<fields::X>(e);
    bn::fixed y = table.fixed.get<fields::Y>(e);
    uintn_t parent = table.uints.get<fields::PARENT, fields::PARENT_SZ>(e);

    // resolve the scengraph to get the absolute position of each entity
    entity_model parent_model = table.models.get(parent);
    while (true)
    {
        x += table.fixed.get<fields::X>(parent);
        y += table.fixed.get<fields::Y>(parent);
        parent = table.uints.get<fields::PARENT, fields::PARENT_SZ>(parent);
        parent_model = table.models.get(parent);
        if (!table.uints.has<fields::PARENT, fields::PARENT_SZ>(parent_model))
            break;
    }

    // the entity's sprite position will be the absolute position on screen
    table.sprites.get(e).set_x(x);
    table.sprites.get(e).set_y(y);
}