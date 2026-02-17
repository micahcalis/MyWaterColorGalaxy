#include <iostream>
#include "Core/Application/Application.hpp"

int main()
{
    Beer::Core::Application application;

    try
    {
        application.Run();
    } catch (const std::exception& exc)
    {
        std::cerr << exc.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
