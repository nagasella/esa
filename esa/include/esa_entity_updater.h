#ifndef ESA_ENTITY_UPDATER_H
#define ESA_ENTITY_UPDATER_H

#include "esa.h"
#include "esa_iupdater.h"


namespace esa
{
    
    template<uint32_t Entities>
    class entity_updater : public iupdater
    {
        /**
         * @brief The IDs of the entities this updater processes.
         * 
         */
        vector<entity, Entities> _entities;

        
        public:


        /**
         * @brief Constructor.
         * 
         */
        entity_updater(tag_t tag) : iupdater::iupdater(tag, udpater_type::ENTITY_UPDATER)
        {

        }


        /**
         * @brief Filter entities processed by this udpater based on their components.
         * 
         */
        [[nodiscard]] virtual bool select(entity e)
        {
            return false;
        }


        /**
         * @brief Update logic.
         * 
         */
        virtual void update(entity e) = 0;


        /**
         * @brief Subscribe an entity to this udpater.
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
         * @brief Unsubscribe an entity from this udpater.
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
         * @brief Returns a vector with the IDs of the entities processed by this updater.
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
        virtual ~entity_updater() = default;

    };

}


#endif