#ifndef ESA_CACHED_QUERY_H
#define ESA_CACHED_QUERY_H

#include "esa.h"


namespace esa
{

    class icached_query
    {
        /**
         * @brief Unique tag.
         * 
         */
        tag_t _tag;


        public:


        /**
         * @brief Constructor.
         * 
         * @param tag The unique tag to assign to the query.
         */
        icached_query(tag_t tag)
        {
            _tag = tag;
        }


        /**
         * @brief Filter entities processed by this query based on their components.
         * 
         */
        virtual bool select(entity e)
        {
            return false;
        }


        /**
         * @brief Initialize the query.
         * 
         */
        virtual void init()
        {

        }


        /**
         * @brief Find entities that satisfy a condtion.
         * 
         */
        virtual bool where(entity e)
        {
            return true;
        }


        virtual void subscribe(entity e) = 0;


        virtual void unsubscribe(entity e) = 0;


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
         * @brief Virtual destructor.
         * 
         */
        virtual ~icached_query() = default;

    };


    
    template<uint32_t Entities>
    class cached_query : public icached_query
    {
        /**
         * @brief The IDs of the entities subscribed to the query.
         * 
         */
        vector<entity, Entities> _entities;


        public:


        /**
         * @brief Constructor.
         * 
         * @param tag The unique tag to assign to the query.
         */
        cached_query(tag_t tag) : icached_query(tag)
        {

        }


        /**
         * @brief Subscribe an entity to the cached query.
         * 
         */
        void subscribe(entity e) override
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
         * @brief Unsubscribe an entity from the cached query.
         * 
         */
        void unsubscribe(entity e) override
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
         * @brief Returns a vector with the IDs of the entities subscribed to the query.
         * 
         * @return vector<entity, Entities> 
         */
        [[nodiscard]] vector<entity, Entities> subscribed()
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