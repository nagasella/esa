#ifndef ESA_CACHED_QUERY_H
#define ESA_CACHED_QUERY_H

#include "esa.h"


namespace esa
{
    
    template<uint32_t Entities>
    class cached_query
    {
        /**
         * @brief Unique tag.
         * 
         */
        tag_t _tag;


        /**
         * @brief The IDs of the entities this query processes.
         * 
         */
        vector<entity, Entities> _entities;


        protected:

        
        public:

        /**
         * @brief Constructor.
         * 
         */
        cached_query(tag_t tag)
        {
            _tag = tag;
        }


        /**
         * @brief Filter entities processed by this query based on their components.
         * 
         */
        [[nodiscard]] virtual bool select(entity e)
        {
            return false;
        }


        /**
         * @brief Initialization.
         * 
         */
        virtual void init()
        {

        }


        /**
         * @brief Find entities that satisfy a condtion.
         * 
         */
        [[nodiscard]] virtual bool where(entity e)
        {
            return true;
        }

        
        /**
         * @brief Returns the unique tag associated to the query.
         * 
         * @return tag_t 
         */
        [[nodiscard]] tag_t tag()
        {
            return _tag;
        }


        /**
         * @brief Subscribe the entity to the cached query.
         * 
         */
        void subscribe(entity e)
        {
            for (auto ent : _entities)
            {
                if (ent == e)
                    return;
            }
            if (select(e))
                _entities.push_back(e);
        }


        /**
         * @brief Unsubscribe the entity from the cached query.
         * 
         */
        void unsubscribe(entity e)
        {
            for (uint32_t i = 0; i < _entities.size(); i++)
            {
                if (_entities[i] == e)
                {
                    _entities.erase(i);
                    break;
                }
            }
        }


        /**
         * @brief Returns a vector with the IDs of the entities processed by this query.
         * 
         * @return vector<entity, Entities> 
         */
        [[nodiscard]] vector<entity, Entities> entities()
        {
            return _entities;
        }


        /**
         * @brief Virtual destructor.
         * 
         */
        virtual ~cached_query() = default;

    };

}


#endif