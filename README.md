# ESA (Entity System Advance)

ESA (Entity System Advance) is an Entity System (or, more precisely, an Entity Component System (ECS) framework) for Game Boy Adance (GBA) game development, written in C++. It was conceived to be used together with the [butano](https://github.com/GValiente/butano?tab=readme-ov-file) game engine, and it was designed to be memory efficient, (hopefully) fast, but first and foremost fun and easy to use.

Below is a basic introduction to Entity Systems, as well as a comprehensive tutorial about ESA and its features. In this repo you'll also find a few examples that cover all the main feature of ESA. For additional questions or help, you can reach out through the [GBADev discord](https://github.com/gbadev-org/awesome-gbadev?tab=readme-ov-file#community).

## Table of contents

[1. An introduction to Entity Systems](#an-introduction-to-entity-systems)

[2. ESA's basics](#esas-basics)

[3. Using ESA in your project](#using-esa-in-your-project)

[4. A practical example](#a-practical-example)

[5. More about fields](#more-about-fields)

[6. More about entity tables](#more-about-entity-tables)

[7. More about entity models](#more-about-entity-models)

[8. Creating and deleting entities](#creating-and-deleting-entities)

[9. More about sprites](#more-about-sprites)

[10. More about entity updaters](#more-about-entity-updaters)

[11. Updaters and tags](#updaters-and-tags)

[12. Update order](#update-order)

[14. Queries](#queries)

[15. Apply](#apply)

[13. Memory allocation of ESA objects](#memory-allocation-of-esa-objects)


## An introduction to Entity Systems

Entity Systems have been a hot topic in game development for several years, although there are different visions about how to implement one. ESA is inspired by the model discussed in [this series of articles from 2007](https://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (always a great read for anyone interested in the topic). It implements some of these ideas, although adapted for the GBA hardware.

The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, implementing something more similar to a database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components for the entities, and Systems are routines that process all the entities that share a common set of Components.

## ESA's basics

ESA is a compact, header-only framework (the main reason for this is that it extensively relies on C++ templates). It aims at implementing the vision described in the articles linked above, organizing the game with a logic that fundamentally resembles a _relational database_ (although here the _relational_ aspect is arguably not that important). Its main building blocks are the following:
* The _Entity table_ is the main data structure of ESA: it contains all the data about game objects, organized in a tabular format
* _Entities_ are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* _Fields_ are the _column indices_ of the table, and they represent individual variables that each entity can own (or not)
* _Entity Models_ are essentially the _types_ of the entities; they define which fields a certain type of entity owns, and which not
* _Updaters_ are objects that work on tables and update their content. There are two types of updaters: 
    * _Entity Updaters_, which work on subsets of entities in a table based on their fields or model
    * _Table Updaters_, which do not necessarily process specific entities, but they can access a table's content freely
* _Queries_ can be performed in order to filter a table - that is, look for specific entities based on a condition. Also, entity tables support _Apply_ operations, which consist in applying a function iteratively on the whole table (this can allow to modify all entities that satisfy a certain condition)

Admittedly, the naming convention is a bit different compared to the usual ECS convention (_fields_ instead of _components_, _updaters_ instead of _systems_, ...), although the basic idea remains the same.

## Using ESA in your project

ESA is meant to be used with the `butano` game engine, so for instructions about how to set up DevkitARM and work with butano follow the excellent information you find [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano porject Makefile to include ESA's `include` path. 

Also, if you want to run the example projects, make sure that the relative path of butano defined in their Makefile matches its location on your computer: the examples are set up in such a way that the butano repo folder should be at the same level as the ESA repo folder.

## A practical example

The following is a practical, compact example of usage of ESA (this is the source code of the `basics` project in the `examples` folder of this repo). It includes many of the basic ideas behind ESA.

```cpp
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

// Parametrization of an entity table and its updaters
using entity_table = esa::entity_table<2, 1, 2, 0>;
using entity_updater = esa::entity_updater<entity_table>;

// This updater changes the x, y coordinates of entities based on their velocity
class u_movement : public entity_updater
{

    public:

    u_movement(entity_table& t) 
        : entity_updater::entity_updater(t, MOVEMENT)
    {

    }

    // This updater will process only entities that satisfy this filter
    bool select(esa::entity_model model) override
    {
        return table.fixed.has<VX>(model)
            && table.fixed.has<VY>(model);
    }

    // Initialize
    void init() override
    {

    }

    // Update each entity processed by this updater
    void update(esa::entity e) override
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
    esa::entity e = table.create<SQUARE>();
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
```

## More about fields

Fields can be thought about as the _columns_ of an entity table. ESA allows to define four different types of fields:

* `bn::fixed` fields (fixed-point decimal values implemented in Butano) - max 32 per table
* `int` fields - max 32 per table
* `bool` fields - max 32 per table
* `uintn_t` fields (`unsigned int` of `n` bits) - see below 

Inside a table, the containers for fields of type `bn::fixed` and `int` are essentially arrays of the corresponding data type. These arrays can be accessed directly like this:

* `table.fixed`: access the array of `bn::fixed` fields
* `table.intgs`: access the array of `int` fields
* `table.bools`: access the array of `bool` fields
* `table.uints`: access the array of `uintn_t` fields

The number of `bn::fixed` and `int` fields for a table are defined as part of its template parameters, but must be <= 32. Instead, `bool` fields are implemented as individual bits inside a single `unsigned int` variable (one for each entity), and `uintn_t` fields as bitsets inside an `unsigned int`: therefore, a table can have up to 32 `bool` fields, and as many `uintn_t` fields as you can fit in 32 bits. Fields are essentially defined as integer indices, for example through `#define` statements or thorugh C++ `enums` (see the ESA `examples`). Each type of field can have its own indexing from `0` to `31`. For example, we could have something like the following:

```cpp
// indices for fields of type bn::fixed
#define VX 0
#define VY 1
// ... up to 31

// indices for fields of type int
#define TIMER 0
#define HEALTH 1
// ... up to 31

// indices for fields of type bool
#define ANIMATED 0
#define COLLECTIBLE 1
// ... up to 31

// indices of fields of type uintn_t (each requires 2 indices)
#define HEALTH 0 // first bit for HEALTH field
#define HEALTH_SIZE 2 // size of the field is 2 bits: [0:1]
#define ITEM_TYPE 2 // first bit for ITEM_TYPE field
#define ITEM_TYPE_SIZE 3 // size of the field is 3 bits: [2:4]
// ... up to 32 bits available
```

Then, these indices can be used to access fields inside the table, using the corresponding array. For example:

```cpp
table.fixed.set<VX>(e, 0);
```

Will set the `bn::fixed` field `VX` for entity `e` to 0, while:

```cpp
int timer = table.intgs.get<TIMER>(e);
```

Will obtain a copy of the value of the `int` field `TIMER` for the entity `e`.

## More about entity tables

Entity tables contain the data of game objects (entities). In order to optimize memory storage, ESA allows to parameterize entity tables in terms of:
* The maximum number of entities
* The maximum number of entity models
* The maximum number of `bn::fixed` fields
* The maximum number of `int` fields

Since an entity table generally contains heterogeneous entities (that is, entities that do not share all the fields with each other) some cells of the table may remain empty: this generates some memory waste. To compensate for this, keep in mind that:
* `bool` and `uintn_t` fields do not take much space: for both data types, there is only a single `unsigned int` variable per entity, therefore they are quite memory efficient (although the access time may be a bit slower)
* It is always possible to define more than 1 entity table, and group in the same table those entities that share many fields; however, this shouldn't be taken too far, since one of the main benefits of an Entity System is its flexibility in terms handling heterogeneous game objects

## More about entity models

An entity model represents the _type_ of an entity: it is associated to the fields the entitiy owns. When instantiating an `entity_table`, it is necessary to specify in its template parameters how many models the table supports. Each table has to support at least one model, but also not more than 32 models per table are supported for now.

It is necessary to define all the entity models for the table before starting to add entities to it. Entity models _cannot_ change during the game. To define an entity model, we just define which fields that specific model contains. It can be done as shown in the example above:

```cpp
table.fixed.add<SQUARE, VX>();
table.fixed.add<SQUARE, VY>();
```

Similarly for `int`, `bool` and `uintn_t` fields. For `uintn_t` fields, it is necessary to specify two indices, one for the field (which corresponds to its _left bit-shift_) and one for its size (in bits):

```cpp
table.uints.add<MODEL, FIELD_ID, FIELD_SIZE>();
```

## Creating and deleting entities

Entities should be created only after all entity models have been defined and all updaters have been added to the table. When an entity is created, all the fields belonging in its model should be initialized:

```cpp
esa::entity e = table.create<SQUARE>();
table.fixed.set<VX>(e, 0.5);
table.fixed.set<VY>(e, 0.5);
```

The type `esa::entity` is just an alias for `unisgned int` (just as basically any other ESA alias, like `entity_model`, `uintn_t`, `field_t`, ...).

Entities can be deleted at any point from a table by using:

```cpp
table.destroy(e);
```

Where `e` is the ID of the entity. After deletion, it is a good idea to `return` from the `update` function of the Updater, to avoid Field indexing issues. To delete all entities in a table, use:

```cpp
table.clear();
```

The IDs of entities that were destroyed are cached (in EWRAM), which allows to create and destroy entities very frequently without losing performance.

## More about sprites

Each entity can have a sprite, implemented using Butano's `bn::sprite_ptr`. Actually, sprites are implemented as `bn::optional` objects, therefore they can be addedd to and removed from entities very easily. A sprite can be added to an entity as shown in the example code above:

```cpp
table.sprites.set(e, bn::sprite_items::my_sprite.create_sprite(x, y));
```

Where `e` is the id of the entity. A sprite associated to an entity can be deleted with:

```cpp
table.sprites.clear(e);
```

## More about entity updaters

Entity updaters work only on entities of a specific model, or that own specific fields. Each table can have up to `128` associated entitity updaters (as well as `128` table updaters). For an entity updater to be able to filter the correct entities, you will need to implement the `select` member function. In an SQL query, `select` allows to select only certain columns from a table: similarly, in ESA it allows to filter the entities based on the fields they own (which are equivalent to table columns). Take for example the code of the example above:

```cpp
bool select(esa::entity_model model) override
{
    return table.fixed.has<VX>(model)
        && table.fixed.has<VY>(model);
}
```

This tells that the entity updater will process all entities that have both the `VX` and `VY` fields. It is possible to filter entities based on their entity model:

```cpp
model == A_CERTAIN_MODEL_ID
```

Or for any type of field:

```cpp
table.intgs.has<FIELD>(model)
```

```cpp
table.bools.has<FIELD>(model)
```

```cpp
table.uints.has<FIELD, FIELD_SIZE>(model);
```

In fact, it is possible to create any arbitrarily complex filter, such as:

```cpp
bool filter(esa::entity_model model) override
{
    return (table.fixed.has<FIELD_1>(model) || table.ingts.has<FIELD_2>(model))
        && !table.uints.has<FIELD_3, FIELD_3_SIZE>(model)
        && model != A_MODEL;
}
```

This kind of filtering will not be particularly expensive, because _it is only performed when a new entity is created_ inside the table: in fact, this filter only filters based on an entity's model, which is static and should not change during the game. It is however possible to _unsubscribe_ entities from an entity updater (see next section).

## Updaters and tags

Every updater (be it entity updater or table updater) has a tag associated to it. This should be passed to the constructor of the parent `entity_updater` or `table_updater` class, and it is an `unsigned int` variable that identifies the updater univocally within the Entity System. Then, entity updaters can be retrieved from anywhere else in the program using:

```cpp
my_updater * updater = (my_updater *) table.get_entity_updater<TAG>();
```

Or, for table updaters:

```cpp
my_updater * updater = (my_updater *) table.get_table_updater<TAG>();
```

For entity updaters, it is possible to unsubcribe entities, so that they will not be processed even if their entity model satisfies the `select` condition:

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

## Update order

When calling ```table.update()```, the order in which updaters are processed is: 
* First, all the entity updaters are processed, in order of insertion
* Then, all table updaters are processed, in order of insertion.

## Queries

Queries are used to obtain a list of IDs of entities that satisfy a certain condition. They return a `bn::vector` containing the IDs of the entities that satisfy the query criteria. Queries can be performed on a table by using one of the overloads of its `query` member function. Different types of query are available, so let's dive in and look at them one by one.

### 1. Queries based on a user-defined function

The first type of queries are queries based on a user-defined function. This needs to be a `bool` function, which will be applied per-entity, and it should return `true` if the entity satisfies the query criteria, otherwise `false`. Here is an example of a user-defined query form the `colored-squares` example project, which finds all entities of `RED_SQUARE` model:

```cpp
bool cs::functions::find_red_squares(esa::entity_table& table, esa::entity e)
{
    if (table.models.get(e) == models::RED_SQUARE)
        return true;
    return false;
}
```

Then, the function can be used to query a table, generally from whithin some updater, to obtain a `bn::vector` containing all the entity IDs that satisfy the condition. For example, the query above is ran as follows:

```cpp
bn::vector<esa::entity, 128> red_squares = table.query<128>(&cs::functions::find_red_squares);
```

### 2. Cached queries

Cached queries are queries that are defined as classes, and they can be more efficient than regular queries in some cases. Their definition is similar to the definition of entity updaters, but instead of implementing an `update` function they implement a `where` function. The naming is taken from classical SQL databases: the `where` function is a `bool` function that can filter based on the values of each entity's field, and must return `true` if the query condition is satisfied, otherwise `false`. A unique tag must be defined for each cached query, pretty much like for updaters.

Here is a simple example of cached query taken from the `colored-squares` example. The header file is:

```cpp
#ifndef CS_Q_ROTATION_H
#define CS_Q_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    class q_rotation : public cached_query
    {
        public:

        q_rotation(esa::entity_table& t);
        bool select(esa::entity_model model) override;
        void init() override;
        bool where(esa::entity e) override;
    };
}


#endif
```

And the corresponding `cpp` file:

```cpp
#include "cs_q_rotation.h"


cs::q_rotation::q_rotation(esa::entity_table& t) :
    cached_query::cached_query(t, tags::QRY_ROTATION)
{
    
}

// this part of the query is cached: the query will return only entities with the ANGLE field
bool cs::q_rotation::select(esa::entity_model model)
{
    return table.intgs.has<fields::ANGLE>(model);
}

void cs::q_rotation::init()
{

}

// this part of the query is dynamic: the query will return only entities for which the ANGLE is > 180 degrees
bool cs::q_rotation::where(esa::entity e)
{
    if (table.intgs.get<fields::ANGLE>(e) > 180)
        return true;
        
    return false;
}
```

After the cached query is defined, it can be executed in a very simple way:

```cpp
bn::vector<esa::entity, 128> entities = table.query<tags::QRY_ROTATION, 128>();
```

Because of how the cached query was defined, this will return all entities that have a `ROTATION` field (as required by the `select` clause) AND for which the rotation angle is > 180 degrees (as required by the `where` clause). This roughly corresponds to an SQL query like:

```sql
select ROTATION from MY_TABLE where ROTATION > 180
```

In cached queries, the cached part of the query is the `select` part, since it only depends on the entity's model (defined at compile time and should not change during the game): therefore, cached queries can be more performant than queries defined on a function especially when they implement a complex `select` part (that is, when they need to satisfy a complex condition in terms of entitiy fields or model).

### 3. Dynamic queries

Dynamic queries are queries that can receive a parameter of any type, and filter based on that parameter, meaning that their filtering condition can change dynamically during the game. Here is an example of a dynamic query taken from the `colored-squares` demo:

```cpp
bool cs::functions::find_yellow_squares_within(esa::entity_table& table, esa::entity e, x_boundaries& boundaries)
{
    esa::entity_model model = table.models.get(e);
    bn::fixed x = table.fixed.get<fields::X>(e);

    if (model == models::YELLOW_SQUARE && x < boundaries.max && x > boundaries.min)
        return true;

    return false;
}
```

In this case, the query receives an object of type `x_boundaries`, which is also inside the `cs_queries.h` header like this:

```cpp
struct x_boundaries
{
    bn::fixed min, max;
};
```

Then, the query can is ran from within the `u_entity_manager` class like this:

```cpp
// define the boundaries to use for the query
queries::x_boundaries boundaries = { 
    .min = -64, 
    .max = 64 
};
// run the query
bn::vector<esa::entity, 128> yellow_squares = table.query<128, queries::x_boundaries>(&queries::find_yellow_squares_within, boundaries);
```

The example above is not particularly representative, since `boundaries.min` and `boundaries.max` are always the same here... However, in principle they could depend on some variable.

Cached queries can be dynamic too: you can always define some varaible inside the query class itself, and then modify its value after retrieving the cached query object from the table at runtime:

```cpp
my_query * query = (my_query *) table.get_cached_query<tags::QUERY_TAG>();
query->parameter = something;
bn::vector<esa::entity, 64> ids = table.query<tags::QUERY_TAG, 64>();
```

## Apply

Additionally to running queries, it is possible to apply a certain function to an entire table. This is done through some overload of `esa::entity_table::apply`. Unlike `query`, `apply` does not return any result; instead, the function is used to _modify_ the table's content conditionally. 

### Types of `apply`

Just like for queries, we have two different types of `apply` implementations:
1. Apply a user-defined function to a whole table
2. Define cached apply objects

Also, `apply` supports passing an external parameter if needed. Since this is very similar to the query case, I will just give an example of the first type of `apply`, taken as usual from the `colored-squares` example. First we define the function to be applied:

```cpp
bool cs::functions::destroy_first_blue_square(esa::entity_table& table, esa::entity e)
{
    if (table.models.get(e) == models::BLUE_SQUARE)
    {
        table.destroy(e);
        return true;
    }
    return false;
}
```

The function must return a `bool` value: if it returns `true`, the execution of the `apply` routine is interrupted, otheriwise it continues with the next entities. In the case above, we want to delete only the first entity of type `BLUE_SQUARE` appearing in the table, therefore we simply return `true` whenever we find an entity that satisfies this condition. This allows to avoid unnecessary iterations on the table. Then, the function can be ran like this:

```cpp
table.apply(&cs::functions::destroy_first_blue_square);
```

A cached apply is defined like a cached query, but the member function to overload is `esa::cached_apply::apply` instead of `esa::cached_apply::where`. The `apply` member function should return `true` if the `apply` execution needs to stop, otherwise `false`.

### A warning about using `apply`

As a general note, it is better to avoid the abuse of the `apply` function: most of the logic of the game should sit inside the entity updaters, and `apply` should be only used in very specific instances, for example to trigger a certain event on many entities at a specific moment in the game. 

More than this, it is important to be careful when using this function, since it could produce undesired behavior, mainly due to the fact that in ESA we generally work with _local copies_ of the varaibles of each entity. Take for example the following case:

```cpp
void my_updater::update(esa::entity e)
{
    bn::fixed x = table.fixed.get<X>(e);

    if (x > 0)
        table.apply(&some_function_that_changes_x);
    
    table.fixed.set<X>(e, x);
}
```

The issue in the code above is that, even if the applied function changes the value of the field `X` for entity `e`, when we use `table.fixed.set<X>(e);` we are overwriting the modifications made by the `apply`, at least for this entitiy. The correct version of this would be:

```cpp
void my_updater::update(esa::entity e)
{
    bn::fixed x = table.fixed.get<X>(e);

    if (x > 0)
        table.apply(&function_that_changes_x);
    else
        table.fixed.set<X>(e);
}
```

Like this, we will avoid overwriting the value of the field `X` after the execution of `apply`.

## Memory allocation of ESA objects

It is a good idea to give some indications about how to allocate ESA objects. In the GBA the fastest memory available is the 32 KB IWRAM, so this is where we would like most of our game objects to live (if possible); then, we have the 256 KB of EWRAM, which is much larger but also slower. Because of this, the general guidelines are the following:

* Entity tables should be allocated in IWRAM (by creating them on the stack): this is where all the data related to our entites should live in order to ensure a fast game loop
* Updaters, cached query and cached apply objects should be allocated in EWRAM (by creating them on the heap using `new`). Obviously, because of this, their member variables will reside in EWRAM, so keep this in mind; however, the local copies of the entities' fields inside the `update` function will be IWRAM allocated, which will allow for good performance

Also useful to note that ESA makes extensive use of templates for handling stuff like entities fields and updaters tags, which have the advantage of being resolved at compile time and can improve execution speed.
