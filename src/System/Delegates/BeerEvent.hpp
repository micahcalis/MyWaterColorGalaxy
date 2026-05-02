#pragma once

#include "System/Delegates/Delegate.hpp"
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <utility>

namespace Beer::System
{
    static const int32_t INVALID_TOKEN = -1;

    using SubscriptionToken = int32_t;

    template<typename Signature>
    class BeerEvent;

    template<typename... Args>
    class BeerEvent<void(Args...)>
    {
    private:
        std::unordered_map<SubscriptionToken, std::function<void(Args...)>> subscribers;
        SubscriptionToken nextToken = 0;

    public:
        SubscriptionToken Subscribe(std::function<void(Args...)> subscriber)
        {
            SubscriptionToken token = nextToken++;
            subscribers[token] = std::move(subscriber);
            return token;
        }

        void Unsubscribe(SubscriptionToken token)
        {
            if (auto it = subscribers.find(token); it != subscribers.end())
            {
                it->second = nullptr;
            }
        }

        void Invoke(Args... args)
        {
            std::erase_if(subscribers, [](auto& subscriber) -> bool {
                return subscriber.second == nullptr;
            });

            for (auto& [token, subscriber] : subscribers)
            {
                if (subscriber)
                {
                    subscriber(std::forward<Args>(args)...);
                }
            }
        }
    };
} // namespace Beer::System
