#ifndef ESA_ISERIES_H
#define ESA_ISERIES_H

#include <cassert>

#include "esa.h"


namespace esa
{
    class iseries
    {
        public:


        /**
         * @brief Mark an entity as not owning this component.
         * 
         * @param e The ID of the entity.
         */
        virtual void remove(entity e) = 0;


        /**
         * @brief Tells if the entity owns this component.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        [[nodiscard]] virtual bool has(entity e)
        {
            return false;
        }


        /**
         * @brief Virtual destructor.
         * 
         */
        virtual ~iseries()
        {

        }

    };

}

#endif