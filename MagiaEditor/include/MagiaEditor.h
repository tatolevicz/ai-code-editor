//
// Created by Arthur Motelevicz on 05/12/23.
//

#ifndef TESTSCINTILLACMAKE_MAGIAEDITOR_H
#define TESTSCINTILLACMAKE_MAGIAEDITOR_H


#include "ScintillaEdit.h"
#include <thread>

namespace sol {
    class state;
}

namespace mg {
    using PrintCallback = std::function<void(const std::string &)>;
    using FinishExecution = std::function<void(bool)>;

    class MagiaEditor : public ScintillaEdit {
    Q_OBJECT

    public:
        using ScriptExecutionCallback = std::function<void(bool, const std::string& msg)>;

        MagiaEditor(QWidget *parent = 0);
        void setup();

        virtual ~MagiaEditor();

        void setPrintCallback(const PrintCallback& cb);

        void execute();
        void executeDebug();
        void stopExecution();
        void stepExecution();
        void continueExecution();

    signals:
        void scriptStarted();
        void scriptPaused();
        void scriptFinished();


    protected:
        void syntaxTimerTimeout();

        void onCharAdded(int ch);

        void onNewLine();

        void scriptModified(Scintilla::ModificationFlags type,
                            Scintilla::Position position,
                            Scintilla::Position length,
                            Scintilla::Position linesAdded,
                            const QByteArray &text,
                            Scintilla::Position line,
                            Scintilla::FoldLevel foldNow,
                            Scintilla::FoldLevel foldPrev);

        void onMarginClicked(Scintilla::Position position,
                             Scintilla::KeyMod modifiers,
                             int margin);

        void idleMouseStart(int x, int y);

        void idleMouseEnd(int x, int y);

        void showAutocomplete();

        void updateErrorMaker(int errorLine);

        int extractErrorLine(const std::string &errorMsg);

        int validateScript(const std::string &script);

        void executeScript(const std::string &script,const ScriptExecutionCallback& cb);

        bool showErrorIfAny(int x, int line, int pos);

        void showVariableValueIfAny(int pos);

        std::shared_ptr<sol::state> _lua{nullptr};

        QTimer *_syntaxTimer{nullptr};
        std::string _currentError;
        std::thread _scriptWorker;
        PrintCallback _printCallback{nullptr};
        void *_lua_state_on_pause{nullptr};
        void *_debug_state_on_pause{nullptr};
        bool _isPausedInsideFunction{false};

    private:
        void internalExecute();
    };
}
#endif //TESTSCINTILLACMAKE_MAGIAEDITOR_H
