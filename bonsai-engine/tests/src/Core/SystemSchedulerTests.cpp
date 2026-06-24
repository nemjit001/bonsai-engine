#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Core.hpp>
#include <World.hpp>

class MockSystem : public ISystem
{
public:
    MOCK_METHOD(void, Run, (World&, float), (override));
};

TEST(SystemSchedulerTests, TestRegisterAndRunSystem)
{
    auto system = std::make_unique<MockSystem>();
    EXPECT_CALL(*system, Run(testing::_, testing::_))
        .Times(1);

    SystemScheduler scheduler{};
    scheduler.Register(SystemPriority_Tick, std::move(system));

    World world{};
    scheduler.RunSystems(world, 0.0F);
}
