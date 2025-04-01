#ifndef ESA_COMPONENTS_H
#define ESA_COMPONENTS_H

#include "esa.h"


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
         * @brief Constructor. Allows to specify initial boolean values
         * for the 32 entiries.
         * 
         * @param value The value to assign.
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
         * @tparam Tag The unique tag of the boolean to set.
         * @param value The boolean value to assign.
         */
        template<tag_t Tag>
        void set(bool value)
        {
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
         * @tparam Tag The unique tag of the element.
         * @tparam Bits The number of bits this element uses.
         * @param value The value to assign.
         */
        template<tag_t Tag, uint32_t Bits>
        void set(uint32_t value)
        {
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