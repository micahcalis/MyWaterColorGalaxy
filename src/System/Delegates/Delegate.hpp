#pragma once

#include <functional>
#include <memory>
#include <type_traits>

namespace Beer::System
{
    template<typename ReturnType, typename... Args>
    using Function = std::function<ReturnType(Args...)>;

    class Delegate
    {
    public:
        template<typename T, typename ReturnType, typename... Args>
        static Function<ReturnType, Args...> BindSafe(
            std::weak_ptr<T> weakObj,
            ReturnType (T::*func)(Args...))
        {
            return [weakObj, func](Args... args) -> ReturnType {
                if (auto sharedObj = weakObj.lock())
                {
                    return (sharedObj.get()->*func)(std::forward<Args>(args)...);
                }

                if constexpr (!std::is_void_v<ReturnType>)
                {
                    return ReturnType{};
                }
            };
        }
    };
} // namespace Beer::System
