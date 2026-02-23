#pragma once

#ifndef NO_COPY

    #define NO_COPY(type)                 \
        type(const type& other) = delete; \
        type& operator=(const type& other) = delete;

#endif // ifndef NO_COPY

#ifndef NO_MOVE

    #define NO_MOVE(type)            \
        type(type&& other) = delete; \
        type& operator=(type&& other) = delete;

#endif // ifndef NO_MOVE

#ifndef DEFAULT_COPY

    #define DEFAULT_COPY(type)                      \
        type(const type& other) noexcept = default; \
        type& operator=(const type& other) noexcept = default;

#endif // Default Copy

#ifndef DEFAULT_MOVE

    #define DEFAULT_MOVE(type)                 \
        type(type&& other) noexcept = default; \
        type& operator=(type&& other) noexcept = default;

#endif // Default Move
