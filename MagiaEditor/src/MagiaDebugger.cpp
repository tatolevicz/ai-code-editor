//
// Created by Arthur Motelevicz on 05/12/23.
//

#include "MagiaDebugger.h"
#include "lua.hpp"
#include <iostream>
#include <sol/sol.hpp>
#include <thread>

namespace mg{

    void luaDebugHook(lua_State *L, lua_Debug *ar) {
        lua_getinfo(L, "nSl", ar);
        int currentLine = ar->currentline;
        std::string currentFunction = ar->name ? ar->name : "global";
//        std::cout << "Current Line: " << currentLine << std::endl;
//        std::cout << "Current function: " << currentFunction << std::endl;

        if(MagiaDebugger::state == MagiaDebugger::DebuggerState::Step_over){
            MagiaDebugger::state = MagiaDebugger::DebuggerState::Paused;
            if(MagiaDebugger::pauseCallback)
                MagiaDebugger::pauseCallback(L, ar, currentLine - 1, currentFunction);
        }

        while(MagiaDebugger::state == MagiaDebugger::DebuggerState::Paused &&
                MagiaDebugger::state != MagiaDebugger::DebuggerState::Stopping) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        bool isBreakPoint = MagiaDebugger::breakpoints.find(ar->currentline) != MagiaDebugger::breakpoints.end();

        if(isBreakPoint && MagiaDebugger::state == MagiaDebugger::DebuggerState::Debugging){
            MagiaDebugger::state = MagiaDebugger::DebuggerState::Paused;
            if(MagiaDebugger::pauseCallback)
                MagiaDebugger::pauseCallback(L, ar, currentLine - 1, currentFunction);
        }

        while(MagiaDebugger::state == MagiaDebugger::DebuggerState::Paused &&
              MagiaDebugger::state != MagiaDebugger::DebuggerState::Stopping) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (MagiaDebugger::state == MagiaDebugger::DebuggerState::Stopping) {
            luaL_error(L, "Script interrupted!");
        }
    }

    void MagiaDebugger::setHook(const std::shared_ptr<sol::state>& sol){
        lua_sethook(sol->lua_state(),luaDebugHook, LUA_MASKLINE, 0);
//        lua_sethook(sol->lua_state(), luaDebugHook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 0);
    }

    void MagiaDebugger::appendBreakpoint(int line){
        breakpoints.insert(line + 1);
    }

    void MagiaDebugger::removeBreakpoint(int line){
        breakpoints.erase(line + 1);
    }

    void MagiaDebugger::setPauseCallback(const PauseCallback& cb){
        pauseCallback = cb;
    }

}
