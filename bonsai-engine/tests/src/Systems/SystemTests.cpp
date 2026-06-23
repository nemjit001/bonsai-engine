#include <gtest/gtest.h>
#include <Systems.hpp>

class FooSystem : public System {};

TEST(SystemTests, TestRegisterSystem)
{
    SystemRegistry registry{};
    registry.Register<FooSystem>();
}

