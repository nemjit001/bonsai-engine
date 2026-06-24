#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Systems/System.hpp>
#include <Systems/SystemScheduler.hpp>
#include <World.hpp>

class MockSystem : public ISystem
{
public:
    MOCK_METHOD(void, Run, (World&, float), (override));
};

TEST(SystemTests, TestRegisterAndRunSystem)
{
    auto system = std::make_unique<MockSystem>();
    EXPECT_CALL(*system, Run(testing::_, testing::_))
        .Times(1);

    SystemScheduler scheduler{};
    scheduler.Register(SystemPriority_Tick, std::move(system));

    World world{};
    scheduler.RunSystems(world, 0.0F);
}
