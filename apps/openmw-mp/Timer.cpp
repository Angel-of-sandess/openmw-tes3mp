//
// Created by koncord on 12.08.17.
//

#include <chrono>
#include <memory>

#include "Script/LuaState.hpp"

#include "Timer.hpp"

using namespace std;

void Timer::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Timer>("Timer",
                                        "start", &Timer::start,
                                        "stop", &Timer::stop,
                                        "restart", &Timer::restart,
                                        "isElapsed", &Timer::isElapsed
    );
}

bool Timer::isElapsed()
{
    return end;
}

void Timer::stop()
{
    end = true;
}

void Timer::start()
{
    end = false;

    const auto duration = chrono::system_clock::now().time_since_epoch();
    const auto msec = chrono::duration_cast<chrono::milliseconds>(duration).count();
    startTime = msec;
}

void Timer::restart(long msec)
{
    targetMsec = msec;
    start();
}

Timer::Timer(sol::environment &env, sol::function &callback, long msec, sol::table &args) : callback(std::move(callback)),
                                                                                         data(std::move(args)),
                                                                                         env(std::move(env))
{
    LOG_MESSAGE_SIMPLE(Log::LOG_TRACE, "Timer::Timer()");
    targetMsec = msec;
    end = true;
    markedToDelete = false;
}

Timer::~Timer()
{
    LOG_MESSAGE_SIMPLE(Log::LOG_TRACE, "Timer::~Timer()");
}

void Timer::tick()
{
    if (end || markedToDelete)
        return;

    const auto duration = chrono::system_clock::now().time_since_epoch();
    const auto time = chrono::duration_cast<chrono::milliseconds>(duration).count();

    if (time - startTime >= targetMsec)
    {
        end = true;
        if (callback.valid())
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_TRACE, "Timer::tick time expired, callback %p valid", callback.registry_index());
            callback.call(data);
        }
        else
            LOG_MESSAGE_SIMPLE(Log::LOG_TRACE, "Timer::tick time expired, callback invalid");
    }
}

void Timer::kill()
{
    markedToDelete = true;
}

void TimerController::Init(LuaState &lua)
{
    sol::table timerTable = lua.getState()->create_table("TimerCtrl");

    timerTable.set_function("create", [&lua](sol::function callback, long msec, sol::table args, sol::this_environment env) {
        return lua.getTimerCtrl().create(env, callback, msec, args);
    });

    timerTable.set_function("kill", [&lua](std::shared_ptr<Timer> timer) {
        lua.getTimerCtrl().kill(timer);
    });
}

std::shared_ptr<Timer> TimerController::create(sol::environment env, sol::function callback, long msec, sol::table args)
{
    newTimersQueue.emplace_back(new Timer(env, callback, msec, args));
    return newTimersQueue.back();
}

void TimerController::kill(const std::shared_ptr<Timer> &timer)
{
    LOG_MESSAGE_SIMPLE(Log::LOG_TRACE, "Timer %p marked for deletion\n", timer.get());
    timer->kill();
    haveMarkedToDeletion = true;
}

void TimerController::tick()
{
    if (haveMarkedToDeletion)
    {
        size_t deleted = timers.size();
        haveMarkedToDeletion = false;
        timers.erase(remove_if(timers.begin(), timers.end(), [](const std::shared_ptr<Timer> &timer) {
            return timer->isMarkedToDelete();
        }), timers.end());
        deleted -= timers.size();
        LOG_MESSAGE_SIMPLE(Log::LOG_TRACE, "Deleted %d timers\n", deleted);
    }

    if (!newTimersQueue.empty())
    {
        timers.insert(timers.begin(), make_move_iterator(newTimersQueue.begin()), make_move_iterator(newTimersQueue.end()));
        LOG_MESSAGE_SIMPLE(Log::LOG_TRACE, "Created %d timers\n", newTimersQueue.size());
        newTimersQueue.clear();
    }

    for (auto &timer : timers)
    {
        timer->tick();
    }
}

void TimerController::terminate()
{
    timers.clear();
}
