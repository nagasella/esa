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

        u32 _tag;

        protected:

        /**
         * @brief A reference to the `entity_table` this updater works on.
         * 
         */
        Table& table;


        public:


        /**
         * @brief Constructor.
         * 
         * @param t A reference to the `entity_table` associated to this updater.
         * @param tag The tag to assign to this udpater.
         */
        table_updater(Table& t, u32 tag)
            : table(t), _tag(tag)
        {

        }


        /**
         * @brief Get the tag assigned to this updater.
         * 
         */
        virtual u32 tag()
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
         * @brief Perform operations on the `entitiy_table` associated to this updater.
         * 
         */
        virtual void update()
        {

        }


        /**
         * @brief Destructor.
         * 
         */
        virtual ~table_updater()
        {

        }

    };

}

#endif