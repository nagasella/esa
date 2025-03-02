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
     * @brief An array of fields of a certain type.
     * 
     * @tparam T the type of Field this `field_array` contains.
     * @tparam Entities The maximum number of Entities this `field_array` can work with.
     * @tparam Models The maximum number of Entity Models this `field_array` can work with.
     * @tparam Size The maximum number of variables of type `T` this `field_array` can store.
     */
    template<typename T, u32 Entities, u32 Models, u32 Size>
    class field_array
    {
        u32 _models [ Size == 0 ? 1 : Models ];
        T   _fields [ Size == 0 ? 1 : Entities * Size ];

        public:

        /**
         * @brief Assign a value to a field of a certain entity, while at the
         * same time building a corresponding entity model.
         * 
         * @tparam Model The model of the entity.
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<u32 Model, u32 Field>
        void add(u32 e, T value)
        {
            BN_ASSERT(Field < Size, "ESA ERROR: maximum number of fields exceeeded!");
            *(_models + Model) |= (1 << Field);
            *(_fields + Size * e + Field) = value;
        }

        /**
         * @brief Assign a value to a field of a certain entity. This function will not 
         * modify the entity model.
         * 
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<u32 Field>
        void set(u32 e, T value)
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
        template<u32 Field>
        T get(u32 e)
        {
            return *(_fields + e * Size + Field);
        }
        
        void clear(u32 e)
        {
            for (u32 f = 0; f < Size; f++)
                *(_fields + e * Size + f) = 0;
        }

        bool matching(u32 model, u32 mask)
        {
            return (_models[model] & mask) == mask;
        }

    };

    
    /**
     * @brief An array of `bool` fields, each stored inside a 32-bits `unsigned int`.
     * For each entity, a `bool_array` can store up to 32 `bool` fields.
     * 
     * @tparam Entities The maximum number of entities this `field_array` can work with. 
     * @tparam Models The maximum number of entity models this `field_array` can work with.
     */
    template<u32 Entities, u32 Models>
    class bool_array
    {
        u32 _models [Models];
        u32 _fields [Entities];

        public:

        /**
         * @brief Assign a value to a field of a certain entity, while at the
         * same time building a corresponding entity model.
         * 
         * @tparam Model The model of the entity.
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<u32 Model, u32 Field>
        void add(u32 e, bool value)
        {
            BN_ASSERT(Field < 32, "ESA ERROR: maximum number of bool fields exceeded!");
            *(_models + Model) |= (1 << Field);
            if (value)
                *(_fields + e) |= 1 << Field;
            else
                *(_fields + e) &= ~(1 << Field);
        }

        /**
         * @brief Assign a value to a field of a certain entity. This function will not 
         * modify the entity model.
         * 
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<u32 Field>
        void set(u32 e, bool value)
        {
            if (value)
                *(_fields + e) |= 1 << Field;
            else
                *(_fields + e) &= ~(1 << Field);
        }

        /**
         * @brief Obtain the value of a field for a certain entity.
         * 
         * @tparam Field The ID of the field.
         * @param e The ID of the entity.
         * @return T The value of the field for the entity, as a `bool`.
         */
        template<u32 Field>
        bool get(u32 e)
        {
            if ( ((*(_fields + e) >> Field) & 1) == 1 )
                return true;
            return false;
        }

        void clear(u32 e)
        {
            *(_fields + e) = 0;
        }

        bool matching(u32 model, u32 mask)
        {
            return (_models[model] & mask) == mask;
        }

    };


    /**
     * @brief An array of `enum` fields.
     * For each Entity, `enum` fields are stored as a set of 1 or more bits inside a 32-bits `unsigned int`.
     * Up to 32 bits are available for each Entity for enum fields.
     * 
     * @tparam Entities The maximum number of entities this `field_array` can work with. 
     * @tparam Models The maximum number of entity models this `field_array` can work with.
     */
    template<u32 Entities, u32 Models>
    class enum_array
    {
        u32 _models [Models];
        u32 _fields [Entities];

        public:

        /**
         * @brief Assign a value to a field of a certain entity, while at the
         * same time building a corresponding entity model.
         * 
         * @tparam Model The model of the entity.
         * @tparam Field The field to assign.
         * @tparam Size The size (in bits) of the enum field.
         * @param e The entity to assign the fields's value to.
         * @param value The value to assign to the field.
         */
        template<u32 Model, u32 Field, u32 Size>
        void add(u32 e, u32 value)
        {
            BN_ASSERT(Field + Size < 32, "ESA ERROR: maximum number of enum fields bits exceeded!");
            *(_models + Model) |= (((1 << Size) - 1) << Field);
            *(_fields + e) &= ~(((1 << Size) - 1) << Field);
            *(_fields + e) |= (value << Field);
        }

        /**
         * @brief Assign a value to a Field of a certain Entity.
         * 
         * @tparam Field The ID of the field.
         * @tparam Size The size (in bits) of the field.
         * @param e The ID of the entity.
         * @param value The value to assign to the field.
         */
        template<u32 Field, u32 Size>
        void set(u32 e, u32 value)
        {
            *(_fields + e) &= ~(((1 << Size) - 1) << Field);
            *(_fields + e) |= (value << Field);
        }

        /**
         * @brief Obtain the value of a field for a certain entity.
         * 
         * @tparam Field The ID of the field to retrieve.
         * @tparam Size The size (in bits) of the field.
         * @param e The ID of the entity.
         * @return T The value of the field for this entity, as a `u32` (`unsigned int`).
         */
        template<u32 Field, u32 Size>
        u32 get(u32 e)
        {
            return ( *(_fields + e) >> Field) & ((1 << Size) - 1);
        }

        void clear(u32 e)
        {
            *(_fields + e) = 0;
        }

        bool matching(u32 model, u32 mask)
        {
            return (_models[model] & mask) == mask;
        }

    };


    /**
     * @brief An array of optional `bn::sprite_ptr` objects.
     * A `bn::sprite_ptr` can be stored for each entity. Since the objects are optional
     * a `sprite_array` is not limited to 128 sprites - the important thing is that no more than
     * 128 sprites are present at the same time in the array.
     * 
     * @tparam Entities The maximum number of entities this `sprite_array` can work with. 
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
        void set(u32 e, const bn::sprite_ptr& sprite)
        {
            *(_sprites + e) = sprite;
        }

        /**
         * @brief Retrieve a reference to the entity's sprite.
         * 
         * @param e The ID of the entity.
         * @return bn::sprite_ptr& a reference to the entity's sprite.
         */
        bn::sprite_ptr& get(u32 e)
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
        bool has(u32 e)
        {
            return (*(_sprites + e)).has_value();
        }

        /**
         * @brief Remove the sprite for an entity (resets the `bn::optional` object).
         * 
         * @param e The ID of the entity.
         */
        void clear(u32 e)
        {
            (*(_sprites + e)).reset();
        }
    };


    /**
     * @brief An array of entity models.
     * For each entity, its corresponding model is stored.
     * 
     * @tparam Entities The maximum number of entities this `sprite_array` can work with. 
     */
    template<u32 Entities>
    class model_array
    {
        u32 _models [Entities];

        public:

        template<u32 Model>
        void add(u32 e)
        {
            *(_models + e) = Model;
        }

        /**
         * @brief Obtian the model of an entity.
         * 
         * @param e The ID of the entity.
         * @return u32 
         */
        u32 get(u32 e)
        {
            return *(_models + e);
        }
        
        void clear(u32 e)
        {
            *(_models + e) = etnull;
        }

    };

}

#endif