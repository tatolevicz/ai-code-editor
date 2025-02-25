//
// Created by Arthur Motelevicz on 25/02/25.
//

#ifndef QWIDGET_LUA_EDITOR_GENERICEDITOR_H
#define QWIDGET_LUA_EDITOR_GENERICEDITOR_H

#include "ScintillaEdit.h"
#include "MgStyles.h"
#include "SciLexer.h"
#include "ILexer.h"
#include "Lexilla.h"
#include <sol/sol.hpp>
#include <QTimer>
#include <regex>
#include "MagiaDebugger.h"
#include "lua.hpp"
#include <QFileInfo>

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
      setupDefaultStyle();

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

      mg::styles::editor::setDefaultStyle(this);
      mg::styles::lua::setDefaultStyle(this);

      this->autoCSetMaxWidth(50);
      this->autoCSetMaxHeight(10);
    }

    void setupDefaultStyle() 
    {
      // Set default style
      styleSetFont(STYLE_DEFAULT, "Courier New");
      styleSetSize(STYLE_DEFAULT, 10);
      styleSetFore(STYLE_DEFAULT, 0x000000);
      styleSetBack(STYLE_DEFAULT, 0xFFFFFF);
      
      // Apply the default style to all
      styleClearAll();
    }

    void setLexerForFile(const QString& filePath) 
    {
      QString extension = QFileInfo(filePath).suffix().toLower();
      
      // Create appropriate lexer based on file extension
      sptr_t lex = 0;
      
      if (extension == "lua") {
        lex = (sptr_t)(void*)CreateLexer("lua");
      }
      else if (extension == "py" || extension == "python") {
        lex = (sptr_t)(void*)CreateLexer("python");
      }
      else if (extension == "cpp" || extension == "h" || extension == "hpp" || extension == "cxx") {
        lex = (sptr_t)(void*)CreateLexer("cpp");
      }
      else if (extension == "js" || extension == "javascript") {
        lex = (sptr_t)(void*)CreateLexer("javascript");
      }
      else if (extension == "html" || extension == "htm") {
        lex = (sptr_t)(void*)CreateLexer("html");
      }
      else if (extension == "css") {
        lex = (sptr_t)(void*)CreateLexer("css");
      }
      else if (extension == "json") {
        lex = (sptr_t)(void*)CreateLexer("json");
      }
      else if (extension == "xml") {
        lex = (sptr_t)(void*)CreateLexer("xml");
      }
      else {
        // Default to a simple text lexer if no specific lexer is available
        lex = (sptr_t)(void*)CreateLexer("null");
      }

      if (lex) {
        setILexer(lex);
        colourise(0, -1);  // Apply syntax highlighting to the entire document
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
      qDebug() << "Mouse Idle start!";
    }

    void idleMouseEnd(int x, int y)
    {
      qDebug() << "Mouse Idle end!";

    }
  };
}
#endif //QWIDGET_LUA_EDITOR_GENERICEDITOR_H
