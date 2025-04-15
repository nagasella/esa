#ifndef ESA_INDEX_UPDATER_H
#define ESA_INDEX_UPDATER_H

#include "esa.h"
#include "esa_iupdater.h"
#include "esa_indexed_series.h"


namespace esa
{
    
    template<typename ComponentType, uint32_t Size>
    class index_updater : public isubscribable_updater
    {
        /**
         * @brief The indexes of the entities subscribed to the updater.
         * 
         */
        vector<index, Size> _indexes;


        protected:


        /**
         * @brief A reference to the indexed series the updater works on.
         * 
         */
        indexed_series<ComponentType, Size> & series;

        
        public:


        /**
         * @brief Constructor.
         * 
         */
        index_updater(indexed_series<ComponentType, Size> & s, tag_t tag) 
            : isubscribable_updater(tag),
            series(s)
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
         * @brief Subscribe an entity to this udpater.
         * 
         */
        void subscribe(entity e) override
        {
            if (!series.has(e))
                return;
            index i = series.find(e);
            for (index ii : _indexes)
            {
                if (ii == i)
                    return;
            }
            if (select(e))
                _indexes.push_back(i);
        }


        /**
         * @brief Unsubscribe an entity from this udpater.
         * 
         */
        void unsubscribe(entity e) override
        {
            for (uint32_t j = 0; j < _indexes.size(); j++)
            {
                index i = _indexes[j];
                if (series.id(i) == e)
                {
                    _indexes.erase(j);
                    return;
                }
            }
        }


        void unsubscribe(entity e, bool destroy) override
        {
            if (!destroy)
            {
                unsubscribe(e);
                return;
            }
            for (uint32_t j = 0; j < _indexes.size(); j++)
            {
                index i = _indexes[j];
                if (series.id(i) == e)
                {
                    _indexes.erase(j);
                    for (uint32_t jj = 0; jj < _indexes.size(); jj++)
                    {
                        if (_indexes[jj] > i)
                            _indexes[jj]--;
                    }
                    return;
                }
            }
        }


        /**
         * @brief Returns a vector with the indexes of the entities currently subscribed to the entity updater.
         * 
         * @return vector<entity, Size> 
         */
        [[nodiscard]] vector<index, Size> subscribed()
        {
            return _indexes;
        }


        /**
         * @brief Virtual destructor.
         * 
         */
        virtual ~index_updater() = default;

    };

}


#endif