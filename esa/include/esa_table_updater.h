/**
 * @file esa_table_updater.h
 * @author nagasella
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ESA_TABLE_UPDATER_H
#define ESA_TABLE_UPDATER_H

#include "esa.h"

namespace esa
{

    template<typename Table>
    class table_updater
    {

        tag_t _tag;

        protected:

        /**
         * @brief A reference to the table this updater works on.
         * 
         */
        Table& table;


        public:


        /**
         * @brief Constructor.
         * 
         * @param t A reference to the table associated to this updater.
         * @param tag_t A unique tag to assign to this udpater.
         */
        table_updater(Table& t, tag_t tag)
            : table(t), _tag(tag)
        {

        }


        /**
         * @brief Get the unique tag assigned to this updater.
         * 
         */
        tag_t tag()
        {
            return _tag;
        }


        /**
         * @brief Initialize the updater.
         * 
         */
        virtual void init()
        {
            
        }

        
        /**
         * @brief Perform operations on the table associated to this updater.
         * 
         */
        virtual void update()
        {

        }


        /**
         * @brief Destructor.
         * 
         */
        virtual ~table_updater() = default;

    };

}

#endif