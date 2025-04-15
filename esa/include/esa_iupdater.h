#ifndef ESA_IUPDATER_H
#define ESA_IUPDATER_H

#include "esa.h"


namespace esa
{
    class iupdater
    {

        /**
         * @brief Unique tag of the updater.
         * 
         */
        tag_t _tag;


        /**
         * @brief Tells if the `update` function will be called by the table.
         * 
         */
        bool _active;


        public:


        /**
         * @brief Constructor.
         * 
         * @param tag The unique tag to assign to the updater.
         */
        iupdater(tag_t tag)
        {
            _tag = tag;
            _active = true;
        }


        /**
         * @brief Returns the unique tag associated to the updater.
         * 
         * @return tag_t 
         */
        [[nodiscard]] tag_t tag()
        {
            return _tag;
        }


        /**
         * @brief Tells if the updater is currently active.
         * If it is, it is processed by `entity_table::update()`.
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool active()
        {
            return _active;
        }


        /**
         * @brief Activate this updater (it will be processed
         * by `entity_table::update()`).
         * 
         */
        void activate()
        {
            _active = true;
        }


        /**
         * @brief Deactivate this updater (it will not be processed
         * by `entity_table::update()`).
         * 
         */
        void deactivate()
        {
            _active = false;
        }


        /**
         * @brief Initialzie the updater.
         * 
         */
        virtual void init()
        {
            
        }


        /**
         * @brief Execute update logic.
         * 
         */
        virtual void update() = 0;


        /**
         * @brief Tells if entities can be subscribed to this updater.
         * 
         * @return true 
         * @return false 
         */
        virtual bool subscribable()
        {
            return false;
        }


        /**
         * @brief Virtual destructor.
         * 
         */
        virtual ~iupdater() = default;

    };


    class isubscribable_updater : public iupdater
    {
        public:
        

        /**
         * @brief Constructor.
         * 
         * @param tag The unique tag for the updater.
         */
        isubscribable_updater(tag_t tag)
            : iupdater(tag)
        {
        }


        /**
         * @brief Subscribe an entity to the updater.
         * 
         * @param e The ID of the entity.
         */
        virtual void subscribe(entity e) = 0;


        /**
         * @brief Unsubscribe an entity from an updater.
         * 
         * @param e The ID of the entity.
         */
        virtual void unsubscribe(entity e) = 0;


        virtual void unsubscribe(entity e, bool destroy) = 0;


        /**s
         * @brief Tells if entities can be subscribed to this updater.
         * 
         */
        bool subscribable() override
        {
            return true;
        }


        virtual ~isubscribable_updater() = default;
    };
}

#endif