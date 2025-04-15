#ifndef ESA_CACHED_APPLY_H
#define ESA_CACHED_APPLY_H

#include "esa.h"


namespace esa
{
    
    class icached_apply
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
         */
        icached_apply(tag_t tag)
        {
            _tag = tag;
        }


        /**
         * @brief Filter entities processed by this apply based on their components.
         * 
         */
        virtual bool select(entity e)
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
         * @brief Modify entities that satisfy a condtion.
         * 
         */
        virtual bool apply(entity e)
        {
            return true;
        }


        virtual void subscribe(entity e) = 0;


        virtual void unsubscribe(entity e) = 0;


        /**
         * @brief Returns the unique tag associated to the apply.
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
        virtual ~icached_apply() = default;
        
    };
    
    template<uint32_t Entities>
    class cached_apply : public icached_apply
    {
        /**
         * @brief The IDs of the entities subscribed to the apply.
         * 
         */
        vector<entity, Entities> _entities;

        
        public:

        /**
         * @brief Constructor.
         * 
         */
        cached_apply(tag_t tag) : icached_apply(tag)
        {

        }


        /**
         * @brief Subscribe an entity to the cached apply object.
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
         * @brief Unsubscribe an entity from the cached apply object.
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
         * @brief Returns a vector with the IDs of the entities subscribed to this apply.
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
        virtual ~cached_apply() = default;

    };

}


#endif