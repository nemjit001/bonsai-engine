#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SDL3/SDL.h>
#include <Core.hpp>

class MockSubsystem : public IEngineSubsystem
{
public:
    MOCK_METHOD(void, ProcessEvent, (SDL_Event const&), (override));
};

TEST(EventLoopManagerTests, TestRunState)
{
    EventLoopManager eventloop{};
    EXPECT_FALSE(eventloop.IsRunning());

    EXPECT_TRUE(eventloop.Init());
    EXPECT_TRUE(eventloop.IsRunning());

    eventloop.Shutdown();
    EXPECT_FALSE(eventloop.IsRunning());
}

TEST(EventLoopManagerTests, TestRegisterSubsystem)
{
    ASSERT_TRUE(SDL_Init(SDL_INIT_EVENTS));

    EventLoopManager eventloop{};
    EXPECT_TRUE(eventloop.Init());

    MockSubsystem mock{};
    eventloop.RegisterSubsystem(&mock);

    EXPECT_CALL(mock, ProcessEvent(testing::_))
        .Times(1);

    SDL_Event quitEvent{ SDL_EVENT_QUIT };
    SDL_PushEvent(&quitEvent);
    eventloop.PumpMessages();

    eventloop.Shutdown();
    SDL_Quit();
}
