#ifndef ESA_VECTOR_H
#define ESA_VECTOR_H

#include "esa.h"
#include <cassert>


namespace esa
{
    template<typename Type, uint32_t MaxSize>
    class vector
    {
        /**
         * @brief Actual array of data.
         * 
         */
        Type _data [ MaxSize ];


        /**
         * @brief Current vector size.
         * 
         */
        uint32_t _size;


        public:


        /**
         * @brief Constructor.
         * 
         */
        vector()
        {
            _size = 0;
        }


        /**
         * @brief Tells if the vector is full.
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool full()
        {
            return _size == MaxSize;
        }


        /**
         * @brief Tells if the vector is empty.
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool empty()
        {
            return _size == 0;
        }


        /**
         * @brief Add an element to the back of the vector.
         * 
         */
        void push_back(const Type & value)
        {
            assert(!full() && "ESA ERROR: vector is full!");
            ::new(static_cast<void*>(_data + _size)) Type(value);
            _size++;
        }


        /**
         * @brief Remove the last element from the vector.
         * 
         */
        void pop_back()
        {
            assert(!empty() && "ESA ERROR: vector is empty!");
            _size--;
        }


        /**
         * @brief Get a reference to the first element in the vector.
         * 
         * @return Type& 
         */
        [[nodiscard]] Type & front()
        {
            assert(!empty() && "ESA ERROR: vector is empty!");
            return _data[0];
        }


        /**
         * @brief Get a reference to the last element in the vector.
         * 
         * @return Type& 
         */
        [[nodiscard]] Type & back()
        {
            assert(!empty() && "ESA ERROR: vector is empty!");
            return _data[_size - 1];
        }


        /**
         * @brief Insert an element at a certain position (index).
         * 
         * @param position The index to insert to.
         * @param value The value to insert.
         */
        void insert(uint32_t i, const Type & value)
        {
            assert(!full() && "ESA ERROR: vector is full!");
            for (uint32_t j = _size; j > i; j--)
                _data[j] = _data[j - 1];
            ::new(static_cast<void*>(_data + i)) Type(value);
            _size++;
        }


        /**
         * @brief Erase an element at a certain index.
         * 
         * @param index The index of the element to erase.
         */
        void erase(uint32_t i)
        {
            assert(!empty() && "ESA ERROR: index is larger than current vector size!");
            for (uint32_t j = i; j < _size - 1; j++)
                _data[j] = _data[j + 1];
            _data[_size - 1].~Type();
            _size--;
        }


        /**
         * @brief Removes all elements from the vector.
         * 
         */
        void clear()
        {
            _size = 0;
        }


        /**
         * @brief Beginning of vector (iterator).
         * 
         * @return Type* 
         */
        [[nodiscard]] Type * begin()
        {
            return _data;
        }


        /**
         * @brief End of vector (iterator).
         * 
         * @return Type* 
         */
        [[nodiscard]] Type * end()
        {
            return _data + _size;
        }


        /**
         * @brief Tells the current siz e of the vector.
         * 
         * @return uint32_t 
         */
        [[nodiscard]] uint32_t size()
        {
            return _size;
        }


        /**
         * @brief Returns a reference to the element at a certain index.
         * 
         * @param index
         * @return Type& 
         */
        [[nodiscard]] Type & operator[](uint32_t i)
        {
            assert(i < _size && "ESA ERROR: index out of range for esa::vector object!");
            return _data[i];
        }

    };
}


#endif