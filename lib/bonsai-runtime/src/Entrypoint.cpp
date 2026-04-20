#include "Engine.hpp"

/// @brief Default entrypoint for bonsai applications.
/// @param argc 
/// @param argv 
/// @return 
int main(int, char*[])
{
    return bonsai::Engine().Run();
}
