#ifndef ESA_ENTITY_UPDATER_H
#define ESA_ENTITY_UPDATER_H

#include "esa.h"
#include "esa_iupdater.h"


namespace esa
{
    
    template<uint32_t Entities>
    class entity_updater : public isubscribable_updater
    {
        /**
         * @brief The IDs of the entities subscribed to the updater.
         * 
         */
        vector<entity, Entities> _entities;

        
        public:


        /**
         * @brief Constructor.
         * 
         */
        entity_updater(tag_t tag) : isubscribable_updater(tag)
        {

        }


        /**
         * @brief Filter entities processed by this udpater based on their components.
         * 
         */
        virtual bool select(entity e)
        {
            return false;
        }


        /**
         * @brief Subscribe an entity to the udpater.
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
         * @brief Unsubscribe an entity from the udpater.
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


        void unsubscribe(entity e, bool destroy)
        {
            unsubscribe(e);
        }


        /**
         * @brief Returns a vector with the IDs of the entities currently subscribed to the updater.
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
        virtual ~entity_updater() = default;

    };

}


#endif