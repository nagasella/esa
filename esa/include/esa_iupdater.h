#ifndef ESA_IUPDATER_H
#define ESA_IUPDATER_H

#include "esa.h"


namespace esa
{
    class iupdater
    {

        /**
         * @brief Unique tag.
         * 
         */
        tag_t _tag;


        /**
         * @brief The type of updater. (table updter ot entity updater)
         * 
         */
        udpater_type _type;


        public:


        /**
         * @brief Constructor.
         * 
         * @param tag 
         */
        iupdater(tag_t tag, udpater_type type)
        {
            _tag = tag;
            _type = type;
        }


        /**
         * @brief Tells the type of this updater. (entity updater or table updater)
         * 
         * @return udpater_type 
         */
        udpater_type type()
        {
            return _type;
        }


        /**
         * @brief Returns the unique tag associated to the updater.
         * 
         * @return tag_t 
         */
        tag_t tag()
        {
            return _tag;
        }


        /**
         * @brief Initialzie the updater.
         * 
         */
        virtual void init()
        {

        }


        /**
         * @brief Virtual destructor.
         * 
         */
        virtual ~iupdater() = default;

    };
}

#endif