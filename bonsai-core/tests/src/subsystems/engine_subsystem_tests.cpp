#include <gtest/gtest.h>
#include <bonsai/subsystems/engine_subsystem.hpp>

namespace bonsai::subsystems
{
    class TestSubsystem : public IEngineSubsystem
    {
    public:
        bool init() override { return true; }
        
        void shutdown() override {}

        void process_event(EngineEvent const& event) override
        {
            if (event.event_type == BONSAI_ENGINE_EVENT_TYPE_QUIT)
            {
                m_received_quit = true;
            }
        }

        bool received_quit_event() { return m_received_quit; };

    private:
        bool m_received_quit = false;
    };

    TEST(EngineSubsystemTests, TestSubsystemRegistryInit)
    {
        EngineSubsystemRegistry subsystem_registry{};
        ASSERT_TRUE(subsystem_registry.init());
        subsystem_registry.shutdown();
    }

    TEST(EngineSubsystemTests, TestRegisterSubsystem)
    {
        EngineSubsystemRegistry subsystem_registry{};
        ASSERT_TRUE(subsystem_registry.init());

        TestSubsystem test_system{};
        ASSERT_FALSE(test_system.received_quit_event());

        EngineSubsystemRegistry::register_subsystem(&test_system);
        EngineSubsystemRegistry::notify_subsystems(EngineEvent{ BONSAI_ENGINE_EVENT_TYPE_QUIT });

        ASSERT_TRUE(test_system.received_quit_event());
        EngineSubsystemRegistry::unregister_subsystem(&test_system);
        subsystem_registry.shutdown();
    }
} // namespace bonsai::subsystems
