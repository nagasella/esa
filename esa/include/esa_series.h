#ifndef ESA_SERIES_H
#define ESA_SERIES_H

#include "esa.h"
#include "esa_iseries.h"
#include <cassert>


namespace esa
{
    template<typename ComponentType, uint32_t Entities>
    class series : public iseries
    {
        /**
         * @brief Entity mask.
         * 
         */
        entity_mask<Entities> _emask;


        /**
         * @brief Array of components.
         * 
         */
        ComponentType _data [ Entities ];


        public:


        /**
         * @brief Add a compoennt to the entity.
         * 
         * @param e The ID of the entity.
         * @param c The component instance.
         */
        void add(entity e, const ComponentType & c)
        {
            _emask.add(e);
            ::new(static_cast<void*>(_data + e)) ComponentType(c);
        }


        /**
         * @brief Remove a component from an entity.
         * 
         * @param e The ID of the entity.
         */
        void remove(entity e) override
        {
            _emask.remove(e);
            _data[e].~ComponentType();
        }


        /**
         * @brief Tells if the entity owns this component.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        [[odiscard]] bool has(entity e) override
        {
            return _emask.contains(e);
        }
        

        /**
         * @brief Returns a reference to the component for an entity.
         * 
         * @param e The ID of the enttity.
         * @return ComponentType& 
         */
        [[nodiscard]] ComponentType & get(entity e)
        {
            assert(this->has(e) && "ESA ERROR: entity does not own the requested component!");
            return _data[e];
        }


        /**
         * @brief Returns a reference to the element at requested index.
         * 
         * @param index
         * @return T& 
         */
        [[nodiscard]] ComponentType & operator[](uint32_t i)
        {
            assert(i < Entities && "ECSA ERROR: series index out of range!");
            return _data[i];
        }

    };

}

#endif