#ifndef ESA_TABLE_UPDATER_H
#define ESA_TABLE_UPDATER_H

#include "esa.h"


namespace esa
{
    class table_updater : public iupdater
    {
        public:


        /**
         * @brief Constructor.
         * 
         * @param tag The tag to assign to the updater.
         */
        table_updater(tag_t tag) : iupdater(tag)
        {

        }

        
        /**
         * @brief Update logic.
         * 
         */
        virtual void update() = 0;


        /**
         * @brief Virtual destructor.
         * 
         */
        virtual ~table_updater() = default;

    };
}

#endif