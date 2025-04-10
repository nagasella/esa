#ifndef ESA_SERIES_H
#define ESA_SERIES_H

#include "esa.h"
#include <cassert>


namespace esa
{
    template<uint32_t Entities>
    class iseries
    {
        /**
         * @brief Entity mask.
         * 
         */
        entity_mask<Entities> _emask;


        protected:


        /**
         * @brief Mark an entity as owning this component.
         * 
         * @param e The ID of the entity.
         */
        void add(entity e)
        {
            _emask.add(e);
        }


        public:


        /**
         * @brief Mark an entity as not owning this component.
         * 
         * @param e The ID of the entity.
         */
        virtual void remove(entity e)
        {
            _emask.remove(e);
        }


        /**
         * @brief Tells if the entity owns this component.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool has(entity e)
        {
            return _emask.contains(e);
        }


        /**
         * @brief Virtual destructor.
         * 
         */
        virtual ~iseries()
        {

        }

    };


    template<typename ComponentType, uint32_t Entities>
    class series : public iseries<Entities>
    {
        /**
         * @brief Array of components.
         * 
         */
        ComponentType _array [ Entities ];


        public:


        /**
         * @brief Add a compoennt to the entity.
         * 
         * @param e The ID of the entity.
         * @param c The component instance.
         */
        void add(entity e, ComponentType c)
        {
            iseries<Entities>::add(e);
            _array[e] = c;
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
            return _array[e];
        }

    };

}

#endif