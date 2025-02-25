//
// Created by Arthur Motelevicz on 05/12/23.
//

#ifndef MAGIADEBUGGER_H
#define MAGIADEBUGGER_H

#include <memory>
#include <unordered_set>

namespace sol{
    class state;
}

namespace mg{

    class MagiaDebugger {
    public:
        using PauseCallback = std::function<void(void* L, void* ar,  int line, const std::string& functionName)>;
        enum class DebuggerState {
            Coding,
            Running,
            Debugging,
            Paused,
            Step_over,
            Stopping,
        };

        inline static std::unordered_set<int> breakpoints;
        static void setHook(const std::shared_ptr<sol::state>& sol);
        static void appendBreakpoint(int line);
        static void removeBreakpoint(int line);
        inline static DebuggerState state = DebuggerState::Coding;

        static void setPauseCallback(const PauseCallback& cb);

        inline static PauseCallback pauseCallback{nullptr};
    };

}
#endif //MAGIADEBUGGER_H
