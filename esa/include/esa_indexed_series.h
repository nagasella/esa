#ifndef ESA_INDEXED_SERIES_H
#define ESA_INDEXED_SERIES_H

#include <cassert>
#include <new>

#include "esa.h"
#include "esa_iseries.h"


namespace esa
{
    template<typename ComponentType, uint32_t Size>
    class indexed_series : public iseries
    {
        /**
         * @brief Entity IDs associated to each index.
         * 
         */
        vector<entity, Size> _entities;


        /**
         * @brief Array of components.
         * 
         */
        vector<ComponentType, Size> _data;


        public:


        /**
         * @brief Add a component to the entity.
         * 
         * @param e The ID of the entity.
         * @param c The component instance.
         */
        void add(entity e, const ComponentType & c)
        {
            assert(!_entities.full() && "ESA ERROR: indexed series is full!");
            _entities.push_back(e);
            _data.push_back(c);
        }


        /**
         * @brief Remove a component from an entity.
         * 
         * @param e The ID of the entity.
         */
        void remove(entity e) override
        {
            for (index i = 0; i < _entities.size(); i++)
            {
                if (_entities[i] == e)
                {
                    _entities.erase(i);
                    _data.erase(i);
                    return;
                }
            }
        }


        /**
         * @brief Tells if the entity owns this component.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool has(entity e) override
        {
            for (entity id : _entities)
            {
                if (id == e)
                    return true;
            }
            return false;
        }
        

        /**
         * @brief Returns a reference to the component based on its index.
         * 
         * @param i The index value.
         * @return ComponentType& 
         */
        [[nodiscard]] ComponentType & get(index i)
        {
            assert(i < _entities.size() && "ESA ERROR: indexed series index is out of bounds!");
            return _data[i];
        }


        /**
         * @brief Returns a reference to the component based on an entity ID.
         * 
         * @param i The index value.
         * @return ComponentType& 
         */
        [[nodiscard]] ComponentType & lookup(entity e)
        {
            for (index i = 0; i < _entities.size(); i++)
            {
                if (e == _entities[i])
                    return _data[i];
            }
            assert(1 == 2 && "ESA ERROR: entity does not own this indexed component!");
        }


        /**
         * @brief Returns the entity ID associated to a certain index.
         * 
         * @param i The index value.
         * @return entity 
         */
        [[nodiscard]] entity id(index i)
        {
            assert(i < _entities.size() && "ESA ERROR: indexed series index is out of bounds!");
            return _entities[i];
        }


        /**
         * @brief Returns the index of a certain entity.
         * 
         * @param e The ID of the entity.
         * @return index 
         */
        [[nodiscard]] index find(entity e)
        {
            for (index i = 0; i < _entities.size(); i++)
            {
                if (_entities[i] == e)
                    return i;
            }
            assert(1 == 2 && "ESA ERROR: entity does not own this indexed component!");
            return 0;
        }


        /**
         * @brief Returns a reference to the element at requested index.
         * 
         * @param i
         * @return T& 
         */
        [[nodiscard]] ComponentType & operator[](index i)
        {
            assert(i < _entities.size() && "ECSA ERROR: indexed series index out of range!");
            return _data[i];
        }


        /**
         * @brief Rturns the current size of the series.
         * 
         * @return uint32_t 
         */
        [[nodiscard]] uint32_t size()
        {
            return _entities.size();
        }

    };

}

#endif