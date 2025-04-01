# ESA (Entity System Advance)

ESA (Entity System Advance) is an Entity-Component-System (ECS) framework written in C++. It is mostly targetting Game Boy Advance (GBA) game development, so it does not make any use of C++ features like RTTI or exceptions. Initially, ESA was conceived to be used together with the [butano](https://github.com/GValiente/butano?tab=readme-ov-file) game engine, but since version 2.0.0 it can be used independently from it (for example, with libtonc or libgba). Also, nothing prevents to use it to develop games for other platforms other than the GBA.

Below is a basic introduction to Entity Systems, as well as a comprehensive tutorial about ESA and its features. In this repo you'll also find a few examples that cover all the main feature of ESA. All examples use butano for now. For additional questions or help, you can reach out through the [GBADev discord](https://github.com/gbadev-org/awesome-gbadev?tab=readme-ov-file#community).

## Table of contents

[1. An introduction to Entity Systems](#an-introduction-to-entity-systems)

[2. ESA's basics](#esas-basics)

[3. Using ESA in your project](#using-esa-in-your-project)

[4. A practical example](#a-practical-example)

[5. More about entity tables](#more-about-entity-tables)

[6. More about components](#more-about-components)

[7. Creating and deleting entities](#creating-and-deleting-entities)

[8. More about entity updaters](#more-about-entity-updaters)

[9. Tags and udpaters](#tags-and-updaters)

[10. Update order](#update-order)

[11. Queries](#queries)

[12. Apply](#apply)

[13. Boosting performance with ARM code (GBA specific)](#boosting-performance-with-arm-code-gba-specific)

## An introduction to Entity Systems

Entity Systems have been a hot topic in game development for several years, although there are different visions about how to implement one. ESA is inspired by the model discussed in [this series of articles from 2007](https://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (always a great read for anyone interested in the topic). It implements some of these ideas, although adapted for the GBA hardware.

The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, implementing something more similar to a database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components for the entities, and Systems are routines that process all the entities that share a common set of Components.

## ESA's basics

ESA is a compact, header-only framework (the main reason for this is that it extensively relies on C++ templates). It aims at implementing the vision described in the articles linked above, organizing the game with a logic that fundamentally resembles a _relational database_ (although here the _relational_ aspect is arguably not that important). Its main building blocks are the following:
* The _Entity table_ is the main data structure of ESA: it contains all the data about game objects, organized in a tabular format
* _Entities_ are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* _Components_ are the _column indices_ of the table, and they implement the data for each entity (each entity can own (or not) every component)
* _Updaters_ are objects that work on tables and update the components of entities. There are two types of updaters:
    * _Entity Updaters_, which work on subsets of entities in a table based on their components
    * _Table Updaters_, which do not necessarily process specific entities, but they can access a table's content freely
* _Queries_ can be performed on an entity table, in order to filter it - that is, look for specific entities based on a condition. Also, entity tables support _Apply_ operations, which consist in applying a function iteratively on the whole table (this can allow to modify all entities that satisfy a certain condition)

## Using ESA in your project

If you are planning to use ESA with the butano game engine, you can find excellent information about how to set up DevkitARM and butano [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano porject Makefile to include ESA's `include` path. 

You can also use ESA with other libraries (like libtonc, libgba, ...), and for general information about the available options and how to set up your development environment check out the [getting started](https://gbadev.net/getting-started.html) page of the awesome GBAdev community website.

ESA comes with a few example projects, all of which are based on butano for the moment. If you want to run these projects, make sure that the relative path of butano defined in their Makefile matches its location on your computer.

## A practical example

The following is a practical, compact example of usage of ESA (this is the source code of the `basics` project in the `examples` folder of this repo). It includes many of the basic ideas behind ESA.

```cpp
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
```

## More about entity tables

In order to optimize memory storage, ESA allows to parameterize entity tables in terms of (in order):

* The maximum number of entities available

* The maximum number of components available

* The maximum number of updaters

* The maximum number of [cached queries](#queries)

* The maximum number of [cached apply objects](#apply)

In the [example](#a-practical-example) above, the entity table was parameterized like this:

```cpp
using entity_table = esa::entity_table<2, 2, 1, 0, 0>;
```

This means that our `entity_table` can contain at most 2 entities, 2 components and 1 updater. Other ESA objects (namely entity updaters, cached queries and cached apply objects) need to be parametrized only in terms of the maximum number of entities. (see [queries](#queries) and [apply](#apply) sections)

## More about components

Components contain data for entities, and they represents _columns_ of the entity table: at the crossing of each column (component ID) and row (entity ID) there is the component for that entity. Out of all the components available in an entity table, each entity will own only some of them. Components can be of any data type, both simple (`int`, `bool`, ...) or complex (`struct`, etc). You may have more than one component with the same data type. Each component should have an integer index associated, called [tag](#tags-and-updaters). Such index should be unique for each component, and should be between `0` and the maximum number of components defined for your table (as explained in the [previous section](#more-about-entity-tables)). In the example above, we have:

```cpp
#define VELOCITY 0
#define SPRITE 1
```

The coresponding components are defined as follows:

```cpp
struct velocity
{
    bn::fixed x, y;
};

using sprite = bn::optional<bn::sprite_ptr>;
```

The velocity component is a `struct`, while the `sprite` component is defined as an optional butano sprite object. We then need to declare that our entity table makes these components available for entities (this is done in the `main`, after creating the entity table):

```cpp
table.add_component<velocity>(VELOCITY);
table.add_component<sprite>(SPRITE);
```

At this point, whenever an entity is created we can add to it any of these components (see section [creating and deleting entities](#creating-and-deleting-entities)). During the game loop, a reference to the component of a certain entity can be retrieved like this:

```cpp
velocity & vel = table.get<velocity, VELOCITY>(e);
```

Note that you need to used both the data type AND tag of the component in order to access it.

### ESA helper components

ESA offers two components that are meant to help with memory efficiency (with some small performance overhead): `bool_set` and `uintn_set`. The first type represents a collection of 32 `bool` values, stored in a single `unsigned int`; the second allows to store unsigned integer values of `n` bits (up to a total of 32 used bits).

As an example, a `bool_set` component can be added to a table like this:

```cpp
table.add_component<esa::bool_set>(COMPONENT_TAG);
```

Then, one of the booleans can be accessed as follows:

```cpp
bool_set & bools = table.get<bool_set, COMPONENT_TAG>(e);
bool value = bools.get<BOOL_TAG>();
```

Here, the `BOOL_TAG` is an integer from `0` to `31`, used to access the specific boolean value. Then, the value can for example be modified like this:

```cpp
bools.set<BOOL_TAG>(false);
```

The way to use components of type `uintn_t` is quite similar, and an example of that is shown in the example `colored-squares`, to store information about animations.

## Creating and deleting entities

The example program above shows how an enitty is created: 

```cpp
esa::entity e = table.create();
```

And how compoennts are added:

```cpp
table.add<velocity, VELOCITY>(e, { 0.5, 0.5 });
table.add<sprite, SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
```

After having set up all the components, there is another important step which consists in _subscribing_ the entity to all the relevant updaters, queries and apply objects:

```cpp
table.subscribe(e);
```

Make sure not to forget this step, since otherwise entities cannot be processed by ESA! You can delete an entity at any point by using:

```cpp
table.destroy(e);
```

Where `e` is the ID of the entity. Keep in mind that any resource associated to an entity (like sprites) is not deallocated automatically when you use `destory`, so you should take care of this manually. Also, after deletion, it is a good idea to break from any ongoing entity loop, or return from the update functions, to avoid indexing issues in ESA. To delete all entities in a table, simply use:

```cpp
table.clear();
```

As a note, the IDs of entities that were destroyed are cached (in EWRAM), which allows to create and destroy entities very frequently without losing performance.

## More about entity updaters

Entity updaters work only on entities that own (or not) specific components. For an entity updater to be able to filter entities correctly, you will need to implement the `select` member function. In an SQL query, `select` allows to select only certain columns from a table: similarly, in ESA it allows to filter entities based on the components they own. Take for example the code of the [example](#a-practical-example) above:

```cpp
bool select(esa::entity e) override
{
    return table.has<SPRITE>(e) 
        && table.has<VELOCITY>(e);
}
```

This tells that the entity updater will process all entities that have both the `SPRITE` and `VELOCITY` components. It is possible to use any arbitrarily complex filter, such as:

```cpp
bool select(esa::entity e) override
{
    return !table.has<COMPONENT1_TAG>(e)
        && table.has<COMPONENT2_TAG>(e)
        && table.get<component2_t, COMPONENT2_TAG>(e) == 3;
}
```

However, remember that this filter is applied _only when an entity is created_ inside the table (not at every frame). If you want to get the IDs of entities that satisfy a certain condition each frame, you can use [queries](#queries).

In general, it is up to you to get a reference to the entity table associated to the updater through its constructor, like shown in the [example above](#a-practical-example).

## Tags and updaters

Tags are unique integer identifiers used for retrieving components, updaters, [cached queries](#queries) and [cached apply objects](#apply). For components, the usage of tags was already covered in the previous sections, so in this section we will focus on updaters instead.

The tag for any updater should be passed to the constructor of the base `entity_updater` or `table_updater` class, as shown in the [practical example](#a-practical-example). Then, entity updaters can be retrieved from anywhere else in the program through their tag:

```cpp
my_updater * updater = (my_updater *) table.get_entity_updater<TAG>();
```

Or, for table updaters:

```cpp
my_updater * updater = (my_updater *) table.get_table_updater<TAG>();
```

It is possible to unsubcribe an entity from an entity updater: in this case, the entity will not be processed by the updater even if it satisfies the `select` condition:

```cpp
my_updater * updater = (my_updater *) table.get_entity_updater<TAG>();
updater->unsubscribe(e);
```

Then, the entity can be subscribed again with:

```cpp
my_updater * updater = (my_updater *) table.get_entity_updater<TAG>();
updater->subscribe(e);
```

A typical case where this could be useful is to avoid to process entities that are off-screen, in order to save computational resources. 

As shown in previous sections, when we first create an entity, we add its components and then we call:

```cpp
table.subscribe(e);
```

This corresponds to subscribing the entity to all the entity updaters that can process it (as well as [cached queries](#queries) and [cached apply objects](#apply)). Therefore, this is a fast way to subscribe the entity to all relevant updaters, queries and apply objects.

## Update order

When calling ```table.update()```, updaters are processed in order of insertion (it doesn't matter whether they are table updaters or entity updaters).

## Queries

Queries are used to obtain a the IDs of all entities that satisfy a certain condition. They return a `esa::vector` (a custom and minimal implementation of `std::vector`) containing the IDs of the entities that satisfy the query criteria. Queries can be performed on a table by using one of the overloads of its `query` member function. Different types of query are available, so let's dive in and look at them one by one.

### 1. Queries based on a user-defined function

The first type of queries are queries based on a user-defined function. This needs to be a `bool` function, which will be applied per-entity: it should take as arguments the `entity_table` and a single `entity`, and return `true` if the entity satisfies the query criteria, otherwise `false`. Here is an example of a user-defined query form the `colored-squares` example project, which finds all squares of color `RED`:

```cpp
bool cs::functions::find_red_squares(entity_table & table, entity e)
{
    color & col = table.get<color, tags::COLOR>(e);
    if (col == color::RED)
        return true;
    return false;
}
```

Then, the function can be used to query a table, generally from whithin some updater, to obtain a `esa::vector` containing all the entity IDs that satisfy the condition. For example, the query above is ran as follows:

```cpp
esa::vector<esa::entity, 128> red_squares = table.query<128>(&cs::functions::find_red_squares);
```

The size of the vector should be enough to fit all the entities that are _expected_ to fit into it. Since this is not necessarily known at compile time, you should be careful about this.

This type of query is very practical as all it requires is to define a function; however it can be a bit inefficient, since the function is executed on all the entities in the table. The next type of query (cached query) is used to tackle this problem.

### 2. Cached queries

Cached queries are queries that are defined as classes, and they can be more efficient than regular queries. Their definition is similar to the definition of entity updaters, but instead of overriding an `update` function they override a `where` function. The naming is taken from classical SQL databases: the `where` function is a `bool` function that can filter based on each entity's components, and must return `true` if the query condition is satisfied, otherwise `false`. A unique tag must be defined for each cached query, pretty much like for updaters.

Here is a simple example of cached query taken from the `colored-squares` example. The header file is:

```cpp
#ifndef CS_Q_ROTATION_H
#define CS_Q_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    class q_rotation : public cached_query
    {
        entity_table & table;

        public:

        q_rotation(entity_table& t);
        bool select(entity e) override;
        void init() override;
        bool where(entity e) override;
    };
}

#endif
```

And the corresponding `cpp` file:

```cpp
#include "cs_q_rotation.h"


cs::q_rotation::q_rotation(esa::entity_table& t) :
    cached_query::cached_query(tags::QRY_ROTATION),
    table(t)
{
    
}

// this part of the query is CACHED: the query will be performed 
// only on entities with the ANGLE component
bool cs::q_rotation::select(entity e)
{
    return table.has<tags::ANGLE>(e);
}

void cs::q_rotation::init()
{

}

// this part of the query is DYNAMIC: the query will return 
// only entities for which the current value of ANGLE is > 180 degrees
bool cs::q_rotation::where(esa::entity e)
{
    if (table.get<int, tags::ANGLE>(e) > 180)
        return true;
    return false;
}
```

An instance of the cached query created through `new` needs to be added to the entity table, like for updaters:

```cpp
table.add_query(new q_rotation(table));
```

Then, the query can be executed in a very simple way:

```cpp
esa::vector<esa::entity, 128> entities = table.query<tags::QRY_ROTATION, 128>();
```

Because of how the cached query was defined, this will return all entities that have a `ANGLE` component (as required by the `select` clause) AND for which the ANGLE is > 180 degrees (as required by the `where` clause). This roughly corresponds to an SQL query like:

```sql
select ANGLE from MY_TABLE where ANGLE > 180
```

In cached queries, the cached part of the query is the `select` part: this is ran only _once_, when the entity is first added to the table. On the other hand, the `where` part is dynamic and so it is executed every time the query is called, but only on the entities that satisfy the `select` condition. Therefore, the execution of the query will gnerally be faster compared to queries implemented in a simple function.

### 3. Dynamic queries

Dynamic queries are queries that can receive a parameter (of any type), and filter based on that parameter, meaning that their filtering condition can change dynamically during the game. Both types of query described previously can be dynamic. Here is an example of a dynamic query based on a function, taken from the `colored-squares` demo:

```cpp
bool cs::functions::find_yellow_squares_within(entity_table& table, entity e, x_boundaries& boundaries)
{
    color & col = table.get<color, tags::COLOR>(e);
    position & pos = table.get<position, tags::POSITION>(e);

    if (col == color::YELLOW && pos.x < boundaries.max && pos.x > boundaries.min)
        return true;

    return false;
}
```

In this case, the query receives an object of type `x_boundaries`, which is defined inside the `cs_queries.h` header like this:

```cpp
struct x_boundaries
{
    bn::fixed min, max;
};
```

Then, the query can be ran from within the `u_entity_manager` class like this:

```cpp
// define the x boundaries (will be passed to the query)
functions::x_boundaries boundaries = { 
    .min = -64, 
    .max = 64 
};
// run the query
esa::vector<entity, 128> yellow_squares = table.query<128, functions::x_boundaries>(&functions::find_yellow_squares_within, boundaries);
```

The example above is not particularly representative, since `boundaries.min` and `boundaries.max` are always the same here... However, in principle they could depend on some other variable.

Cached queries can be dynamic too: you can always define some varaible inside the cached query class itself, and then modify its value after retrieving the cached query object from the table at runtime:

```cpp
my_query * query = (my_query *) table.get_cached_query<tags::QUERY_TAG>();
query->parameter = something;
esa::vector<esa::entity, 64> ids = table.query<tags::QUERY_TAG, 64>();
```

## Apply

Additionally to running queries, it is possible to apply a certain function to an entire table. This is done through some overload of `esa::entity_table::apply`. Unlike `query`, `apply` does not return any result; instead, the function is used to _modify_ the table's content conditionally. 

### Types of `apply`

Just like for queries, we have two different types of `apply` implementations:
1. Apply a user-defined function to a whole table
2. Define cached apply objects that can be called at runtime

The two have the same advantages and disadvantages as queries based on functions and cached queries. Here, I will just give an example of the first type of `apply`, taken as usual from the `colored-squares` example. First we define the function to be applied:

```cpp
bool cs::functions::destroy_first_blue_square(entity_table& table, entity e)
{
    color & col = table.get<color, tags::COLOR>(e);
    if (col == color::BLUE)
    {
        table.get<sprite, tags::SPRITE>(e).reset(); // deallocate sprite resources
        table.destroy(e);
        return true;
    }
    return false;
}
```

The function must return a `bool` value: if it returns `true`, the execution of the `apply` routine is interrupted, otheriwise it continues with the next entity in the table. In the case above, we want to delete only the first entity of color `BLUE` appearing in the table, so we simply return `true` whenever we find an entity that satisfies this condition. This allows to avoid unnecessary iterations on the table. Then, the function can be ran like this:

```cpp
table.apply(&cs::functions::destroy_first_blue_square);
```

Regarding cached apply objects, they are defined pretty much like a cached queries, but the member function to override is `esa::cached_apply::apply` instead of `esa::cached_apply::where`. The `apply` member function should return `true` if the `apply` execution needs to stop, otherwise `false`. Cached apply objects also need to implement a `select` memeber function, which is the cached part of the apply.

## Boosting performance with ARM code (GBA specific)

In GBA development, if you feel like you need some performance boost it is often a good idea to compile some of your code in ARM instructions and store it in IWRAM (by default, code is compiled as Thumb and stored in ROM). The butano engine allows to generate ARM code in IWRAM by using the macro `BN_CODE_IWRAM` (check [this](https://gvaliente.github.io/butano/faq.html#faq_memory_arm_iwram) out in the butano FAQ), but you can do the same with libtonc too. We can apply this principle to updaters, queries and apply objects.

For example, take the scenegraph updater in the `tiny-galaxy` example project. We could change its header file `tg_u_scenegraph.h` as follows:

```cpp
#ifndef TG_U_SCENEGRAPH_H
#define TG_U_SCENEGRAPH_H

#include "tg_definitions.h"

namespace tg
{
    class u_scenegraph : public entity_updater
    {
        public:

        u_scenegraph(entity_table & t);
        bool select(entity e) override;
        void init() override;
        BN_CODE_IWRAM void update(entity e) override; // marked as IWRAM code
    };
}

#endif
```

Then, we can implement in `tg_u_scenegraph.cpp` all functions apart from the `update` function:

```cpp
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
```

And finally we can implement the `update` function in a file named `tg_u_scenegraph.iwram.cpp` (which should also be in the `src` folder):

```cpp
#include "tg_u_scenegraph.h"

void tg::u_scenegraph::update(entity e)
{
    position & pos = table.get<position, tags::POSITION>(e);
    bn::sprite_ptr & spr = table.get<sprite, tags::SPRITE>(e).value();
    entity parent = table.get<entity, tags::PARENT>(e);

    bn::fixed abs_x = pos.x;
    bn::fixed abs_y = pos.y;

    while (true)
    {
        position & parent_pos = table.get<position, tags::POSITION>(parent);
        abs_x += parent_pos.x;
        abs_y += parent_pos.y;

        if (!table.has<tags::PARENT>(parent))
            break;
        parent = table.get<entity, tags::PARENT>(parent);
        if (!table.has<tags::PARENT>(parent))
            break;
    }

    spr.set_x(abs_x);
    spr.set_y(abs_y);
}
```

This will give us some performance boost compared to the all-Thumb implementation. The same principle can be applied to table updaters, cached queries and cached apply objects.