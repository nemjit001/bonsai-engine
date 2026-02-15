#include <gtest/gtest.h>
#include <bonsai/subsystems/engine_event_manager.hpp>
#include <bonsai/subsystems/engine_event.hpp>

namespace bonsai::subsystems
{
    class TestSystem : public IEngineSubsystem
    {
    public:
        void process_event(EngineEvent const& event) override
        {
            if (event.event_type == BONSAI_ENGINE_EVENT_TYPE_QUIT)
            {
                m_quit_received = true;
            }
        }

        bool quit_received()
        {
            return m_quit_received;
        }

    private:
        bool m_quit_received = false;
    };

    TEST(EngineEventManagerTests, TestInitEventManager)
    {
        auto& event_manager = EngineEventManager::get();
        ASSERT_TRUE(event_manager.init());
        event_manager.shutdown();
    }

    TEST(EngineEventManagerTests, TestRegisterAndNotifySubsystem)
    {
        auto& event_manager = EngineEventManager::get();
        ASSERT_TRUE(event_manager.init());
        
        // Set up the test system & register with event manager
        TestSystem test_system{};
        event_manager.register_subsystem(&test_system);

        // Notify registered systems of quit event
        event_manager.notify(EngineEvent{ BONSAI_ENGINE_EVENT_TYPE_QUIT });
        event_manager.run();
        EXPECT_TRUE(test_system.quit_received());
        
        event_manager.shutdown();
    }
} // namespace bonsai::subsystems
