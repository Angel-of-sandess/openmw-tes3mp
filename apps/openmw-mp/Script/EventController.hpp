//
// Created by koncord on 30.07.17.
//

#pragma once

#include "sol.hpp"
#include "Utils.hpp"
#include "LuaState.hpp"

typedef unsigned Event;

namespace CoreEvent
{
    enum
    {
        ON_EXIT = 0,
        ON_POST_INIT,
        ON_REQUEST_PLUGIN_LIST,
        ON_PLAYER_CONNECT,
        ON_PLAYER_DISCONNECT,
        ON_PLAYER_DEATH,
        ON_PLAYER_RESURRECT,
        ON_PLAYER_CELLCHANGE,
        ON_PLAYER_KILLCOUNT,
        ON_PLAYER_ATTRIBUTE,
        ON_PLAYER_SKILL,
        ON_PLAYER_LEVEL,
        ON_PLAYER_BOUNTY,
        ON_PLAYER_REPUTATION,
        ON_PLAYER_EQUIPMENT,
        ON_PLAYER_INVENTORY,
        ON_PLAYER_JOURNAL,
        ON_PLAYER_FACTION,
        ON_PLAYER_SHAPESHIFT,
        ON_PLAYER_SPELLBOOK,
        ON_PLAYER_QUICKKEYS,
        ON_PLAYER_TOPIC,
        ON_PLAYER_DISPOSITION,
        ON_PLAYER_BOOK,
        ON_PLAYER_MISCELLANEOUS,
        ON_PLAYER_INTERACTION,
        ON_PLAYER_REST,
        ON_PLAYER_SENDMESSAGE,
        ON_PLAYER_ENDCHARGEN,
        ON_PLAYER_WEATHER,

        ON_RECORD_DYNAMIC,

        ON_CHANNEL_ACTION,

        ON_MP_REFNUM,

        ON_ACTOR_EQUIPMENT,
        ON_ACTOR_CELL_CHANGE,
        ON_ACTOR_LIST,
        ON_ACTOR_TEST,

        ON_CELL_LOAD,
        ON_CELL_UNLOAD,
        ON_CELL_DELETION,

        ON_CONTAINER,
        ON_DOOR_STATE,
        ON_OBJECT_PLACE,
        ON_OBJECT_STATE,
        ON_OBJECT_SPAWN,
        ON_OBJECT_DELETE,
        ON_OBJECT_LOCK,
        ON_OBJECT_SCALE,
        ON_OBJECT_TRAP,

        ON_WORLD_MAP,

        LAST,
    };
    const int FIRST = ON_EXIT;
}

class CallbackCollection // todo: add sort by dependencies
{
public:
    struct CBType
    {
        sol::environment env;
        sol::function fn;
        bool needsOldState;
        CBType(sol::environment _env, sol::function _fn, bool _needsOldState): env(_env), fn(_fn), needsOldState(_needsOldState) {}
    };

    typedef std::vector<CBType> Container;
    typedef Container::iterator Iterator;
    typedef Container::const_iterator CIterator;

    template<typename> struct type_tag {};

    void push(sol::environment &env, sol::function &func, bool needsOldState) { functions.emplace_back(env, func, needsOldState); }
    CIterator begin() const { return functions.begin(); }
    CIterator end() const { return functions.end(); }

    void stop() {_stop = true;}
    bool isStoped() const {return _stop;}
    CIterator stopedAt() const { return lastCalled; }

    template<typename... Args, typename OldData>
    void callWOld(const OldData &oldData, Args&&... args)
    {
        lastCalled = functions.end();
        _stop = false;
        for (CIterator iter = functions.begin(); iter != functions.end(); ++iter)
        {
            if (!_stop)
            {
                if (!iter->needsOldState)
                {
                    iter->fn.call(std::forward<Args>(args)...);
                    continue;
                }
                iter->fn.call(std::forward<Args>(args)..., oldData);
            }
            else
            {
                lastCalled = iter;
                break;
            }
        }
    }

    template<typename... Args>
    void call(type_tag<void>, Args&&... args)
    {
        lastCalled = functions.end();
        _stop = false;
        for (CIterator iter = functions.begin(); iter != functions.end(); ++iter)
        {
            if (!_stop)
                iter->fn.call(std::forward<Args>(args)...);
            else
            {
                lastCalled = iter;
                break;
            }
        }
    }

    template<typename R, typename... Args>
    decltype(auto) call(type_tag<R>, Args&&... args)
    {
        R ret;

        lastCalled = functions.end();
        _stop = false;
        for (CIterator iter = functions.begin(); iter != functions.end(); ++iter)
        {
            if (!_stop)
                ret = iter->fn.call(std::forward<Args>(args)...);
            else
            {
                lastCalled = iter;
                break;
            }
        }
        return ret;
    }

private:
    Container functions;
    CIterator lastCalled;
    bool _stop = false;
};

class EventController
{
public:
    static void Init(LuaState &lua);
public:
    explicit EventController(LuaState *luaCtrl);
    typedef std::unordered_map<int, CallbackCollection> Container;

    void registerEvent(int event, sol::environment &env, sol::function& func, bool needsOldState);
    CallbackCollection& GetEvents(Event event);
    Event createEvent();
    void raiseEvent(Event id, sol::table data, const std::string &moduleName = "");
    void stop(int event);

    template<Event event, bool canProvideOldState = false, typename R = void, typename... Args>
    R Call(Args&&... args)
    {
        if (canProvideOldState)
            events.at(event).callWOld(std::forward<Args>(args)...);
        else
            return events.at(event).call(CallbackCollection::type_tag<R>{}, std::forward<Args>(args)...);
    }

    template<Event event, bool canProvideOldState = false, typename R = void>
    R Call()
    {
        return events.at(event).call(CallbackCollection::type_tag<R>{});
    }
private:
    Container events;
    Event lastEvent = CoreEvent::LAST;
    LuaState *luaCtrl;
};
