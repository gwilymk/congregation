#include <stdexcept>
#include "rts/common.hpp"

namespace rts
{
    namespace holders
    {
        template <class Resource>
        template <typename Parameter>
        void Holder<Resource>::load(const std::string &name, const std::string &filename, const Parameter &second_param)
        {
            std::unique_ptr<Resource> resource(new Resource());
            if(!resource->loadFromFile(filename, second_param))
                throw std::runtime_error("Failed to load resource: " + filename);

            auto inserted = m_resource_map.insert(std::make_pair(name, std::move(resource)));
            ASSERT(inserted.second);
        }

        template <class Resource>
        void Holder<Resource>::load(const std::string &name, const std::string &filename)
        {
            std::unique_ptr<Resource> resource(new Resource());
            if(!resource->loadFromFile(filename))
                throw std::runtime_error("Failed to load resource: " + filename);

            auto inserted = m_resource_map.insert(std::make_pair(name, std::move(resource)));
            ASSERT(inserted.second);
        }

        template <class Resource>
        Resource &Holder<Resource>::get(const std::string &name)
        {
            auto found = m_resource_map.find(name);
            ASSERT(found != m_resource_map.end());

            return *found->second;
        }

        template <class Resource>
        const Resource &Holder<Resource>::get(const std::string &name) const
        {
            auto found = m_resource_map.find(name);
            ASSERT(found != m_resource_map.end());

            return *found->second;
        }
    }
}
