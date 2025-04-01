#include "tg_u_background.h"

#include "bn_regular_bg_items_bg_space.h"


tg::u_background::u_background()
    : table_updater::table_updater(tags::BACKGROUND),
    bg(bn::regular_bg_items::bg_space.create_bg(0, 0))
{

}

void tg::u_background::init()
{

}

void tg::u_background::update()
{
    // move the background
    bg.set_x(bg.x() - 0.025);
    bg.set_y(bg.y() + 0.05);
}