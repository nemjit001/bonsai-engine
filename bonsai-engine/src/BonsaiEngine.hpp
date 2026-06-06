#ifndef BONSAI_ENGINE_HPP
#define BONSAI_ENGINE_HPP

#include <BonsaiBase.hpp>

/// @brief Enumeration of engine return codes.
enum BonsaiEngineResult : int32
{
    BonsaiEngineResult_Ok = 0,
};

class BonsaiEngine
{
public:
    /// @brief Run the engine application.
    /// @return A BonsaiEngineResult return code.
    int32 Run();
};

#endif //BONSAI_ENGINE_HPP
