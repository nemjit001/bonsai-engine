#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Systems/System.hpp>

class MockSystem : public ISystem
{
public:
    MOCK_METHOD(void, Run, (World&, float), (override));
};

TEST(SystemTests, TestRegisterAndRunSystem)
{
    MockSystem system{};
    EXPECT_CALL(system, Run(testing::_, testing::_))
        .Times(1);
}
