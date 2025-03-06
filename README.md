# ESA (Entity System Advance)

ESA (Entity System Advance) is an Entity System for Game Boy Adance (GBA) game development, written in C++. It was conceived to be used together with the [Butano](https://github.com/GValiente/butano?tab=readme-ov-file) game engine, and it was designed to be first and foremost fun and easy to use.

## An introduction to Entity Systems

Entity Systems have been a hot topic in game development for several years, although there are different visions about how to implement one. ESA is inspired by the model discussed in [this series of articles from 2007](https://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (always a great read for anyone interested in the topic). It implements some of these ideas, although adapted for the GBA hardware.

The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, implementing something more similar to a database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components for the entities, and Systems are routines that process all the entities that share a common set of Components.

## ESA's basics

ESA is a compact, header-only framework (the main reason for this is that it extensively relies on C++ templates). Its main building blocks are the following:
* The _Entity table_ is the main data structure of ESA: it essentially represents a collection of game objects, organized in tabular format
* _Entities_ are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* _Fields_ are the _column indices_ of the table, and they represent individual variables that each entity can own (or not)
* _Entity Models_ are essentially the _types_ of the entities; they define which fields a certain type of entity owns, and which not
* _Updaters_ are objects that work on tables and update their content. There are two types of updaters: 
    * _Entity Updaters_, which work on subsets of entities in a table based on their fields or model
    * _Table Updaters_, which do not necessarily process specific entities, but they can access a table's content freely
* _Queries_ can be performed in order to filter a table - that is, look for specific entities based on a condition. Also, entity tables support _Apply_ operations, which consist in applying a function iteratively on the whole table (this can allow to modify all entities that satisfy a certain condition)

Like this, ESA essentially tries to implement the vision described in the articles linked above, organizing the game with a logic that fundamentally resembles a _relational database_ (although here the _relational_ aspect is arguably not that important).

## Using ESA in your project

ESA is meant to be used with the Butano game engine, so for instructions about how to set up DevkitARM and work with Butano, follow the excellent information that you find [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano porject Makefile to include esa's path. Also, if you want to run the example projects, make sure that the relative path of butano defined in their Makefile matches its location on your computer: the examples are set up in such a way that the butano repo folder should be at the same level as the ESA repo folder.

## A practical example

The following is a practical example of usage of ESA (find the source code in the `examples` folder of this repo). It includes many of the basic ideas behind ESA.

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
```

The type `esa::u32` is an ESA type, alias for `unsigned int`.

## More about fields

ESA allows to define four different types of fields:

* `bn::fixed` fields (fixed-point decimal values implemented in Butano)
* `int` fields
* `bool` fields
* `enum` fields

Inside a table, the containers for fields of type `bn::fixed` and `int` are essentially arrays of the corresponding data type. The number of `bn::fixed` and `int` fields for a table are defined as part of its template parameters (in any case, this number must be <= 32). Instead, `bool` fields are implemented as individual bits inside a single `unsigned int` variable, and `enum`s as bitsets inside an `unsigned int`: therefore, a table can have up to 32 `bool` fields, and as many `enum` fields as you can fit in 32 bits.

Fields are essentially defined as indices, for example through `#define` statements or thorugh C++ `enums` (see the ESA examples). Each type of field can have its own indexing from `0` to `31`. For example, we could have something like the following:

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

// indices of fields of type enum (each requires 2 indices)
#define HEALTH 0 // first bit for HEALTH field
#define HEALTH_SIZE 2 // size of the field is 2 bits: [0:1]
#define ITEM_TYPE 2 // first bit for ITEM_TYPE field
#define ITEM_TYPE_SIZE 3 // size of the field is 3 bits: [2:4]
// ... up to 32 bits available
```

## More about entity tables

In order to optimize memory storage, ESA allows to parameterize entity tables in terms of:
* The number of entities
* The number of entity models
* The number of `bn::fixed` fields
* The number of `int` fields
* The number of entity updaters
* The number of table updaters

Since an entity table generally contains heterogeneous entities (that is, entities that do not share all the fields) some cells of the table may remain empty: this generates some memory waste. To compensate for this, keep in mind that:
* `bool` and `enum` fields do not take much space: for both data types, there is only a single `unsigned int` variable per entity, therefore they are quite memory efficient (although the access time can be a bit slower)
* It is always possible to define more than 1 entity table, and group in the same table those entities that share many fields; however, this shouldn't be taken too far, since one of the main benefits of an Entity System is its flexibility in terms handling heterogeneous game objects

## More about entity models

An entity model represents the _type_ of an entity: it is associated to the fields the entitiy owns. When instantiating an `entity_table`, it is necessary to specify in its template parameters how many models the table supports. Each table has to support at least one model, but also not more than 32 models per table are supported for now.

It is necessary to define all the entity models for the table before starting to add entities to it. This essentially means defining which fields the specific model contains. It can be done as shown in the example above:

```cpp
table.fixed.add<SQUARE, VX>();
table.fixed.add<SQUARE, VY>();
```

Similarly for `int`, `bool` and `enum` fields. For `enum` fields, it is necessary to specify two indices, one for the field (which corresponds to its _left bit-shift_) and one for its size (in bits):

```cpp
table.enums.add<MODEL, FIELD_ID, FIELD_SIZE>();
```

## Creating and deleting entities

Entities should be created only after all entity models have been defined and all updaters have been added to the table. When an entity is created, all the fields belonging in its model should be initialized:

```cpp
u32 e = table.create<SQUARE>();
table.fixed.set<VX>(e, 0.5);
table.fixed.set<VY>(e, 0.5);
```

Entities can be then deleted at any point from a table by using:

```cpp
table.destroy(e);
```

Where `e` is the ID of the entity. After deletion, it is a good idea to `return` from the `update` function of the Updater, to avoid Field indexing issues. To delete all entities in a table, use:

```cpp
table.clear();
```

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

Entity updaters should define which entity model or which fields they require. This is done by implementing the `require` function. Take for example the code of the example above:

```cpp
require_fixed<VX>();
require_fixed<VY>();
```

This tells that the entity updater will process all entities that have both the `VX` and `VY` fields. It is possible to require any type of field:

```cpp
require_int<FIELD>();
```

```cpp
require_bool<FIELD>();
```

```cpp
require_enum<FIELD, FIELD_SIZE>();
```

Note how, for `enum` fields, we need to specify both the index of the field and its size (in bits). It is also possible to require a spcific entity model (but not more than one):

```cpp
require_model<MODEL>();
```

Inside each updater class, the protected member variable `table` allows to access the entity table associated to the updater. It can be used to retrieve fields from entities and modifiy them.

## Updaters and tags

Every updater (be it entity updater or table updater) has a tag associated to it. This should be passed to the constructor of the parent `entity_updater` or `table_updater` class, and it is an `unsigned int` variable that identifies the updater univocally within the Entity System. Then, entity updaters can be retrieved from anywhere else in the program using:

```cpp
my_updater * updater = (my_updater *) table.get_entity_updater<TAG>();
```

Or, for table updaters:

```cpp
my_updater * updater = (my_updater *) table.get_table_updater<TAG>();
```

## Update order

When calling ```table.update()```, the order in which updaters are processed is: first, all the entity updaters are processed, in order of insertion; then, all table updaters are processed, in order of insertion.

## Memory allocation of ESA objects

It is a good idea to give some indications about how to allocate ESA objects. In the GBAm the fastest memory available is the 32 KB IWRAM, so this is where we would like most of our game objects to live (if possible); then, we have the 256 KB of EWRAM, which is much larger but also slower. Because of this, the general guidelines are the following:

* Entity tables should be allocated in IWRAM (by creating them on the stack): this is where all the data related to our entites should live in order to ensure a fast game loop
* Updaters should be allocated in EWRAM (by creating them on the heap). Obviously, because of this, their member variables will reside in EWRAM, so keep this in mind; however, the local copies of the entities' fields inside the `update` function will be IWRAM allocated, which will allow for good performance

Also useful to note that ESA makes extensive use of templates for handling stuff like entities fields and updaters tags, which have the advantage of being resolved at compile time and can improve execution speed.

## Queries

Queries are used to obtain a list of IDs of entities that satisfy a certain condition. They return a `bn::vector` containing the IDs of the entities. Queries can be performed on a table by using one of the overloads of its `query` member function. 

The simplest type of query is one that returns all the entities processed by a certain entity updater. The updater has to be identified by its unique tag in the function template parameters. The query can be performed like in the following example (see the `colored-suqares` example project):

```cpp
bn::vector<esa::u32, 128> = table.query<tags::ROTATION, 128>();
```

Here the number `128` is just an example: the user needs to specify the expected maximum number of IDs that will be returned by the query. In some cases, it may be possible that this is not known at compile time, so it's important to be careful about this: all the entities that do not fit into the vector will be ignored by ESA.

The second type of queries are user-defined queries. These can be defined as `bool` functions, and they take as paramters the `entity_table` on which they will work, as well as an entity ID. The query function is applied per-entity, and it should return `true` if the entity satisfies the query criteria, otherwise `false`. Here is an example of a user-defined query form the `colored-squares` example project, which finds all entities of `RED_SQUARE` type:

```cpp
bool cs::queries::find_red_squares(entity_table& table, u32 e)
{
    if (table.models.get(e) == models::RED_SQUARE)
        return true;
    return false;
}
```

Then, the function can be used to query a table, generally from whithin some updater, to obtain a `bn::vector` containing all the entity IDs that satisfy the condition. For example, the query above is ran as follows:

```cpp
bn::vector<esa::u32, 128> red_squares = table.query<128>(&cs::queries::find_red_squares);
```

The query returns a `bn::vector` with the IDs of the entities that satisfy the query condition. The size of the vector may not always be known at compile time, so it is important to be careful with this.

Sometimes, it is necessary to pass some parameter to the query in order to have a _dynamic_ kind of filtering, and this can be done by implementing those parameters into a `struct` or a `class`, which can then be passed to the query function. An example of this is also shown in the `colored-squares` example project.

## Apply

Additionally to the `query` function, `entity_tables` have an `apply` function: it is used to apply a certain user-defined function to all the entities of a table that satisfy a certain condition. Unlike `query`, `apply` does not return any result; instead, the function is used to _modify_ the table's content conditionally.

Similarly to the case of queries, we first need to define a function that will be applied to the table:

```cpp
bool cs::queries::destroy_first_blue_square(entity_table& table, u32 e)
{
    if (table.models.get(e) == models::BLUE_SQUARE)
    {
        table.destroy(e);
        return true;
    }
    return false;
}
```

The function must return a `bool` value: if it returns `true`, the execution of the `apply` routine is interrupted, otheriwise it continues with the next entities. In the case above, we want to delete only the first entity of type `BLUE_SQUARE` appearing in the table, therefore we simply return `true` whenever we find an entity that satisfies this condition. This allows to avoid unnecessary iterations on the table.

Then, the function can be used like this:

```cpp
table.apply(&cs::queries::destroy_first_blue_square);
```

Similarly to `query`, also `apply` allows to pass an additional parameter to the applied function.

As a general note, it is better to avoid to abuse the `apply` function: most of the logic of the game should sit inside the entity updaters, and `apply` should be only used in very specific instances, for example to trigger a certain event at a specific moment in the game.
