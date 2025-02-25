//
// Created by Arthur Motelevicz on 05/12/23.
//

#include "MagiaEditor.h"
#include "MgStyles.h"

#include "SciLexer.h"
#include "ILexer.h"
#include "Lexilla.h"
#include <sol/sol.hpp>
#include <QTimer>
#include <regex>
#include "MagiaDebugger.h"
#include "lua.hpp"

namespace mg{
    MagiaEditor::MagiaEditor(QWidget *parent):
    ScintillaEdit(parent){}

    void MagiaEditor::setup(){
        auto lexNum = GetLexerCount();
        qDebug() << "Num of lexers: " << lexNum;

        char name[20];
        for(int i = 0; i < lexNum; i++){
            GetLexerName(i,name, 20);
            qDebug() << "Lexer available: " << name;
        }

        auto lex = CreateLexer("lua");

        assert(lex != nullptr);

        this->resize(QSize(this->width(), this->height()));
        this->setWrapMode(SC_WRAP_WORD);

        this->setILexer((sptr_t)(void*)lex);

        mg::styles::editor::setDefaultStyle(this);
        mg::styles::lua::setDefaultStyle(this);

        this->autoCSetMaxWidth(50);
        this->autoCSetMaxHeight(10);

        // Conectar sinais e slots para eventos de digitação
        connect(this, &ScintillaEdit::charAdded, this, &MagiaEditor::onCharAdded);
        connect(this, &ScintillaEdit::modified, this, &MagiaEditor::scriptModified);
        connect(this, &ScintillaEdit::marginClicked, this, &MagiaEditor::onMarginClicked);
        connect(this, &ScintillaEdit::dwellStart, this, &MagiaEditor::idleMouseStart);
        connect(this, &ScintillaEdit::dwellEnd, this, &MagiaEditor::idleMouseEnd);

        //lua setup
        _lua = std::make_shared<sol::state>();
        _lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);

        MagiaDebugger::setHook(_lua);

        // syntax timer setup
        _syntaxTimer = new QTimer(this);
        _syntaxTimer->setInterval(800); // 1000 ms = 1 segundo
        connect(_syntaxTimer, &QTimer::timeout, this, &MagiaEditor::syntaxTimerTimeout);

        this->setMouseDwellTime(500);

        // Define uma função de print personalizada
        _lua->set_function("print", [this](sol::variadic_args va, sol::this_state ts) {
            lua_State* L = ts;  // Obter o lua_State atual
            std::string output;
            for (auto v : va) {
                sol::object arg = v.get<sol::object>();
                arg.push(L);  // Coloca o objeto no topo da pilha Lua
                const char* str = lua_tostring(L, -1);  // Converte o objeto no topo da pilha para string
                if (str) {
                    output += str;
                } else {
                    // Obter o tipo de Lua do objeto
                    const void* pointer = lua_topointer(L, -1);

                    switch (lua_type(L, -1)) {
                        case LUA_TTABLE:
                            output += "table";  // Representação simplificada de uma tabela
                            break;
                        case LUA_TFUNCTION:
                            output += "function";  // Representação simplificada de uma função
                            break;
                        case LUA_TUSERDATA:
                            output += "userdata";  // Representação de userdata
                            break;
                        case LUA_TTHREAD:
                            output += "thread";  // Representação de thread (corrotina)
                            break;
                        default:
                            output += luaL_typename(L, -1);
                            break;
                    }

                    output += " <" + std::to_string(reinterpret_cast<std::ptrdiff_t>(pointer)) + ">";
                }

                lua_pop(L, 1);  // Remove o objeto do topo da pilha
                output += " ";
            }

            if(_printCallback)
                _printCallback(output);
        });


        MagiaDebugger::setPauseCallback([this](void *L, void *ar, int line, const std::string& functionName){

            _isPausedInsideFunction = functionName != "global";
            _lua_state_on_pause = L;
            _debug_state_on_pause = ar;

            QMetaObject::invokeMethod(this, [this,line](){
                if(MagiaDebugger::state == MagiaDebugger::DebuggerState::Paused)
                    send(SCI_MARKERADD, line, styles::Markers::BREAKPOINT_ACHIEVED);

                send(SCI_MARKERADD, line, styles::Markers::BREAKPOINT_ACHIEVED_BACKGROUND);
            });

            emit scriptPaused();
        });
    }

    MagiaEditor::~MagiaEditor(){}

    void MagiaEditor::syntaxTimerTimeout() {
        int length = this->textLength();
        std::string script = this->getText(length).toStdString();
        int errorLine = validateScript(script);
        updateErrorMaker(errorLine);
    }

    void MagiaEditor::scriptModified(Scintilla::ModificationFlags type,
                                    Scintilla::Position position,
                                    Scintilla::Position length,
                                    Scintilla::Position linesAdded,
                                    const QByteArray &text,
                                    Scintilla::Position line,
                                    Scintilla::FoldLevel foldNow,
                                    Scintilla::FoldLevel foldPrev)
    {
        _syntaxTimer->start();
    }

    void MagiaEditor::onMarginClicked(Scintilla::Position position,
                                     Scintilla::KeyMod modifiers,
                                     int margin){

        int lineClicked = send(SCI_LINEFROMPOSITION, position, 0);

        if(margin == styles::Margins::FOLDING) {
            // Obter a linha que foi clicada
            qDebug() << "Margin clicked on line " << lineClicked;
            // Alternar dobradura na linha
            this->send(SCI_TOGGLEFOLD, lineClicked);
            return;
        }

        if(margin == styles::Margins::SYMBOLS){
            if (send(SCI_MARKERGET, lineClicked) & (1 << styles::Markers::BREAKPOINT)) {
                send(SCI_MARKERDELETE, lineClicked, styles::Markers::BREAKPOINT);
                send(SCI_MARKERDELETE, lineClicked, styles::Markers::BREAKPOINT_BACKGROUND);
                MagiaDebugger::removeBreakpoint(lineClicked);
            } else {
                send(SCI_MARKERADD, lineClicked, styles::Markers::BREAKPOINT);
                send(SCI_MARKERADD, lineClicked, styles::Markers::BREAKPOINT_BACKGROUND);
                MagiaDebugger::appendBreakpoint(lineClicked);
            }
        }
    }


    void MagiaEditor::onCharAdded(int ch) {
        // Implementação de lógica de quando mostrar o autocomplete
        if (ch == '(' || ch == ' ') {
            // Exemplo: mostrar autocomplete após '(' ou ' '
            showAutocomplete();
        }
        else if(ch == '\n'){
            onNewLine();
        }
        else {
        }
    }

    void MagiaEditor::onNewLine() {
        //TODO::just testsing -> this can work but need a lot of tests
//    int currentLine = this->send(SCI_LINEFROMPOSITION, this->send(SCI_GETCURRENTPOS));
//    int lineIndentation = 2;//this->send(SCI_GETLINEINDENTATION, currentLine);
//    this->send(SCI_SETLINEINDENTATION, currentLine + 1, lineIndentation);
//    this->send(SCI_GOTOPOS, this->send(SCI_POSITIONFROMLINE, currentLine + 1) + lineIndentation);
    }

    void MagiaEditor::showAutocomplete() {
        // Exemplo de fonte de dados para autocomplete (palavras-chave Lua)
        QStringList luaKeywords = {"function", "end", "if", "then", "else", "for", "while", "do", "repeat", "until"};
        QString wordList = luaKeywords.join(" ");

        // Exibir lista de autocomplete
        this->autoCShow(0,wordList.toUtf8().data());
    }

    void MagiaEditor::updateErrorMaker(int errorLine) {

        // Primeiro, remova todos os marcadores de erro existentes
        this->markerDeleteAll(styles::Markers::ERROR);

        // Se houver um erro, adicione o marcador de erro na linha correspondente
        if (errorLine != -1) {
            this->markerAdd(errorLine-1, 1);
        }
    }

    int MagiaEditor::validateScript(const std::string& script) {

        sol::load_result result = _lua->load(script);
//        auto result = _lua->script(script);
        if (!result.valid()) {
            sol::error err = result;
            std::string errorMsg = err.what();

            // Extrair e retornar o número da linha do erro (implemente esta função)
            return extractErrorLine(errorMsg);
        }

        return -1; // Retorna -1 se a sintaxe estiver correta
    }

    void MagiaEditor::executeScript(const std::string& script, const ScriptExecutionCallback& cb) {

        _scriptWorker = std::thread([this, script, cb](){
            try {
                _lua->script(script);
                cb(true, "");
            }
            catch(const sol::error& err){
                std::cerr << "Error: " << err.what();
                cb(false, err.what());
            }
        });

        _scriptWorker.detach();
    }

    int MagiaEditor::extractErrorLine(const std::string& errorMsg) {
        _currentError = errorMsg;

        std::regex standard(R"(\]:([0-9]+):)"); // Padrão para capturar o número da linha
        std::smatch results;

        if (std::regex_search(errorMsg, results, standard) && results.size() > 1) {
            return std::stoi(results[1].str()); // Converte o número da linha para inteiro
        }

        _currentError = "";
        return -1; // Retorna -1 se não encontrar o número da linha
    }


    void MagiaEditor::idleMouseStart(int x, int y){
        int pos = positionFromPoint(x, y);
        int line = lineFromPosition(pos);

        //show error tooltip logic
        showErrorIfAny(x, line , pos);

        if(MagiaDebugger::state == MagiaDebugger::DebuggerState::Paused)
            showVariableValueIfAny(pos);
    }

    void MagiaEditor::idleMouseEnd(int x, int y){
        callTipCancel();
    }

    bool MagiaEditor::showErrorIfAny(int x, int line, int pos){
        //show error tooltip logic
        if(x >= styles::MarginsSize::NUMBERS &&
            x <= styles::MarginsSize::NUMBERS + styles::MarginsSize::SYMBOLS ) {
            int markerMask = markerGet(line);
            int errorMask = (1 << styles::Markers::ERROR);
            if (markerMask & errorMask) {
                callTipShow(pos, _currentError.c_str());
            }
            return true;
        }

        return false;
    }

    void MagiaEditor::showVariableValueIfAny(int pos) {

        int startPos = wordStartPosition(pos, true);
        int endPos = wordEndPosition(pos, true);

        auto wordArray = textRange(startPos, endPos);

        // Agora você tem a palavra sob o cursor do mouse
        std::string wordUnderCursor = wordArray.toStdString();

        if(wordUnderCursor.empty())
            return;

        if(_isPausedInsideFunction) {
            auto *state = (lua_State *) _lua_state_on_pause;
            auto *debug = (lua_Debug *) _debug_state_on_pause;
            const char *varName;
            int index = 1;
            std::string varValue;
            while ((varName = lua_getlocal(state, debug, index)) != NULL) {
                if (strcmp(varName, wordUnderCursor.c_str()) == 0) {
                    varValue = lua_tostring(state, -1);
                    lua_pop(state, 1);  // Remover a variável da pilha
                    break;
                }
                lua_pop(state, 1);  // Remover a variável da pilha
                index++;
            }

            if (!varValue.empty()) {
                QMetaObject::invokeMethod(this,
                  [this, pos, varValue]() {
                      callTipShow(pos, varValue.c_str());
                  },
                  Qt::QueuedConnection);
            }

            return;
        }

        sol::object luaVar = (*_lua)[wordUnderCursor];
        if (luaVar.valid()) {
            bool shouldShow = false;
            std::string varValue;
            if (luaVar.get_type() == sol::type::string) {
                varValue = luaVar.as<std::string>();
                shouldShow = true;
            } else if (luaVar.get_type() == sol::type::number) {
                varValue = std::to_string(luaVar.as<double>());
                shouldShow = true;
            }
            else if (luaVar.get_type() == sol::type::boolean) {
                varValue = std::to_string(luaVar.as<bool>());
                shouldShow = true;
            }

            if(shouldShow) {
                QMetaObject::invokeMethod(this,
                  [this, pos, varValue]() {
                      callTipShow(pos, varValue.c_str());
                  },
                  Qt::QueuedConnection);
            }
        }
    }

    void MagiaEditor::setPrintCallback(const PrintCallback &cb) {
        _printCallback = cb;
    }


    void MagiaEditor::execute(){
        if(MagiaDebugger::state != MagiaDebugger::DebuggerState::Coding)
            return;

        MagiaDebugger::state = MagiaDebugger::DebuggerState::Running;

        internalExecute();
    }


    void MagiaEditor::executeDebug(){

        if(MagiaDebugger::state != MagiaDebugger::DebuggerState::Coding)
            return;

        MagiaDebugger::state = MagiaDebugger::DebuggerState::Debugging;
        internalExecute();
    }

    void MagiaEditor::stopExecution(){

        this->markerDeleteAll(styles::Markers::BREAKPOINT_ACHIEVED);
        this->markerDeleteAll(styles::Markers::BREAKPOINT_ACHIEVED_BACKGROUND);

        if(MagiaDebugger::state == MagiaDebugger::DebuggerState::Coding ||
            MagiaDebugger::state == MagiaDebugger::DebuggerState::Stopping)
            return;

        MagiaDebugger::state = MagiaDebugger::DebuggerState::Stopping;
        _lua->stack_clear();
        _lua->collect_garbage();
    }

    void MagiaEditor::stepExecution() {
        if(MagiaDebugger::state != MagiaDebugger::DebuggerState::Paused)
            return;

        this->markerDeleteAll(styles::Markers::BREAKPOINT_ACHIEVED);
        this->markerDeleteAll(styles::Markers::BREAKPOINT_ACHIEVED_BACKGROUND);

        MagiaDebugger::state = MagiaDebugger::DebuggerState::Step_over;
    }

    void MagiaEditor::continueExecution(){
        this->markerDeleteAll(styles::Markers::BREAKPOINT_ACHIEVED);
        this->markerDeleteAll(styles::Markers::BREAKPOINT_ACHIEVED_BACKGROUND);

        if(MagiaDebugger::state == MagiaDebugger::DebuggerState::Paused){
            MagiaDebugger::state = MagiaDebugger::DebuggerState::Debugging;
            return;
        }
    }

    void MagiaEditor::internalExecute(){
        emit scriptStarted();
        auto length = this->textLength();
        std::string script = this->getText(length).toStdString();
        executeScript(script,[this](bool success, const std::string& msg){
            if(!success) {
                if(_printCallback)
                    _printCallback(msg);

                MagiaDebugger::state = MagiaDebugger::DebuggerState::Coding;
                emit scriptFinished();
                return;
            }

            _lua->stack_clear();

            if(_printCallback)
                _printCallback("\nScript execution ended!\n");

            MagiaDebugger::state = MagiaDebugger::DebuggerState::Coding;

            emit scriptFinished();
        });
    }


}