#ifndef ESA_VECTOR_H
#define ESA_VECTOR_H

#include "esa.h"
#include <cassert>


namespace esa
{
    template<typename T, uint32_t MaxSize>
    class vector
    {
        /**
         * @brief Actual array of data.
         * 
         */
        T _data [ MaxSize ];


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
         * @brief Add an element to the vector.
         * 
         */
        void push_back(const T value)
        {
            assert(!full() && "ESA ERROR: vector is full!");
            _data[_size] = value;
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
         * @brief Get a reference to the last element in the vector.
         * 
         * @return T& 
         */
        [[nodiscard]] T & front()
        {
            assert(!empty() && "ESA ERROR: vector is empty!");
            return _data[0];
        }


        /**
         * @brief Get a reference to the last element in the vector.
         * 
         * @return T& 
         */
        [[nodiscard]] T & back()
        {
            assert(!empty() && "ESA ERROR: vector is empty!");
            return _data[_size - 1];
        }


        /**
         * @brief Insert an element at a certain position.
         * 
         * @param position The index to insert to.
         * @param value The value to insert.
         */
        void insert(uint32_t index, const T value)
        {
            assert(!full() && "ESA ERROR: vector is full!");
            for (uint32_t i = _size; i > index; i--)
                _data[i] = _data[i - 1];
            _data[index] = value;
            _size++;
        }


        /**
         * @brief Erase an element at a certain index.
         * 
         * @param index The index of the element to erase.
         */
        void erase(uint32_t index)
        {
            assert(!empty() && "ESA ERROR: index is larger than current vector size!");
            for (uint32_t i = index; i < _size - 1; i++)
                _data[i] = _data[i + 1];
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
         * @return T* 
         */
        [[nodiscard]] T * begin()
        {
            return _data;
        }


        /**
         * @brief End of vector (iterator).
         * 
         * @return T* 
         */
        [[nodiscard]] T * end()
        {
            return _data + _size;
        }


        /**
         * @brief Tells the current vector size.
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
         * @return T& 
         */
        [[nodiscard]] T & operator[](uint32_t index)
        {
            assert(index < _size && "ESA ERROR: index out of range for esa::vector object!");
            return _data[index];
        }

    };
}


#endif