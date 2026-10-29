#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Systems/SystemRegistry.hpp"

/// @brief Mocks ECS system interface.
class MockSystem : public Bonsai::IECSSystem
{
public:
    MOCK_METHOD(void, Run, (entt::registry&), (override));
};

/// @brief Wraps an ECS system.
class ECSSystemWrapper : public Bonsai::IECSSystem
{
public:
    explicit ECSSystemWrapper(Bonsai::IECSSystem* wrappee)
        : _wrappee(wrappee) {}

    void Run(entt::registry& registry) override
    {
        _wrappee->Run(registry);
    }

private:
    Bonsai::IECSSystem* _wrappee = nullptr;
};

TEST(SystemRegistryTests, TestAddSystem)
{
    // Set up mock system state
    MockSystem mockSystem{};
    EXPECT_CALL(mockSystem, Run(testing::_))
        .Times(1);

    // Create a wrapping ECS system to track mock calls
    Bonsai::SystemRegistry systemRegistry{};
    systemRegistry.AddSystem<ECSSystemWrapper>(&mockSystem);

    // Run all systems using an empty registry
    entt::registry emptyRegistry{};
    systemRegistry.RunSystems(emptyRegistry);
}
