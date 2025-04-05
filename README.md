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

[9. Update order](#update-order)

[10. Tags and udpaters](#tags-and-updaters)

[11. Query](#query)

[12. Apply](#apply)

[13. Boosting performance with ARM code](#boosting-performance-with-arm-code)

## An introduction to Entity Systems

Entity Systems have been a hot topic in game development for several years, although there are different visions about how to implement one. ESA is inspired by the model discussed in [this series of articles from 2007](https://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (always a great read for anyone interested in the topic). It implements some of these ideas, although adapted for the GBA hardware.

The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, implementing something more similar to a database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components for the entities, and Systems are routines that process all the entities that share a common set of Components. The IDs of groups of entities that share common aspects are obtained through queries on the database.

## ESA's basics

ESA is a compact, header-only framework. It aims to implement the vision described in the articles linked above, organizing the game in a way that resembles a _relational database_ (although here the _relational_ aspect is arguably not that important). Its main building blocks are the following:
* The [_Entity table_](#more-about-entity-tables) is the main data structure of ESA: it contains all the data about game objects, organized in a tabular format
* [_Entities_](#creating-and-deleting-entities) are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* [_Components_](#more-about-components) are the _column indices_ of the table, and they implement the data for each entity (each entity can own (or not) each component)
* _Updaters_ are objects that work on tables and update the components of entities. There are two types of updaters:
    * [_Entity Updaters_](#more-about-entity-updaters), which work on subsets of entities in a table based on their components
    * _Table Updaters_, which do not necessarily process specific entities, but they can access a table's content freely (for example through queries)
* [_Queries_](#query) can be performed on an entity table, in order to filter it - that is, look for specific entities based on a condition
* Also, entity tables support [_Apply_](#apply) operations, which consist in applying a function iteratively on the whole table (this can allow to modify all entities that satisfy a certain condition, at a specific moment in time)

## Using ESA in your project

If you are planning to use ESA with the butano game engine, you can find excellent information about how to set up DevkitARM and butano [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano porject Makefile to include ESA's `include` path. 

You can also use ESA with other libraries (like libtonc, libgba, ...), and for general information about the available options and how to set up your development environment check out the [getting started](https://gbadev.net/getting-started.html) page of the awesome GBAdev community website. Again, you will need to add ESA's `include` path to your project.

ESA comes with a few example projects, all of which are based on butano for the moment. Examples are always a good way to learn since you can try to tweak things and see what happens. If you want to run these projects, make sure that the relative path of butano defined in their Makefile matches its location on your computer.

## A practical example

The following is a simple practical example of usage of ESA. This is the source code of the `basics` project in the `examples` folder of this repo, and as the name says it includes many of the basic ideas behind ESA. It will be used as a reference in the next sections of this tutorial.

```cpp
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
    void update(entity e) override
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

This means that an `entity_table` can contain at most 2 entities, 2 components and 1 updater. Other ESA objects (namely entity updaters, [cached queries]((#queries)) and [cached apply objects](#apply)) need to be parametrized only in terms of the maximum number of entities.

## More about components

Components contain data for entities, and they represent the _columns_ of an entity table: at the crossing of each column (component ID) and row (entity ID) there is a component for an entity. Out of all the components available in an entity table, each entity will only own some of them. Components can be of any data type, both simple (`int`, `bool`, ...) or complex (`struct`, ...). More than one component with the same data type is allowed; however, each component needs to be identified by a unique integer index, called [tag](#tags-and-updaters). Such index should be between `0` and the maximum number of components allowed for the table (as explained in the [previous section](#more-about-entity-tables)). 

In the example above, we have defined an entity table with a maxium  of 2 components. We define their tags like this:

```cpp
#define VELOCITY 0
#define SPRITE 1
```

The coresponding components are:

```cpp
struct velocity
{
    bn::fixed x, y;
};

using sprite = bn::optional<bn::sprite_ptr>;
```

The velocity component is a `struct`, while the `sprite` component is defined as an optional butano sprite object. Then we create 2 columns in the table with these 2 datatypes:

```cpp
table.add_component<velocity>(VELOCITY);
table.add_component<sprite>(SPRITE);
```

During the game loop, a reference to the component of a certain entity can be retrieved like this:

```cpp
velocity & vel = table.get<velocity, VELOCITY>(e);
```

Where `e` is the entity ID. Note that you will need to used both the data type AND tag of the component in order to access it.

### Boosting components access performance

When you add a new column to the table as it was done above, what is actually happening is that an array of components of that data type is being allocated on the heap - which, for the GBA, means EWRAM. Although there's plenty of room in EWRAM, the access to this type of memory can often be slower compared to IWRAM access. For this reason, ESA allows to store tables columns in IWRAM as well: in order to do this, you will need to define the array of components (in practice, an `esa::series` container) on the stack, and then pass a pointer to it to the entity table. If we consider the previous example, we need to replace this part of the code:

```cpp
table.add_component<velocity>(VELOCITY); // EWRAM allocated
table.add_component<sprite>(SPRITE); // EWRAM allocated
```

With this:

```cpp
esa::series<velocity, 2> velocitites; // IWRAM allocated
esa::series<sprite, 2> sprites; // IWRAM allocated
table.add_series(&velocities, VELOCITY);
table.add_series(&sprites, SPRITE);
```

The `esa::series` template parameters are (1) the data type of the component and (2) the maximum number of entities for the associated `entity_table`. When you define your entity table, you can use a mixed approach and decide which of its columns to put in EWRAM and which to put in IWRAM. If this is not enough for you performance-wise, check out also [how to boost performance with ARM code](#boosting-performance-with-arm-code) as an additional technique to speed up your code execution. In general, my suggestion is to start with placing all your components in EWRAM, and move them in IWRAM only if you really see some performance hit, since in general most of _any_ game's logic is not really performance critical.

### ESA helper components

ESA offers two components that are meant to help with memory efficiency (with some small performance overhead): `bool_set` and `uintn_set`. The first type represents a collection of 32 `bool` values, stored in a single `unsigned int`; the second allows to store unsigned integer values of `n` bits (up to a total of 32 bits).

As an example, this is how to use a `bool_set`:

```cpp
// tag for the component
#define BOOLEANS 0 

// tags for the individual bool values
#define VALUE0 0 
#define VALUE1 1
#define VALUE2 2

// add the component to the table
esa::entity_table<1, 1, 0, 0, 0> table;
table.add_component<esa::bool_set>(BOOLEANS);

// create an entity with a esa::bool_set component
esa::entity e = table.create();
table.add<esa::bool_set, BOOLEANS>(e);

// get a reference to the component
esa::bool_set & bools = table.get<esa::bool_set, BOOLEANS>(e);

// set some boolean values
bools.set<VALUE0>(true);
bools.set<VALUE1>(false);
bools.set<VALUE2>(false);

// read the boolan values
bool value1 = bools.get<VALUE0>; // true
bool value2 = bools.get<VALUE1>; // false
bool value3 = bools.get<VALUE2>; // false
```

The way to use components of type `uintn_set` is quite similar, and an example of that is shown in the example `colored-squares` (to store information about animations).

## Creating and deleting entities

Entities can be added to a table simply by using: 

```cpp
esa::entity e = table.create();
```

Then, we can start adding components:

```cpp
table.add<velocity, VELOCITY>(e, { 0.5, 0.5 });
table.add<sprite, SPRITE>(e, bn::sprite_items::squares.create_sprite(0, 0));
```

Finally, there is a last step which consists in _subscribing_ the entity to all the relevant updaters, queries and apply objects (if this is not done, entities will not be processed so make sure not to forget it):

```cpp
table.subscribe(e);
```

You can delete an entity at any point by using:

```cpp
table.destroy(e);
```

Keep in mind that any resource associated to an entity (like sprites) is not deallocated automatically when you use `destory`, so you should take care of this manually. To delete all entities in a table, simply use:

```cpp
table.clear();
```

## More about entity updaters

Entity updaters work only on entities that own (or not) specific components. When you implement an entity updater, you will need to override the `select` member function for each entity updater. Take for example the code of the [example above](#a-practical-example):

```cpp
bool select(esa::entity e) override
{
    return table.has<SPRITE>(e) 
        && table.has<VELOCITY>(e);
}
```

This tells that the entity updater will process all entities that have both the compoennts with tags `SPRITE` and `VELOCITY`. Note that a reference to the entity table must be obtained manually from the updater's constructor It is possible to implement any arbitrarily complex filter, such as:

```cpp
bool select(esa::entity e) override
{
    return !table.has<COMPONENT1_TAG>(e)
        && table.has<COMPONENT2_TAG>(e)
        && table.get<component2_t, COMPONENT2_TAG>(e) == 3;
}
```

However, remember that this filter is applied _only when an entity is created_ inside the table (not at every frame). If you want to get the IDs of entities that satisfy a condition that changes dynamically at every frame, you should use [queries](#queries). 

Entity updaters also need to implement the `init` and `update` functions:

```cpp
void init() override
{
    // do some initialization...
}

void update(entity e) override
{
    // work on each entity that satisfies the "select" filter...
}
```

These are called when calling `table.init()` and `table.update()`. 

It is possible to _unsubcribe_ an entity from an entity updater: in this case, the entity will not be processed by the updater even if it satisfies the `select` condition:

```cpp
updater->unsubscribe(e);
```

Then, the entity can be subscribed again with:

```cpp
updater->subscribe(e);
```

A typical case where this could be useful is to avoid to process entities that are off-screen, in order to save computational resources.

## Update order

When calling ```table.update()```, updaters are processed in order of insertion (it doesn't matter whether they are table updaters or entity updaters).

## Tags and updaters

Tags are unique integer identifiers used for retrieving components, updaters, cached queries and cached apply objects. As a clarification, tags should be unique for each ESA object, not _globally_: there will be a set of unique tags for components, a different set for updaters, another set for queries and another again for apply objects. Each of this set will be numbered `0` to `whatever`. For components, the usage of tags was already covered in the previous sections, so in this section we will focus on updaters instead.

When implementing an updater, its tag should be passed to the constructor of the base `entity_updater` or `table_updater` class, as shown in the [practical example](#a-practical-example). Then, updaters can be retrieved from anywhere else in the program through their tag:

```cpp
my_updater * updater = (my_updater *) table.get_updater<UPDATER_TAG>();
```

This works all the same for [cached queries](#query) and [cached apply objects](#apply).

## Query

Queries are used to obtain a the IDs of all entities that satisfy a certain condition. They return a `esa::vector` (a custom and minimal implementation of `std::vector`) containing the IDs of the entities that satisfy the query criteria. Queries can be performed on a table by using one of the overloads of its `query` member function. Different types of query are available, so let's dive in and look at them one by one.

### 1. Queries based on a user-defined function

The first type of queries are queries based on a user-defined function. This needs to be a `bool` function, which will be applied per-entity: it should take as arguments the `entity_table` and a single `entity`, and return `true` if the entity satisfies the query criteria, otherwise `false`. An example of this type of query was provided in the [practical example](#a-practical-example) section:

```cpp
bool find_entities_moving_right(entity_table & table, entity e)
{
    velocity & vel = table.get<velocity, VELOCITY>(e);
    if (vel.x > 0)
        return true;
    return false;
}
```

Then, the function can be used to query a table, to obtain a `esa::vector` containing all the entity IDs that satisfy the condition. For example, the query above is ran as follows:

```cpp
esa::vector<entity, 2> ids = table.query<2>(&find_entities_moving_right);
```

The size of the vector should be enough to fit all the entities that are _expected_ to be found by the query: since this is not necessarily known at compile time, you should be careful about this. You can then iterate on the ids easily using a range-based for looop:

```cpp
for (entity e : ids)
{
    velocity & vel = table.get<velocity, VELOCITY>(e);
    vel.x *= -1;
}
```

This type of query is very practical as all it requires is to define a function; however it can be inefficient, since the function is applied to all the entities in the table indiscriminately. The next type of query (cached query) is used to tackle this problem and help improving performance.

### 2. Cached queries

Cached queries are queries that are defined as classes. Their definition is similar to the definition of [entity updaters](#more-about-entity-updaters), but instead of overriding an `update` function they override a `where` function (the naming is taken from classical SQL databases): this is a `bool` function that will filter based on each entity's components data, and must return `true` if the query condition is satisfied, otherwise `false`. A unique tag must be defined for each cached query, pretty much like for updaters.

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

// this part of the query is CACHED (executed only once, when an entity is added to the table): 
// the query will be performed only on entities with the ANGLE component
bool cs::q_rotation::select(entity e)
{
    return table.has<tags::ANGLE>(e);
}

void cs::q_rotation::init()
{

}

// this part of the query is DYNAMIC (executed every time the query is called): 
// the query will return only entities with ANGLE > 180 degrees
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

Then, the query can be called easily:

```cpp
esa::vector<esa::entity, 128> entities = table.query<tags::QRY_ROTATION, 128>();
```

This roughly corresponds to an SQL query like:

```sql
select ANGLE from MY_TABLE where ANGLE > 180
```

In cached queries, the cached part of the query is the `select` part: this is ran only _once_, when the entity is first added to the table. On the other hand, the `where` part is dynamic and so it is executed every time the query is called (but only on the entities that satisfy the `select` condition).

### 3. Dynamic queries

Dynamic queries are queries that can receive a parameter (of any type), and filter based on that parameter. Both types of query described previously (fucntion-based and cached queries) can be dynamic. Here is an example of a dynamic query based on a function, taken from the `colored-squares` demo:

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
my_query * query = (my_query *) table.get_query<QUERY_TAG>();
query->parameter = something;
esa::vector<esa::entity, 64> ids = table.query<QUERY_TAG, 64>();
```

## Apply

Additionally to running queries, it is possible to apply a certain function to an entire table. This is done through some overload of `esa::entity_table::apply`. Unlike `query`, `apply` does not return any result; instead, the function is used to _modify_ the table's content conditionally.

### Types of `apply`

Just like for [queries](#queries), we have two different types of `apply` implementations:
1. Apply a user-defined function to a whole table
2. Use a cached apply object that can be called at runtime

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

The function must return a `bool` value: if it returns `true`, the execution of the `apply` routine is interrupted immediately, otheriwise it continues with the next entity in the table. This approach is used in order to avoid unnecessary iterations: for example, in the case above, we want to delete only the first entity of color `BLUE` appearing in the table, so we simply return `true` whenever we find an entity that satisfies this condition.  Then, the function can be ran like this:

```cpp
table.apply(&cs::functions::destroy_first_blue_square);
```

If you want to use a cached apply object instead, you can define them pretty much like cached queries, but the member function to override is `esa::cached_apply::apply` instead of `esa::cached_apply::where`: 

```cpp
bool apply(esa::entity e) override
{
    // implement some conditional modification of the entity
}
```

The `apply` member function should return `true` if the `apply` execution needs to stop, otherwise `false`. As for updaters and queries, cached apply objects also need to implement a `select` memeber function, which is the cached part of the apply, and they need to be assigned a tag. You can call a cached apply object on a table similarly to queries:

```cpp
table.apply<CACHED_APPLY_TAG>();
```

## Boosting performance with ARM code

In GBA development, if you feel like you need some performance boost it is often a good idea to compile some of your code in ARM instructions and store it in IWRAM (by default, code is compiled as Thumb and stored in ROM). The butano engine allows to generate ARM code in IWRAM by using the macro `BN_CODE_IWRAM` (check [this](https://gvaliente.github.io/butano/faq.html#faq_memory_arm_iwram) out in the butano FAQ), but you can do the same with libtonc too. We can apply this principle to updaters, queries and apply objects.

In ESA, updaters generally implement small `update` functions that take care of very specialized tasks. This makes it easy to identify performance-critical parts of your program to compile as ARM code in IWRAM (remember that IWRAM does not have so much space). For example, if we take the `u_scenegraph` updater in the `tiny-galaxy` example project, we could change its header file `tg_u_scenegraph.h` as follows:

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

This will give some important performance boost compared to the all-Thumb implementation. The same principle can be applied to the `where` and `apply` functions of queries and apply objects, and it can also be combined with [putting components in IWRAM](#boosting-components-access-performance) to get even more performance advantages.