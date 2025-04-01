#ifndef ECSA_ARRAY_H
#define ECSA_ARRAY_H

#include "esa.h"
#include <cassert>


namespace esa
{
    template<typename T, uint32_t Size>
    class array
    {
        /**
         * @brief Actual array of data.
         * 
         */
        T _data [ Size ];


        public:


        /**
         * @brief Constructor.
         * 
         */
        array()
        {

        }


        /**
         * @brief Constructor. Allows to initialize the array elements to a specific value.
         * 
         * @param value The value to assign for initialization.
         */
        array(T value)
        {
            for (uint32_t i = 0; i < Size; i++)
                _data[i] = value;
        }


        /**
         * @brief Tells the current vector size.
         * 
         * @return uint32_t 
         */
        [[nodiscard]] uint32_t size()
        {
            return Size;
        }


        /**
         * @brief Returns a reference to the element at requested index.
         * 
         * @param index
         * @return T& 
         */
        [[nodiscard]] T & operator[](uint32_t index)
        {
            assert(index < Size && "ECSA ERROR: array index out of range!");
            return _data[index];
        }

    };
}


#endif