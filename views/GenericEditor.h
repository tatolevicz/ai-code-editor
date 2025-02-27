//
// Created by Arthur Motelevicz on 25/02/25.
//

#ifndef QWIDGET_LUA_EDITOR_GENERICEDITOR_H
#define QWIDGET_LUA_EDITOR_GENERICEDITOR_H

#include "ScintillaEdit.h"
#include "MgStyles.h"
#include "MagiaTheme.h"
#include "SciLexer.h"
#include "ILexer.h"
#include "Lexilla.h"
#include <sol/sol.hpp>
#include <QTimer>
#include <regex>
#include "MagiaDebugger.h"
#include "lua.hpp"
#include <QFileInfo>
#include <QFontDatabase>

namespace aic
{

  class GenericEditor : public ScintillaEdit
  {
  Q_OBJECT

  public:
    using ScriptExecutionCallback = std::function<void(bool, const std::string &msg)>;

    GenericEditor(QWidget *parent = 0): ScintillaEdit(parent){}

    void setup()
    {
      this->resize(QSize(this->width(), this->height()));
      this->setWrapMode(SC_WRAP_WORD);
      setupModernTheme();

      auto lexNum = GetLexerCount();
      qDebug() << "Num of lexers: " << lexNum;

      char name[20];
      for(int i = 0; i < lexNum; i++){
        GetLexerName(i,name, 20);
        qDebug() << "Lexer available: " << name;
      }

      // Conectar sinais e slots para eventos de digitação
      connect(this, &ScintillaEdit::charAdded, this, &GenericEditor::onCharAdded);
      connect(this, &ScintillaEdit::modified, this, &GenericEditor::scriptModified);
      connect(this, &ScintillaEdit::marginClicked, this, &GenericEditor::onMarginClicked);
      connect(this, &ScintillaEdit::dwellStart, this, &GenericEditor::idleMouseStart);
      connect(this, &ScintillaEdit::dwellEnd, this, &GenericEditor::idleMouseEnd);

      this->setMouseDwellTime(500);

      setupSyntaxHighlighting();

      this->autoCSetMaxWidth(50);
      this->autoCSetMaxHeight(10);
    }

    void setupModernTheme() 
    {
      // Try to load JetBrains Mono font
      int fontId = QFontDatabase::addApplicationFont(":/resources/fonts/JetBrainsMono-Regular.ttf");
      QString fontFamily = "JetBrains Mono";
      if (fontId == -1) {
        // Fallback to a system monospace font if JetBrains Mono is not available
        fontFamily = "Courier New";
      }
      
      // Set editor background color to the new DARKEST background color for code area
      setStyleSheet(QString("QsciScintilla { background-color: #%1; }")
                   .arg(mg::theme::Colors::CODE_BG, 6, 16, QChar('0')));

      // Basic editor settings - using direct Scintilla messages
      // Show line numbers in margin 0
      send(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
      send(SCI_SETMARGINWIDTHN, 0, 40);
      
      // Indentation settings
      send(SCI_SETUSETABS, false);
      send(SCI_SETINDENT, 4);
      send(SCI_SETTABWIDTH, 4);
      send(SCI_SETINDENTATIONGUIDES, SC_IV_LOOKBOTH);
      
      // Set default style
      send(SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<sptr_t>(fontFamily.toUtf8().constData()));
      send(SCI_STYLESETSIZE, STYLE_DEFAULT, 11);
      send(SCI_STYLESETFORE, STYLE_DEFAULT, mg::theme::ScintillaColors::DEFAULT_TEXT);
      send(SCI_STYLESETBACK, STYLE_DEFAULT, mg::theme::ScintillaColors::CODE_BG); // Usar a nova cor mais escura

      // Apply to all styles
      send(SCI_STYLECLEARALL);

      // Line numbers style
      send(SCI_STYLESETFORE, STYLE_LINENUMBER, mg::theme::ScintillaColors::LINE_NUMBERS);
      send(SCI_STYLESETBACK, STYLE_LINENUMBER, mg::theme::ScintillaColors::CODE_BG); // Usar a nova cor mais escura aqui também

      // Set selection colors
      send(SCI_SETSELBACK, true, mg::theme::ScintillaColors::SELECTION_BG);
      send(SCI_SETSELFORE, true, mg::theme::ScintillaColors::SELECTION_FG);

      // Set caret style
      send(SCI_SETCARETFORE, mg::theme::ScintillaColors::DEFAULT_TEXT);
      send(SCI_SETCARETWIDTH, 2);

      // Set caret line color
      send(SCI_SETCARETLINEBACK, mg::theme::ScintillaColors::LINE_ACTIVE);
      send(SCI_SETCARETLINEVISIBLE, true);
      send(SCI_SETCARETLINEBACKALPHA, 50);

      // Set brace matching colors
      send(SCI_STYLESETFORE, STYLE_BRACELIGHT, mg::theme::ScintillaColors::BRACE_MATCH);
      send(SCI_STYLESETBOLD, STYLE_BRACELIGHT, true);
      send(SCI_STYLESETFORE, STYLE_BRACEBAD, mg::theme::ScintillaColors::BRACE_MISMATCH);

      // Set margin colors
      send(SCI_SETMARGINBACKN, 1, mg::theme::ScintillaColors::MARGIN_BG);
      
      // Set fold margin
      send(SCI_SETFOLDMARGINCOLOUR, mg::theme::ScintillaColors::MARGIN_BG);
      send(SCI_SETFOLDMARGINHICOLOUR, mg::theme::ScintillaColors::MARGIN_BG);
    }
    
    void setupSyntaxHighlighting()
    {
      // Customize syntax highlighting for common elements
      // These will be applied regardless of the specific lexer
      send(SCI_STYLESETFORE, SCE_LUA_WORD, mg::theme::ScintillaColors::KEYWORD);      // keywords
      send(SCI_STYLESETFORE, SCE_LUA_STRING, mg::theme::ScintillaColors::STRING);     // strings
      send(SCI_STYLESETFORE, SCE_LUA_COMMENTLINE, mg::theme::ScintillaColors::COMMENT); // line comments
      send(SCI_STYLESETFORE, SCE_LUA_COMMENT, mg::theme::ScintillaColors::COMMENT);   // block comments
      send(SCI_STYLESETFORE, SCE_LUA_NUMBER, mg::theme::ScintillaColors::NUMBER);     // numbers
      send(SCI_STYLESETFORE, SCE_LUA_OPERATOR, mg::theme::ScintillaColors::OPERATOR); // operators
      send(SCI_STYLESETFORE, SCE_LUA_IDENTIFIER, mg::theme::ScintillaColors::DEFAULT_TEXT); // identifiers
      send(SCI_STYLESETFORE, SCE_LUA_WORD2, mg::theme::ScintillaColors::FUNCTION);    // known functions
      
      // Similar for C++ lexer
      send(SCI_STYLESETFORE, SCE_C_WORD, mg::theme::ScintillaColors::KEYWORD);        // keywords
      send(SCI_STYLESETFORE, SCE_C_STRING, mg::theme::ScintillaColors::STRING);       // strings
      send(SCI_STYLESETFORE, SCE_C_COMMENTLINE, mg::theme::ScintillaColors::COMMENT); // line comments
      send(SCI_STYLESETFORE, SCE_C_COMMENT, mg::theme::ScintillaColors::COMMENT);     // block comments
      send(SCI_STYLESETFORE, SCE_C_NUMBER, mg::theme::ScintillaColors::NUMBER);       // numbers
      send(SCI_STYLESETFORE, SCE_C_OPERATOR, mg::theme::ScintillaColors::OPERATOR); // operators
      send(SCI_STYLESETFORE, SCE_C_IDENTIFIER, mg::theme::ScintillaColors::DEFAULT_TEXT); // identifiers
      
      // Python
      send(SCI_STYLESETFORE, SCE_P_WORD, mg::theme::ScintillaColors::KEYWORD);        // keywords
      send(SCI_STYLESETFORE, SCE_P_STRING, mg::theme::ScintillaColors::STRING);       // strings
      send(SCI_STYLESETFORE, SCE_P_COMMENTLINE, mg::theme::ScintillaColors::COMMENT); // line comments
      send(SCI_STYLESETFORE, SCE_P_NUMBER, mg::theme::ScintillaColors::NUMBER);       // numbers
      
      // JavaScript
      send(SCI_STYLESETFORE, SCE_HJ_WORD, mg::theme::ScintillaColors::KEYWORD);       // keywords
      send(SCI_STYLESETFORE, SCE_P_STRING, mg::theme::ScintillaColors::STRING);      // strings
      send(SCI_STYLESETFORE, SCE_HJ_COMMENT, mg::theme::ScintillaColors::COMMENT);    // comments
      send(SCI_STYLESETFORE, SCE_HJ_NUMBER, mg::theme::ScintillaColors::NUMBER);      // numbers
    }

    void setLexerForFile(const QString& filePath) 
    {
      QString extension = QFileInfo(filePath).suffix().toLower();
      
      // Create appropriate lexer based on file extension
      sptr_t lex = 0;
      
      if (extension == "lua") {
        lex = reinterpret_cast<sptr_t>((void*)CreateLexer("lua"));
      }
      else if (extension == "py" || extension == "python") {
        lex = reinterpret_cast<sptr_t>((void*)CreateLexer("python"));
      }
      else if (extension == "cpp" || extension == "h" || extension == "hpp" || extension == "cxx") {
        lex = reinterpret_cast<sptr_t>((void*)CreateLexer("cpp"));
      }
      else if (extension == "js" || extension == "javascript") {
        lex = reinterpret_cast<sptr_t>((void*)CreateLexer("javascript"));
      }
      else if (extension == "html" || extension == "htm") {
        lex = reinterpret_cast<sptr_t>((void*)CreateLexer("html"));
      }
      else if (extension == "css") {
        lex = reinterpret_cast<sptr_t>((void*)CreateLexer("css"));
      }
      else if (extension == "json") {
        lex = reinterpret_cast<sptr_t>((void*)CreateLexer("json"));
      }
      
      if (lex) {
        // Set the lexer
        send(SCI_SETILEXER, 0, lex);
        
        // Apply our theme's syntax highlighting
        setupSyntaxHighlighting();
        
        // Set keywords for the lexer
        if (extension == "lua") {
          send(SCI_SETKEYWORDS, 0, reinterpret_cast<sptr_t>("and break do else elseif end for function goto if in local not or repeat return then until while"));
          send(SCI_SETKEYWORDS, 1, reinterpret_cast<sptr_t>("print assert collectgarbage dofile error _G getmetatable ipairs loadfile next pairs pcall print rawequal rawget rawset require select setmetatable tonumber tostring type _VERSION xpcall"));
          send(SCI_SETKEYWORDS, 2, reinterpret_cast<sptr_t>("false nil true"));
        }
        else if (extension == "py" || extension == "python") {
          send(SCI_SETKEYWORDS, 0, reinterpret_cast<sptr_t>("and as assert break class continue def del elif else except exec finally for from global if import in is lambda not or pass print raise return try while with yield"));
        }
        else if (extension == "cpp" || extension == "h" || extension == "hpp" || extension == "cxx") {
          send(SCI_SETKEYWORDS, 0, reinterpret_cast<sptr_t>("alignas alignof and and_eq asm auto bitand bitor bool break case catch char char16_t char32_t class compl const constexpr const_cast continue decltype default delete do double dynamic_cast else enum explicit export extern false final float for friend goto if inline int long mutable namespace new noexcept not not_eq nullptr operator or or_eq override private protected public register reinterpret_cast return short signed sizeof static static_assert static_cast struct switch template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq"));
        }
      }
    }

    virtual ~GenericEditor(){}

  protected:
    void onCharAdded(int ch)
    {
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

    void showAutocomplete() {
      // Exemplo de fonte de dados para autocomplete (palavras-chave Lua)
      QStringList luaKeywords = {"function", "end", "if", "then", "else", "for", "while", "do", "repeat", "until"};
      QString wordList = luaKeywords.join(" ");
      // Exibir lista de autocomplete
      this->autoCShow(0,wordList.toUtf8().data());
    }

    void onNewLine()
    {
      //Todo indentation based on language
      qDebug() << "On new line!";
    }

    void scriptModified(Scintilla::ModificationFlags type, Scintilla::Position position, Scintilla::Position length,
                        Scintilla::Position linesAdded, const QByteArray &text, Scintilla::Position line,
                        Scintilla::FoldLevel foldNow, Scintilla::FoldLevel foldPrev)
    {
      qDebug() << "Editing file:";
    }

    void onMarginClicked(Scintilla::Position position, Scintilla::KeyMod modifiers, int margin)
    {
      //Just examples
      int lineClicked = send(SCI_LINEFROMPOSITION, position, 0);

      if(margin == mg::styles::Margins::FOLDING) {
        // Obter a linha que foi clicada
        qDebug() << "Margin clicked on line " << lineClicked;
        // Alternar dobradura na linha
        this->send(SCI_TOGGLEFOLD, lineClicked);
        return;
      }

      if(margin == mg::styles::Margins::SYMBOLS)
      {
        if (send(SCI_MARKERGET, lineClicked) & (1 << mg::styles::Markers::BREAKPOINT)) {
          send(SCI_MARKERDELETE, lineClicked, mg::styles::Markers::BREAKPOINT);
          send(SCI_MARKERDELETE, lineClicked, mg::styles::Markers::BREAKPOINT_BACKGROUND);
        } else {
          send(SCI_MARKERADD, lineClicked, mg::styles::Markers::BREAKPOINT);
          send(SCI_MARKERADD, lineClicked, mg::styles::Markers::BREAKPOINT_BACKGROUND);
        }
      }
    }

    void idleMouseStart(int x, int y)
    {
//      qDebug() << "Mouse Idle start!";
    }

    void idleMouseEnd(int x, int y)
    {
//      qDebug() << "Mouse Idle end!";

    }
  };
}
#endif //QWIDGET_LUA_EDITOR_GENERICEDITOR_H
