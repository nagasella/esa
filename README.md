# ESA (Entity System Advance)

ESA (Entity System Advance) is an Entity-Component-System (ECS) framework written in C++. It is mostly targetting Game Boy Advance (GBA) game development, so it does not make any use of C++ features like RTTI or exceptions. Initially, ESA was conceived to be used together with the [butano](https://github.com/GValiente/butano?tab=readme-ov-file) game engine, but since version 2.0.0 it can be used independently from it (for example, with libtonc or libgba).

Below is a basic introduction to Entity Systems, as well as a comprehensive tutorial about ESA and its features. You can use it to learn and also as a reference to come back to from time to time. It covers all aaspects from basic usage to common optimization techniques. In this repo you'll also find a few examples that cover all the main feature of ESA. All examples are based on butano for now. For additional questions or help, you can reach out through the [GBADev discord](https://github.com/gbadev-org/awesome-gbadev?tab=readme-ov-file#community).

## Table of contents

- [An introduction to Entity Systems](#an-introduction-to-entity-systems)

- [An introduction to ESA](#an-introduction-to-esa)

- [Using ESA in your project](#using-esa-in-your-project)

- [Tutorial - Part 1: basic building blocks](#tutorial---part-1-basic-building-blocks)

    - [A simple example](#a-simple-example)

    - [Entity tables](#entity-tables)

    - [Components](#components)

    - [Entity lifecycle](#entity-lifecycle)

    - [Updaters](#updaters)

    - [Queries](#queries)

    - [Apply](#apply)

- [Tutorial - Part 2: advanced aspects](#tutorial---part-2-advanced-aspects)

    - [Storing components in IWRAM](#storing-components-in-iwram)

    - [Accessing components with the `[]` operator](#accessing-components-using-the--operator)

    - [Indexed components and index updaters](#indexed-components-and-index-updaters)

- [Appendix A: boosting performance with ARM code](#appendix-a-boosting-performance-with-arm-code)

- [Appendix B: ESA helper components](#appendix-b-esa-helper-components)

## An introduction to Entity Systems

Entity Systems have been a hot topic in game development for several years, although there are different visions about how to implement one. ESA is inspired by the model discussed in [this series of articles from 2007](https://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (always a great read for anyone interested in the topic). It implements some of these ideas, although with a focus on the GBA hardware.

The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, implementing something more similar to a database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components for the entities, and Systems are routines that process all the entities that share a common set of Components. The IDs of groups of entities that share common aspects are obtained through queries on the database.

## An introduction to ESA

ESA is a compact, header-only framework. It aims to implement the vision described in the articles linked above, allowing you to organize your game in a way that resembles a _relational database_ (although here the _relational_ aspect is arguably not that important). It provides a [clear framework](#tutorial---part-1-basic-building-blocks) to organize the code of your games, it makes it easy to [distribute the data of your game between IWRAM and EWRAM](#storing-components-in-iwram), it allows to easily [optimize memory consumption](#indexed-components-and-index-updaters) where necessary and [makes it simple to identify performance-critical parts of your code to compile as ARM instructions](#appendix-a-boosting-performance-with-arm-code). 

Its main building blocks are the following:
* The [_Entity table_](#entity-tables) is the main data structure of ESA: it contains all the data about game objects, organized in a tabular format
* [_Entities_](#entity-lifecycle) are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* [_Components_](#components) are the _column indices_ of the table, and they implement the data for each entity (each entity can own (or not) each component)
* _Updaters_ are objects that work on tables and update the components of entities:
    * [_Entity Updaters_](#entitiy-updaters), which work on subsets of entities in a table based on their components
    * [_Table Updaters_](#table-updaters), which do not necessarily process specific entities, but they can access a table's content freely (for example through queries)
    * [Index updaters](#indexed-components-and-index-updaters) were introduced since ESA 2.2.0 - they are a more abstract kind of updater which work on indexed components (see advanced section of the tutorial)
* [_Queries_](#queries) can be performed on an entity table, in order to filter it - that is, look for specific entities based on a condition
* Also, entity tables support [_Apply_](#apply) operations, which consist in applying a function iteratively on the whole table (this can allow to modify all entities that satisfy a certain condition, at a specific moment in time)

## Using ESA in your project

If you are planning to use ESA with the butano game engine, you can find excellent information about how to set up DevkitARM and butano [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano porject Makefile to include ESA's `include` path. 

You can also use ESA with other libraries (like libtonc, libgba, ...), and for general information about the available options and how to set up your development environment check out the [getting started](https://gbadev.net/getting-started.html) page of the awesome GBAdev community website. Again, you will need to add ESA's `include` path to your project.

ESA comes with a few example projects, all of which are based on butano for the moment. Examples are always a good way to learn since you can try to tweak things and see what happens. If you want to run these projects, make sure that the relative path of butano defined in their Makefile matches its location on your computer.

## Tutorial - Part 1: basic building blocks

The following is an extensive tutorial about ESA's features. This first part covers the basics about using ESA: entity tables, components, updaters, queries and apply obejcts.

### A simple example

Let's start with a simple example about using ESA. Below you can see how to define some components, allocate an entity table and create some entities.

```cpp
#include "esa.h"

// define tags for the components
#define POSITION 0
#define VELOCITY 1

// define the components
struct position 
{
    int x, y;
};

struct velocity 
{
    int x, y;
};

int main()
{
    // define an entity table with 100 entities and 2 components
    esa::entity_table<100, 2, 0, 0, 0> table;

    // add components to the table
    table.add_component<position>(POSITION);
    table.add_component<velocity>(VELOCITY);

    // create 100 entities and give them components
    for (int i = 0; i < 100; i++)
    {
        esa::entity e = table.create(); // esa::entity is an alias for unsigned short
        table.add<position, POSITION>(e, {0, 0});
        table.add<velocity, VELOCITY>(e, {1, 1});
        table.subscribe(e);
    }

    // udpate entities
    for (esa::entity e = 0; e < 100; e++)
    {
        position & p = table.get<position, POSITION>(e);
        velocity & v = table.get<velocity, VELOCITY>(e);

        p.x += v.x;
        p.y += v.y;
    }
}
```

Each of the commented parts will be examplained in more detail in the next sections. Note that the update iteration shown above (using a for loop) is not the standard way of working with ESA. Instead, ESA uses [updaters](#updaters), [queries](#queries) and [apply objects](#apply), which were not used above just to keep the example compact. The next sections will cover all of them.

### Entity tables

In order to optimize memory consumption, ESA allows to parameterize entity tables with these template parmaeters:

* `Entities`: the maximum number of entities available (the number of rows in the table)

* `Components`: the maximum number of [components](#components) available (the number of columns in the table)

* `Updaters`: the maximum number of [updaters](#updaters)

* `Queries`: the maximum number of [cached queries](#2-cached-queries)

* `Applys`: the maximum number of [cached apply objects](#2-cached-apply-objects)

In the [example](#a-simple-example) above, the entity table was parameterized like this:

```cpp
esa::entity_table<100, 2, 0, 0, 0> table;
```

This means that the `entity_table` can contain at most `100` entities and `2` components. No updaters, cached queries or cached apply objects are allowed here (although this is not a typical scenario).

## Components

Components contain data for entities. In essence, each column in a table is an array of components: at the crossing of each column (component ID) and row (entity ID) there is a component for an entity. We can define the available components for a table as in the example above:

```cpp
table.add_component<position>(POSITION);
table.add_component<velocity>(VELOCITY);
```

The template parameter is the type of the component, while the argument of the function is the [unique tag](#tags-and-updaters) to assign to the component (which is also used to retrieve the component for each entity).  Components can be of any data type, both simple (`int`, `bool`, ...) or complex (`struct`, ... but they should always provide one constructor that does not take any arguments). More than one component with the same data type is allowed; however, each component needs to be identified by a unique integer index, called [tag](#tags-and-updaters). Such index should be between `0` and the maximum number of components allowed for the table (as explained in the [previous section](#entity-tables)). ESA also offers [a couple of custom components](#appendix-b-esa-helper-components) developed for memory efficiency. 

## Entity lifecycle

An entity can be added to a table as shown in the example above: 

```cpp
esa::entity e = table.create();
```

Then, we can add some components to it:

```cpp
table.add<position, POSITION>(e, {0, 0});
table.add<velocity, VELOCITY>(e, {1, 1});
```

In the example above, we also have this instruction:

```cpp
table.subscribe(e);
```

This _subscribes_ the entity to every relevant [entity updater](#entitiy-updaters), [cached query](#2-cached-queries) and [cached apply obejct](#2-cached-apply-objects).

A reference to a component can be retrieved from an entity like this:

```cpp
position & p = table.get<position, POSITION>(e);
```

Finally, entities can be deleted from the table using:

```cpp
table.destroy(e);
```

If a component of the entity hold resources, like for example sprites or backgrounds, make sure to deallocate them in the component's destructor, and they will be freed when the entity is destroyed. To delete all entities in a table, simply use:

```cpp
table.clear();
```

## Updaters

Updaters are the core of ESA: they are objects that work on entity tables and process their components. They implement the logic of the game, each focusing on a very precise and limited aspect. There are two main types of updaters: [table updaters](#table-updaters) and [entity updaters](#entitiy-updaters), from which you can inherit to define the updaters of your game. A third type, called [index updater](#indexed-components-and-index-updaters) will be introduced in the advanced section of the tutorial. 

Each of them has at least an `init` function (for generic initialization) and an `update` function. They also must have a [unique tag](#tags-and-updaters) associated. The idea is that we want to avoid using [for loops](#a-simple-example) in our main program to work on entities; instead, we want to implement our logic inside updaters, then attach those updaters to our entity table and let ESA deal with the rest:

```cpp
entity_table<100, 2, 5, 0, 0> table;
table.add_components<position>(POSITION);
table.add_components<velocity>(VELOCITY);

table.add_udpater(new udpater1(table));
table.add_udpater(new udpater2(table));

table.init(); // call "init" for every updater

// create some entities... (shown previously)

while (1)
{
    table.update(); // call "update" for every updater
}
```

This keeps your main game loop clean and tidy. Btw, it is important to ALWAYS allocate updaters with `new`! (Since ESA will `delete` them when the table is destroyed)

Updaters can be deactivated and activated: when they are deactivated, their `update` function will not be called during `table.update()`. This is useful if you want to put on hold some functionality of your game (typical example: pause the game), and it can be done both for table updaters and entity updaters (discussed in the [next section](#entitiy-updaters)). You can define the initial state for an updater as you add it to the table:

```cpp
table.add_updater(new updater(table), false); // updater will start inactive
```

If you do not specify the boolean parameter, the default state of the udpater will be active. You can also deactivate the updater at any time during the game loop:

```cpp
table.deactivate_updater<UPDATER_TAG>();
```

And then reactivate it, using:

```cpp
table.activate_updater<UPDATER_TAG>();
```

You can also make all the updaters active at once:

```cpp
table.activate_all_updaters();
```

Or deactivate al of them at once:

```cpp
table.deactivate_all_updaters();
```

### Table updaters

The simplest type of updater is the `esa::table_updater`: it does not work on any specific entity, but it can work freely on the whole table. To define a new a table updater, you need to create a class that inherits from `esa::table_updater`, and override the `init` and `update` member functions. Normally, you will also need to get a reference to the entity table for practical purposes, and pass a [unique tag to use for the updater](#tags-and-updaters) to the constructor of the base class. Here is a small example:

```cpp
#define TAG 0 // updater's tag

using entity_table = esa::entity_table<100, 2, 1, 0, 0>;

class updater : public esa::table_updater
{
    entity_table & table;

    public:

    // assign tag and get a reference to the table
    updater(entity_table & t) : 
        table_updater(TAG),
        table(t)
    { }

    void init() override
    { 
        // some initialization code... if needed
    }

    void update() override
    {
        // some update code...
    }
};
```

You can do whatever you want in the `init` and `update` functions. For example, you can get the IDs of various entities through [queries](#queries), and then work on those entities (collision detection, ...), or anything else really.

### Entitiy updaters

Entity updaters work only on entities that own (_or not_) specific components. These entities are said to be _subscribed_ to the entity updater.  Entity updaters have to inherit from `esa::entity_updater<Entities>`. The `Entities` parameter indicates how many entities _at most_ this updater will work on: it does _not_ need to match the `Entities` parameter of the [entity table](#entity-tables); actually, if you use a smaller number, this will be saving some memory. 

Then you will have to override the `select` member function: this function implements a _filter_ which decides which entities will be processed by the updater. An arbitrarily complex filter can be used, however remember that this function is only ran ONCE per entity (when the entity is added to the table).

Let's make a practical example and implement an update logic for the `position` and `velocity` components, in order to finally replace the [for loop](#a-simple-example) in the `main` from the first example:

```cpp
#define MOVEMENT 0 // updater's tag

using entity_table = esa::entity_table<100, 2, 1, 0, 0>;

class updater : public esa::entity_updater<100> // can also be < 100...
{
    entity_table & table;

    public:

    updater(entity_table & t) : 
        entity_updater(MOVEMENT),
        table(t)
    { }

    // filter the entities to process based on the components they own
    bool select(esa::entity e) override
    {
        return table.has<POSITION>(e) && table.has<VELOCITY>(e);
    }

    void init() override { }

    void update() override
    {
        // loop on all entities currently subscribed to this updater
        for (esa::entity e : this->subscribed())
        {
            position & p = table.get<position, POSITION>(e);
            velocity & v = table.get<velocity, VELOCITY>(e);

            p.x += v.x;
            p.y += v.y;
        }
    }
};
```

You are allowed to delete entities (using `table.destroy(e)`) as you loop through them, since ESA does not delete entities immediately but records the IDs of the deleted entities and then deletes all of them at the end of a `table.update()` call (when all updaters have been prcessed for that frame).

An entity updater allows you to _unsubscribe_ entities: when an entity is unsubscribed the updater will not process it anymore. You can unsubscribe an entity from an updater with:

```cpp
table.unsubscribe_from_updater<UPDATER_TAG>(e);
```

Then, the entity can be subscribed again with:

```cpp
table.subscribe_to_updater<UPDATER_TAG>(e);
```

### Tags and updaters

Tags are unique integer identifiers used for retrieving components, updaters, cached queries and cached apply objects. As a clarification, tags should be unique for each of those four ESA objects inividually, not _globally_: there will be a set of unique tags for components, a different set for updaters, another set for queries and another again for apply objects. Each of these sets will be numbered `0` to [the maximum defined for the table](#entity-tables). Updaters can be retrieved from anywhere else in the program through their tag:

```cpp
my_updater * updater = (my_updater *) table.get_updater<UPDATER_TAG>();
```

Similarly also for cached queries and cached apply objects (exaplained in the next section).

## Queries

Updaters are nice, but using only those to work on a table can be limiting. For example, sometimes what we need is an easy way to obtain (at a specific moment in the program) the indexes of all the entities that satisfy a certain condition and then do something with those IDs (check collisions, ...). We use queries for this. Queries can be implemented in two ways:
1. As a user-defined function
2. As a _cached query_ object

### 1. Queries based on a user-defined function

The first type of query is one based on a user-defined function. This needs to be a `bool` function, which will be applied per-entity: it should take as arguments the `entity_table` and a single `entity`, and return `true` if the entity satisfies the query criteria, otherwise `false`. Let's say we want to get all the entities on the right side of the screen, then the query function will look like this:

```cpp
bool find_entities_on_right_side(entity_table & table, entity e)
{
    if (!table.has<POSITION>(e))
        return false;
        
    position & p = table.get<position, POSITION>(e);
    if (p.x > 0)
        return true;

    return false;
}
```

Then, this function can be used to query a table, that is, to obtain the IDs of all the entity that satisfy the condition:

```cpp
esa::vector<entity, 100> ids = table.query<100>(&find_entities_on_right_side);
```

The query returns an `esa::vector` of entity IDs. `esa::vector` is similar to `std::vector`, with some features missing and maximum capacity defined at compile time. The maximum capacity you define for the vector of the vector should be enough to fit all the entities that are _expected_ to be found by the query: since this is not necessarily known at compile time, you should be careful about this.

This type of query is very practical as all it requires is to define a function; however it can be inefficient, since the function is applied to all the entities in the table indiscriminately. In order to get the best performance you will usually want to use [cached queries](#2-cached-queries) instead.

Queries based on functions can be dynamic if the function takes also a third parameter (reference, of any type), which is used for filtering. In this case the filtering condition can change in time based on the parameter. You can check out the example `colored-squares` to see a practical example of this type of query.

### 2. Cached queries

Cached queries are queries that are defined as classes, and offer better performance because they filter the entities they will work on as they are added to the table. Their definition is similar to the definition of [entity updaters](#entitiy-updaters), but they need to inherit from `esa::cached_query<Entities>`. The parameter `Entities` represents the _expected maximum_ number of entities the query will retrieve: it does _not_ have to match the `Entities` parameter of the entity table. Actually, if it is smaller you will save some memory.

Compared to entity udpaters, we do not override an `update` function, but a `where` function (the naming is taken from classical SQL databases): this is a `bool` function that will filter based on each entity's components data, and must return `true` to signal that the query condition is satisfied, otherwise `false`. A unique tag must be defined for each cached query, pretty much like for updaters (it is passed to the constructor of the base class).

Let's implement the same query that we implemented before as a cached query:

```cpp
#define QRY_FIND_ON_RIGHT_SIDE 0 // query's tag

using entity_table = esa::entity_table<100, 2, 1, 1, 0>;

class query : public  esa::cached_query<50> // we expect to find 50 entities max each time
{
    entity_table & table;

    public:

    query(entity_table & t) : 
        cached_query(QRY_FIND_ON_RIGHT_SIDE),
        table(t)
    { }

    // CACHED part of the query: executed ONLY when an entity is added to the table
    bool select(esa::entity e) override
    {
        return table.has<POSITION>(e);
    }

    void init() override { }

    // DYNAMIC part of the query: executed each time the query is called
    bool where(esa::entity e) override
    {
        position & p = table.get<position, POSITION>(e);

        if (p.x > 0)
            return true;

        return false;
    }
};
```

The query above roughly corresponds to an SQL query like:

```sql
select POSITION_X from MY_TABLE where POSITION_X > 0
```

An instance of the query needs to be attached to the table, as for updaters:

```cpp
table.add_query(new query(table));
```

Then, it can (finally) be called inside the game like this:

```cpp
esa::vector<entity, 50> ids = table.query<QRY_FIND_ON_RIGHT_SIDE, 50>(); 
```

As an alternative, you can define a `esa::vector` first and pass it to the function as a reference, then run the query without a `return`:

```cpp
esa::vector<entity, 50> ids;
table.query<QRY_FIND_ON_RIGHT_SIDE, 50>(ids); 
```

This saves the time to copy back the vector at the end of the execution of the query. Entities can be `unsubscribed` from queries (in this case the query will not be able to find them):

```cpp
table.unsubscribe_from_query<QUERY_TAG>(e);
```

Then you can re-subscribe the entity by:

```cpp
table.subscribe_to_query<QUERY_TAG>(e);
```

If you want a pointer to the query later on, you can get by using:

```cpp
query * q = (query *) table.get_query<QRY_FIND_ON_RIGHT_SIDE>();
```

## Apply

Additionally to running queries, it is possible to `apply` a certain function to an entire table. When we use `apply`, we don't want to get a vector of IDs: we just want to modify a bunch of entities at that specific moment of the program (usually as a response to some other event).

Just like for queries, there are two different types of `apply` implementations:
1. Apply a user-defined function to a table
2. Use a cached apply object

### 1. Apply based on a function

Let's define a function that teleports an entity from `x = -6` to `x = 6` for no reason:

```cpp
bool teleport_entity(entity_table& table, entity e)
{
    if (!table.has<POSITION>(e))
        return false;

    position & p = table.get<position, POSITION>(e);
    if (p.x == -6)
        p.x = 6;

    return false;
}
```

The function must return a `bool` value: if it returns `true`, the execution of the `apply` routine is IMMEDIATELY INTERRUPTED, otheriwise it continues with the next entity in the table. This approach is used to give the option to avoid unnecessary iterations, but in the example above we want to check every entity so we always return `false`. Then, the function can be ran like this:

```cpp
table.apply(&teleport_entity);
```

You can also pass parameters to an applied functions for dynamic behavior, just like for queries.

### 2. Cached apply objects

Cached apply objects allow for better performance compared to apply operations based on functions, as they filter the entities they will work on as they are added to the table. To implement a cached apply obejct you have to inherit from `esa::cached_apply<Entities>`. The parameter `Entities` represents the _expected maximum_ number of entities the apply object will have to work on: it does _not_ have to match the number of entities in the entity table, since an apply can in principle work on less entities (a smaller number will save some memory). 

Let's implement the same teleport function from above in a cached apply object (you should have gotten the idea of how this works by now):

```cpp
#define APPLY_TELEPORT 0

using entity_table = esa::entity_table<100, 2, 1, 1, 1>;

class apply : public esa::cache_apply<50> // we expect to work on 50 entities max
{
    entity_table & table;

    public:

    updater(entity_table & t) : 
        cached_apply(APPLY_TELEPORT),
        table(t)
    { }

    // CACHED part of the apply: executed ONLY when an entity is added to the table
    bool select(eas::entity e) override
    {
        return table.has<POSITION>(e);
    }

    void init() override { }

    // DYNAMIC part of the apply: executed each time the apply is called
    bool apply(esa::entity e) override
    {
        position & p = table.get<position, POSITION>(e);
        if (p.x == -6)
            p.x = 6;
        return false;
    }
}
```

Then add an instance of it to the table:

```cpp
table.add_apply(new apply(table));
```

And finally call it whenever you want to have all the entities with `x = -6` magically teleported to `x = 6`:

```cpp
if (condition)
    table.apply<APPLY_TELEPORT, 50>(); // call the apply using its tag
```

Entities can be `unsubscribed` from apply objects:

```cpp
table.unsubscribe_from_apply<APPLY_TAG>(e);
```

In this case the apply will not process the entity anymore. Then you can re-subscribe the entity by:

```cpp
table.subscribe_to_apply<APPLY_TAG>(e);
```

## Tutorial - Part 2: advanced aspects

This part of the tutorial exapnds on some of the concepts introduced previously, mostly dealing with memory efficiency and performance aspects. It requires to understand a bit better how ESA works under the hood, so this is why these topics were kept in a separate section.

### Storing components in IWRAM

In the [example above](#a-simple-example), we added some columns to the table like this:

```cpp
table.add_component<position>(POSITION);
table.add_component<velocity>(VELOCITY);
```

What is actually happening here is that an array of components of each data type is being allocated on the heap - which, when compiling games with the GBA toolchain, means EWRAM. Although there's plenty of room in EWRAM (and therefore it is GOOD to allocate your components there), the access to this type of memory can often be slower compared to IWRAM access. For this reason, ESA allows to store components in IWRAM as well: in order to do this, you will need to define the array of components (in practice, an `esa::series` container) on the stack, and then pass a pointer to it to the entity table. Like this:

```cpp
esa::series<position, 100> positions; // stack (IWRAM) allocated
esa::series<velocity, 100> velocities; // stack (IWRAM) allocated
table.add_series(&positions, POSITION);
table.add_series(&velocities, VELOCITIES);
```

The `esa::series` template parameters are (1) the data type of the component and (2) the maximum number of entities for the associated `entity_table`. When you define your entity table, you can use a mixed approach and decide which components to put in EWRAM (this should be your DEFAULT choice) and which to put in IWRAM (should be only for components used in performance-critical tasks, since IWRAM is small).


### Accessing components using the `[]` operator

Up to now, in each `update` function components were accessed using:

```cpp
position & p = table.get<position, POSITION>(e);
```

This is nice and elegant, however it can be a bit inefficient. There can be some performance improvement by using _direct component indexing_ instead: we will need to get a reference to each component array (which is an `esa::series` object, as explained in the previous section) and access the components with the `[]` operator. Easier in code than by words:

```cpp
#define MOVEMENT 0

using entity_table = esa::entity_table<100, 2, 1, 0, 0>;

class updater : public esa::entity_udpater<100>
{
    // reference to the indivdual component arrays
    esa::series<position, 100> & positions;
    esa::series<velocity, 100> & velocities;

    public:

    updater(entity_table & t) 
        : entity_updater(MOVEMENT),
        positions(t.get_series<position, POSITION>()),
        velocities(t.get_series<velocity, VELOCITIES>())
    { }

    bool select(esa::entity e) override
    {
        // check if the entity has the position and velocity components
        // we can use the series objects directly instead of the table
        return positions.has(e) && velocities.has(e);
    }

    void init() override { }

    void update() override
    {
        for (esa::entity e : this->entities())
        {
            // access the components in the series like it was an array!
            position & p = positions[e];
            velocity & v = velocities[e];

            p.x += v.x;
            p.y += v.y;
        }
    }
};
```

The code is a bit more convoluted especially in the way the constructor is set up, but from my profiling experiments it is generally faster than the standard way (using `table.get<type, TAG>(e)`). This is probably due to the extra time required for the function call compared to indexing the series directly.

### Indexed components and index updaters

When you add a component to a table like explained previously, an array of compoennts is created. The size of this array of components is the same as the `Entities` parameter of the entity table: if your table can hold `50` entities, each array of components will have `50` elements; if your table can hold `500` entities, they will have `500` elements. The index of each component in a component array is the ID of the entity that owns it.

However, some entities in the table _may not_ own the component: for example, if only `5` entities need the component out of `100`, then you will end up with a lot of _wasted memory_. To help with this, from ESA 2.2.0 indexed components were introduced: when you use an indexed component, it is like having a column in the table that is _shorter_ than the other columns (and therefore uses less memory). The container that holds indexed components is the `esa::indexed_series` object, which behaves more like a vector rather than an array. The index of each component in the series is essentially unrelated to the entity ID: however, the series stores some information about the correspondence of each entity ID to each component index. 

For example, let's say that we have an `animation` component like this:

```cpp
#define ANIMATION 2

struct animation
{
    int first, last, current, timer, frame_duration;
};
```

We can add it to a table as an indexed component like this:

```cpp
table.add_component<animation, 5>(ANIMATION);
```

This is not so much different compared to adding a regular component, we only have a second template parameter that specifies the size of the underlying indexed series: essentially, `5` is the maximum number of entities _we expect_ will use this component; if we try to assign the `animation` component to a 6th entity, ESA will jsut throw an error.

You can store indexed components in IWRAM similarly to [regular components](#storing-components-in-iwram), by creating an indexed series on the stack:

```cpp
esa::indexed_series<animation, 5> animations;
table.add_series(&animations, ANIMATION);
```

Then, indexed components can be processed using a third type of updater called index updater. In an index updaters, the `update` function works on an `esa::index` instead of an `esa::entity`, which is the index of the indexed series. Remember, this is unrelated to the entity ID, although there is a way to convert between the two. Below is an example of an index updater for the `animation` component defined previously. Let's also suppose that we have a `sprite` component, that was added to the table as a _regular_ component (not indexed), with its own `SPRITE` tag (they implementation details of this sprite component do not really matter here):

```cpp
#define UPD_ANIMATION 0

using entity_table = esa::entity_table<100, 4, 1, 0, 0>;

class updater : public esa::index_updater<animation, 5>
{
    entity_table & table;

    public:

    updater(entity_table & t) : 
        index_updater(t.get_series<animation, 5, ANIMATION>(), UPD_ANIMATION),
        table(t)
    { }

    bool select(esa::entity e) override
    {
        // "series" is a protected member variable of each index updater
        // it is a reference to the indexed series the updater works on
        return this->series.has(e);
    }

    void init() override { }

    void update() override
    {
        // loop on the indexes of the indexed series
        for (esa::index i : this->subscribed())
        {
            // access the aniamtion component at this index
            animation & anim = this->series[i];

            // update the animation
            if (anim.timer > 0)
                anim.timer--;
            else
            {
                if (anim.curr < anim.last)
                    anim.curr++;
                else
                    anim.curr = anim.first;
                
                anim.timer = anim.frame_duration;

                // find the entity ID corresponding to the index
                esa::entity e = series.id(i);

                // use the entity ID to retrieve other components
                sprite & spr = table.get<sprite, SPRITE>(e);
                // ...
                // udpate sprite aniamtion frame...            
                // ...
            }
        }
    }
};
```

This approach allows at the same time to iterate only on those few entities that possess the indexed component and to have very fast access to other components of those entities from the entity table. 

The main weakeness of index updaters is related to when you add or remove entities to/from the table: due to the extra time needed to map an entity ID to the corresponding component index (which is slower than the opposite) they can be a bit slow. In general (and specifically if you have a lot of entities that need to be constantly created and destroyed) my suggestion would be to try to privilege regular components and entity updaters instead, and use indexed components and updaters only when the advantages in terms of memory impact are critical.

## Appendix A: boosting performance with ARM code

In GBA development, if you feel like you need some performance boost it is often a good idea to compile some of your code in ARM instructions and store it in IWRAM (by default, code is compiled as Thumb and stored in ROM). The butano engine allows to generate ARM code in IWRAM by using the macro `BN_CODE_IWRAM` (check [this](https://gvaliente.github.io/butano/faq.html#faq_memory_arm_iwram) out in the butano FAQ), but you can do the same with other libraries too like libtonc. We can apply this principle to updaters, queries and apply objects.

In ESA, updaters generally implement small `update` functions that take care of very specialized tasks. This makes it easy to identify performance-critical parts of your program to compile as ARM code in IWRAM. For example, we can modify the example discussed in the section about entity updaters as follows (we will use the example with [direct component indexing](#accessing-components-using-the--operator). First, we have to define the `updater.h` header file like this:

```cpp
#define MOVEMENT 0

using entity_table = esa::entity_table<100, 2, 1, 0, 0>;

class udpater : public esa::entity_updater<100>;
{
    esa::series<position, 100> & positions;
    esa::series<velocity, 100> & velocities;

    public:

    udpater(entity_table & t);
    bool select(esa::entity e) override;
    void init() override;
    BN_CODE_IWRAM void update() override; // marked as IWRAM (ARM) code
};
```

Then, we will implement in `udpater.cpp` all functions apart from the `update` function:

```cpp
#include "udpater.h"

updater::updater(entity_table & t)
    : entity_updater(MOVEMENT),
    positions(t.get_series<position, POSITION>()),
    velocities(t.get_series<velocity, VELOCITIES>()),
{

}

bool udpater::select(esa::entity e)
{
    return positions.has(e) && velocities.has(e);
}

void udpater::init()
{

}
```

And finally we can implement the `update` function in a file named `udpater.iwram.cpp`:

```cpp
#include "udpater.h"

void updater::update()
{
    for (esa::entity e : this->subscribed())
    {
        position & p = positions[e];
        velocity & v = velocities[e];

        p.x += v.x;
        p.y += v.y;
    }
}
```

This will give some important performance boost compared to the all-Thumb implementation, especially [if you also placed the components in IWRAM](#storing-components-in-iwram). The same principle can be applied to the `where` and `apply` functions of queries and apply objects.


## Appendix B: ESA helper components

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
