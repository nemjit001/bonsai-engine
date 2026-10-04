#include "Application.hpp"
#include <SDL3/SDL_main.h>

int main(int, char*[])
{
    return Bonsai::Application()
        .Run();
}