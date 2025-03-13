/**
 * @file esa_arrays.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_ARRAYS_H
#define ESA_ARRAYS_H

#include "esa.h"

#include "bn_fixed.h"
#include "bn_sprite_ptr.h"

namespace esa
{

    /**
     * @brief An array of entity fields of a certain type.
     * 
     * @tparam T The type of field this array contains.
     * @tparam Entities The maximum number of entities this array can work with.
     * @tparam Models The maximum number of entity models this array can work with.
     * @tparam Size The maximum number of variables of type `T` this array can store.
     */
    template<typename T, u32 Entities, u32 Models, u32 Size>
    class field_array
    {

        entity_model _models [Size == 0 ? 1 : Models];
        T _fields [Size == 0 ? 1 : Entities * Size];

        public:

        /**
         * @brief Constructor.
         * 
         */
        field_array()
        {
            for (entity_model m = 0; m < (Size == 0 ? 1 : Models); m++)
                *(_models + m) = 0;
            for (u32 f = 0; f < (Size == 0 ? 1 : Entities * Size); f++)
                *(_fields + f) = 0;
        }

        /**
         * @brief Mark and entity model as possessing a specific field.
         * 
         * @tparam Model The entity model.
         * @tparam Field The field to assign.
         */
        template<entity_model Model, field_t Field>
        void add()
        {
            BN_ASSERT(Field < Size, "ESA ERROR: maximum number of fields exceeeded!");
            *(_models + Model) |= (1 << Field);
        }

        /**
         * @brief Assign a value to a field of a certain entity.
         * 
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<field_t Field>
        void set(entity e, T value)
        {
            *(_fields + e * Size + Field) = value;
        }

        /**
         * @brief Obtain the value of a field for a certain entity.
         * 
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @return T The value of the field for the entity.
         */
        template<field_t Field>
        T get(entity e)
        {
            return *(_fields + e * Size + Field);
        }

        /**
         * @brief Tells if an entity model owns a certain field.
         * 
         * @tparam Field The field to check for.
         * @param model The model to check.
         * @return true 
         * @return false 
         */
        template<field_t Field>
        bool has(entity_model model)
        {
            return ((_models[model] >> Field) & 1) == 1;
        }
        
        /**
         * @brief Clear the fields for an entity.
         * 
         * @param e The entitiy ID.
         */
        void clear(entity e)
        {
            for (u32 f = 0; f < Size; f++)
                *(_fields + e * Size + f) = 0;
        }

    };

    
    /**
     * @brief An array of `bool` fields, each stored inside a 32-bits `unsigned int`.
     * For each entity, a `bool_array` can store up to 32 `bool` fields.
     * 
     * @tparam Entities The maximum number of entities this array can work with. 
     * @tparam Models The maximum number of entity models this array can work with.
     */
    template<u32 Entities, u32 Models>
    class bool_array
    {
        entity_model _models [Models];
        u32 _fields [Entities];

        public:

        /**
         * @brief Constructor.
         * 
         */
        bool_array()
        {
            for (entity_model m = 0; m < Models; m++)
                *(_models + m) = 0;
            for (entity e = 0; e < Entities; e++)
                *(_fields + e) = 0;
        }

        /**
         * @brief Mark and entity model as possessing a specific `bool` field.
         * 
         * @tparam Model The model of the entity.
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<entity_model Model, field_t Field>
        void add()
        {
            BN_ASSERT(Field < 32, "ESA ERROR: maximum number of bool fields exceeded!");
            *(_models + Model) |= (1 << Field);
        }

        /**
         * @brief Assign a value to a `bool` field of a certain entity.
         * 
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<field_t Field>
        void set(entity e, bool value)
        {
            if (value)
                *(_fields + e) |= 1 << Field;
            else
                *(_fields + e) &= ~(1 << Field);
        }

        /**
         * @brief Obtain the value of a `bool` field for a certain entity.
         * 
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @return T The value of the field for the entity, as a `bool`.
         */
        template<field_t Field>
        bool get(entity e)
        {
            if ( ((*(_fields + e) >> Field) & 1) == 1 )
                return true;
            return false;
        }

        /**
         * @brief Tells if an entity model owns a certain `bool` field.
         * 
         * @tparam Field The field to check for.
         * @param model The model to check.
         * @return true 
         * @return false 
         */
        template<field_t Field>
        bool has(entity model)
        {
            return ((_models[model] >> Field) & 1) == 1;
        }

        /**
         * @brief Clear all the `bool` fields for an entity.
         * 
         * @param e The ID of the entity.
         */
        void clear(entity e)
        {
            *(_fields + e) = 0;
        }

    };


    /**
     * @brief An array of `uintn_t` fields (unsigned integers of n bits).
     * For each Entity, `uintn_t` fields are stored as a set of 1 or more bits inside a 32-bits `unsigned int`.
     * Up to 32 bits are available for each Entity for unit_t fields.
     * 
     * @tparam Entities The maximum number of entities this array can work with. 
     * @tparam Models The maximum number of entity models this array can work with.
     */
    template<u32 Entities, u32 Models>
    class uintn_array
    {
        entity_model _models [Models];
        u32 _fields [Entities];

        public:

        /**
         * @brief Constructor.
         * 
         */
        uintn_array()
        {
            for (entity_model m = 0; m < Models; m++)
                *(_models + m) = 0;
            for (entity e = 0; e < Entities; e++)
                *(_fields + e) = 0;
        }

        /**
         * @brief Mark and entity model as possessing a specific `uintn_t` field.
         * 
         * @tparam Model The model of the entity.
         * @tparam Field The field to assign.
         * @tparam Size The size (in bits) of the enum field.
         */
        template<entity_model Model, field_t Field, uintn_size Size>
        void add()
        {
            BN_ASSERT(Field + Size < 32, "ESA ERROR: maximum number of enum fields bits exceeded!");
            *(_models + Model) |= (1 << Field);
        }

        /**
         * @brief Assign a value to a `uintn_t` field of a certain entity.
         * 
         * @tparam Field The ID of the field.
         * @tparam Size The size (in bits) of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<field_t Field, uintn_size Size>
        void set(entity e, uintn_t value)
        {
            *(_fields + e) &= ~(((1 << Size) - 1) << Field);
            *(_fields + e) |= (value << Field);
        }

        /**
         * @brief Obtain the value of a `uintn_t` field for a certain entity.
         * 
         * @tparam Field The ID of the field to retrieve.
         * @tparam Size The size (in bits) of the field.
         * @param e The ID of the entity.
         * @return T The value of the field for this entity.
         */
        template<field_t Field, uintn_size Size>
        uintn_t get(entity e)
        {
            return ( *(_fields + e) >> Field) & ((1 << Size) - 1);
        }

        /**
         * @brief Tells if an entity model owns a certain `uintn_t` field.
         * 
         * @tparam Field The field to check for.
         * @param model The model to check.
         * @return true 
         * @return false 
         */
        template<field_t Field, uintn_size Size>
        bool has(entity_model model)
        {
            return ((_models[model] >> Field) & 1) == 1;
        }

        /**
         * @brief Clear all the `uintn_t` fields for an entity.
         * 
         * @param e The ID of the entity.
         */
        void clear(entity e)
        {
            *(_fields + e) = 0;
        }

    };


    /**
     * @brief An array of optional `bn::sprite_ptr` objects.
     * A single `bn::sprite_ptr` can be stored for each entity. Since the objects are optional,
     * a `sprite_array` is not limited to 128 elements - the important thing is that no more than
     * 128 sprites are present at the same time on screen.
     * 
     * @tparam Entities The maximum number of entities this array can work with. 
     */
    template<u32 Entities>
    class sprite_array
    {
        bn::optional<bn::sprite_ptr> _sprites [Entities];

        public:

        /**
         * @brief Assign a sprite to an entity.
         * 
         * @param e The ID of the entity.
         * @param sprite A `bn::sprite_ptr` object to assign to this entity.
         */
        void set(entity e, const bn::sprite_ptr& sprite)
        {
            *(_sprites + e) = sprite;
        }

        /**
         * @brief Retrieve a reference to the entity's sprite.
         * 
         * @param e The ID of the entity.
         * @return bn::sprite_ptr& a reference to the entity's sprite.
         */
        bn::sprite_ptr& get(entity e)
        {
            return (*(_sprites + e)).value();
        }
        
        /**
         * @brief Tells if the Entity currently has a sprite (i.e., if the `bn::optional` object has a value).
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        bool has(entity e)
        {
            return (*(_sprites + e)).has_value();
        }

        /**
         * @brief Remove the sprite for an entity (resets the `bn::optional` object).
         * 
         * @param e The ID of the entity.
         */
        void clear(entity e)
        {
            (*(_sprites + e)).reset();
        }
    };


    /**
     * @brief An array of entity models.
     * For each entity, its corresponding model is stored.
     * 
     * @tparam Entities The maximum number of entities this array can work with. 
     */
    template<u32 Entities>
    class model_array
    {
        entity_model _models [Entities];

        public:

        /**
         * @brief Constructor.
         * 
         */
        model_array()
        {
            for (entity e = 0; e < Entities; e++)
                *(_models + e) = 0xffffffff;
        }

        /**
         * @brief Assign a model to an entity.
         * 
         * @tparam Model The model to assign.
         * @param e The entity ID.
         */
        template<entity_model Model>
        void add(entity e)
        {
            *(_models + e) = Model;
        }

        /**
         * @brief Obtian the model of an entity.
         * 
         * @param e The ID of the entity.
         * @return u32 
         */
        u32 get(entity e)
        {
            return *(_models + e);
        }
        
        /**
         * @brief Clear the model for an entity.
         * 
         * @param e The entity ID.
         */
        void clear(entity e)
        {
            *(_models + e) = 0xffffffff;
        }

    };

}

#endif