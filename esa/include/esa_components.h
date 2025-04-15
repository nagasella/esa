#ifndef ESA_COMPONENTS_H
#define ESA_COMPONENTS_H

#include "esa.h"
#include <cassert>


namespace esa
{
    class bool_set
    {
        /**
         * @brief The actual binary data.
         * 
         */
        uint32_t _data;


        public:


        /**
         * @brief Constructor.
         * 
         */
        bool_set()
        {
            _data = 0;
        }


        /**
         * @brief Constructor. Allows to initialize all the 
         * 32 booleans to the same value.
         * 
         * @param value either `true` or `false`.
         */
        bool_set(bool value)
        {
            if (value)
                _data = 0xffffffff;
            else
                _data = 0;
        }


        /**
         * @brief Set one of the 32 boolean values to `true` or `false`.
         * 
         * @tparam Tag The unique tag (`0`...`31`) of the boolean value to set.
         * @param value The boolean value to assign.
         */
        template<tag_t Tag>
        void set(bool value)
        {
            assert(Tag < 31 && "ESA ERROR: bool_set index used it too large! (maximum is 31)");
            if (value)
                _data |= (1 << Tag);
            else
                _data &= ~(1 << Tag);
        }


        /**
         * @brief Obtain the value of one of the 32 booleans, based on its tag.
         * 
         * @tparam Tag The unique tag of the boolean.
         * @return true 
         * @return false
         */
        template<tag_t Tag>
        [[nodiscard]] bool get()
        {
            assert(Tag < 31 && "ESA ERROR: bool_set index used it too large! (maximum is 31)");
            if (((_data >> Tag) & 1) == 1)
                return true;
            return false;
        }

    };


    class uintn_set
    {
        /**
         * @brief The actual binary data.
         * 
         */
        uint32_t _data;


        public:


        /**
         * @brief Constructor.
         * 
         */
        uintn_set()
        {
            _data = 0;
        }


        /**
         * @brief Assign a value to one of the elements.
         * 
         * @tparam Tag The unique tag of the element (`0`...`31`).
         * @tparam Bits The number of bits this element uses.
         * @param value The value to assign (must fit in the number of bits chosen).
         */
        template<tag_t Tag, uint32_t Bits>
        void set(uint32_t value)
        {
            assert(Tag + Bits < 31 && "ESA ERROR: exceeded uintn_set maximum capacity (32 bits!");
            assert(value < (1 << Bits) && "ESA ERROR: trying to set a uintn_set value too large for the number of bits chosen!");
            _data = (_data & ~(((1 << Bits) - 1) << Tag)) | (value << Tag);
        }


        /**
         * @brief Obtain one of the elements, based on its tag and size in bits.
         * 
         * @tparam Tag The unique tag of the element.
         * @tparam Size The size of the element in bits. 
         * @return uint32_t 
         */
        template<tag_t Tag, uint32_t Bits>
        [[nodiscard]] uint32_t get()
        {
            return (_data >> Tag) & ((1 << Bits) - 1);
        }
    };
}

#endif