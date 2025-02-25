#ifndef MYHTMLSTYLE_H
#define MYHTMLSTYLE_H

#include "ScintillaEdit.h"
#include "SciLexer.h"
#include <QFontDatabase>

namespace mg {
    namespace styles{

        struct Margins {
            inline static int SYMBOLS = 1;
            inline static int NUMBERS = 0;
            inline static int FOLDING = 2;
        };

        struct MarginsSize {
            inline static int SYMBOLS = 30;
            inline static int NUMBERS = 25;
            inline static int FOLDING = 16;
        };

        struct Markers {
            inline static int ERROR = 1;
            //todo continue adding
            inline static int BREAKPOINT = 2;
            inline static int BREAKPOINT_BACKGROUND = 3;
            inline static int BREAKPOINT_ACHIEVED = 4;
            inline static int BREAKPOINT_ACHIEVED_BACKGROUND = 5;
            inline static int OTHERS = 6;
        };

        struct LuaEditorColors {
            inline static int PRE_PROC = 0xDDB26D; // Funções e métodos
            inline static int IDENTIFIER = 0xE7BEB4; // Identificadores
            inline static int KEYWORD = 0xB46981; // Palavras-chave (preto para destaque)
            inline static int COMMENT = 0x79554C; // Comentários
            inline static int DARK_BACKGROUND = 0x2C1E1B; // Cor de fundo do editor
            inline static int BACKGROUND = 0x3B2824; // Cor de fundo do editor
            inline static int LINE_ACIVE = 0x422E29;
            inline static int STRING = 0xCBB129; // Strings
            inline static int KNOWN_FUNCTION = 0x6193E9; // Funções e métodos
            inline static int ERRORS = 0x4B4BDB;
            inline static int NUMBER = 0x40BDBD; // Números
            inline static int OPEN_STRING = 0xE7BEB4; // Strings
            inline static int OPERATOR = 0xD98E6C; // Operadores
            inline static int CONSTANT = 0xAAB362; // Constantes (nil, true, false)
            inline static int LINE_PAUSED = 0x2D4B4B; // Breakpoint achieved
        };

        struct editor {
            inline static void setupFolding(ScintillaEdit *editor) {
                // Configurar a margin para dobradura (número da margin, geralmente 2 para dobradura)
                int marginWidth = MarginsSize::FOLDING; // Largura da margin em pixels

                editor->send(SCI_SETMARGINWIDTHN, Margins::FOLDING, marginWidth);
                editor->send(SCI_SETMARGINMASKN, Margins::FOLDING, SC_MASK_FOLDERS);
                editor->send(SCI_SETMARGINTYPEN, Margins::FOLDING, SC_MARGIN_SYMBOL);
                editor->send(SCI_SETMARGINSENSITIVEN, Margins::FOLDING, true);

                editor->send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
                editor->send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
                editor->send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
                editor->send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
                editor->send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
                editor->send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
                editor->send(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);

                editor->send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDER, LuaEditorColors::LINE_ACIVE);
                editor->send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDEROPEN, LuaEditorColors::LINE_ACIVE);
                editor->send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB, LuaEditorColors::LINE_ACIVE);
                editor->send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL, LuaEditorColors::LINE_ACIVE);
                editor->send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDEREND, LuaEditorColors::LINE_ACIVE);
                editor->send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDEROPENMID, LuaEditorColors::LINE_ACIVE);
                editor->send(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL, LuaEditorColors::LINE_ACIVE);

                // Definir a cor de frente (foreground) do marcador de dobradura
                editor->send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDER, LuaEditorColors::COMMENT);
                editor->send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDEROPEN, LuaEditorColors::COMMENT);
                editor->send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDERSUB, LuaEditorColors::COMMENT);
                editor->send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDERTAIL, LuaEditorColors::COMMENT);
                editor->send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDEREND, LuaEditorColors::COMMENT);
                editor->send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDEROPENMID, LuaEditorColors::COMMENT);
                editor->send(SCI_MARKERSETFORE, SC_MARKNUM_FOLDERMIDTAIL, LuaEditorColors::COMMENT);

                editor->setFoldMarginColour(true, LuaEditorColors::DARK_BACKGROUND);
                editor->setFoldMarginHiColour(true, LuaEditorColors::DARK_BACKGROUND);

                // Habilitar dobradura
                editor->send(SCI_SETPROPERTY, (uptr_t) "fold", (sptr_t) "1");
                editor->send(SCI_SETPROPERTY, (uptr_t) "fold.compact", (sptr_t) "1");

            }

            inline static void setupCallTips(ScintillaEdit *editor) {
                editor->callTipSetBack(LuaEditorColors::BACKGROUND);
                editor->callTipSetFore(LuaEditorColors::COMMENT);
            }

            inline static void setupMarkers(ScintillaEdit *editor) {

                //error
                editor->markerDefine(Markers::ERROR, SC_MARK_CIRCLE);
                editor->markerSetFore(Markers::ERROR, LuaEditorColors::ERRORS);
                editor->markerSetBack(Markers::ERROR, LuaEditorColors::ERRORS);

                //breakpoint
                editor->markerDefine(Markers::BREAKPOINT, SC_MARK_CIRCLE);
                editor->markerSetFore(Markers::BREAKPOINT, LuaEditorColors::PRE_PROC);
                editor->markerSetBack(Markers::BREAKPOINT, LuaEditorColors::PRE_PROC);

                editor->markerDefine(Markers::BREAKPOINT_BACKGROUND, SC_MARK_BACKGROUND);
                editor->markerSetFore(Markers::BREAKPOINT_BACKGROUND, LuaEditorColors::LINE_ACIVE);
                editor->markerSetBack(Markers::BREAKPOINT_BACKGROUND, LuaEditorColors::LINE_ACIVE);

                editor->markerDefine(Markers::BREAKPOINT_ACHIEVED, SC_MARK_CIRCLE);
                editor->markerSetFore(Markers::BREAKPOINT_ACHIEVED, LuaEditorColors::NUMBER);
                editor->markerSetBack(Markers::BREAKPOINT_ACHIEVED, LuaEditorColors::NUMBER);

                editor->markerDefine(Markers::BREAKPOINT_ACHIEVED_BACKGROUND, SC_MARK_BACKGROUND);
                editor->markerSetFore(Markers::BREAKPOINT_ACHIEVED_BACKGROUND, LuaEditorColors::LINE_PAUSED);
                editor->markerSetBack(Markers::BREAKPOINT_ACHIEVED_BACKGROUND, LuaEditorColors::LINE_PAUSED);


                editor->setMarginMaskN(Margins::SYMBOLS, 1 << Markers::ERROR | 1 << Markers::BREAKPOINT | 1 << Markers::BREAKPOINT_ACHIEVED); // Permite o marcador de error na margem 1
            }


            inline static void setDefaultStyle(ScintillaEdit *editor) {
                editor->styleSetBack(STYLE_DEFAULT, LuaEditorColors::DARK_BACKGROUND);
                editor->styleSetFont(STYLE_DEFAULT, "Courier New");
                editor->styleSetSize(STYLE_DEFAULT, 16);
                editor->styleClearAll(); //this copies the defualt style to all the others

                editor->send(SCI_SETTABWIDTH, 2);
                editor->send(SCI_SETUSETABS, 1); // Use 1 para usar tabs reais


                //selection
                editor->setSelBack(true, LuaEditorColors::LINE_ACIVE);
                editor->setSelFore(true, LuaEditorColors::IDENTIFIER);

                // Habilita a visualização das guias de indentação
//            editor->send(SCI_SETINDENTATIONGUIDES, SC_IV_LOOKBOTH);
                // Ativa a autoindentação
//            editor->send(SCI_SETAUTOINDENT, 1);

                editor->setCaretFore(LuaEditorColors::IDENTIFIER);
                editor->setCaretLineBack(LuaEditorColors::LINE_ACIVE);  // Define a cor de fundo para o destaque
                editor->setCaretLineFrame(1);

                editor->styleSetBack(STYLE_LINENUMBER, LuaEditorColors::BACKGROUND); // Cinza para fundo
                editor->styleSetFore(STYLE_LINENUMBER, LuaEditorColors::COMMENT); // Branco para texto
                editor->styleSetSize(STYLE_LINENUMBER, 12);

                editor->setMarginTypeN(Margins::NUMBERS, SC_MARGIN_NUMBER);
                editor->setMarginWidthN(Margins::NUMBERS, MarginsSize::NUMBERS);
                editor->setMarginMaskN(Margins::NUMBERS, 0); // nao permite a renderizacao de marcadores na margem 1

                editor->setMarginWidthN(Margins::SYMBOLS,  MarginsSize::SYMBOLS);
                editor->setMarginTypeN(Margins::SYMBOLS, SC_MARGIN_COLOUR);
                editor->setMarginBackN(Margins::SYMBOLS, LuaEditorColors::BACKGROUND);
                editor->setMarginSensitiveN(Margins::SYMBOLS, true);

                setupFolding(editor);
                setupCallTips(editor);
                setupMarkers(editor);
            }
        };


        struct lua {
            inline static void setDefaultStyle(ScintillaEdit *editor) {

                QFontDatabase::addApplicationFont(":/resources/fonts/Courier-New.ttf");
                QFontDatabase::addApplicationFont(":/resources/fonts/Courier-New-Bold.ttf");

                editor->styleSetFore(SCE_LUA_DEFAULT, LuaEditorColors::IDENTIFIER);
                editor->styleSetBold(SCE_LUA_DEFAULT, false);

                editor->styleSetFore(SCE_LUA_COMMENT, LuaEditorColors::COMMENT);
                editor->styleSetBold(SCE_LUA_COMMENT, false);

                editor->styleSetFore(SCE_LUA_COMMENTLINE, LuaEditorColors::COMMENT);
                editor->styleSetBold(SCE_LUA_COMMENTLINE, false);

                editor->styleSetFore(SCE_LUA_NUMBER, LuaEditorColors::NUMBER);
                editor->styleSetBold(SCE_LUA_NUMBER, false);

                editor->styleSetFore(SCE_LUA_WORD, LuaEditorColors::KEYWORD);
                editor->styleSetBold(SCE_LUA_WORD, false);

                editor->styleSetFore(SCE_LUA_STRING, LuaEditorColors::STRING);
                editor->styleSetBold(SCE_LUA_STRING, false);

                editor->styleSetFore(SCE_LUA_CHARACTER, LuaEditorColors::STRING);
                editor->styleSetBold(SCE_LUA_CHARACTER, false);

                editor->styleSetFore(SCE_LUA_LITERALSTRING, LuaEditorColors::STRING);
                editor->styleSetBold(SCE_LUA_LITERALSTRING, false);

                editor->styleSetFore(SCE_LUA_PREPROCESSOR, LuaEditorColors::PRE_PROC);
                editor->styleSetBold(SCE_LUA_PREPROCESSOR, false);

                editor->styleSetFore(SCE_LUA_OPERATOR, LuaEditorColors::OPERATOR);
                editor->styleSetBold(SCE_LUA_OPERATOR, false);

                editor->styleSetFore(SCE_LUA_IDENTIFIER, LuaEditorColors::IDENTIFIER);
                editor->styleSetBold(SCE_LUA_IDENTIFIER, false);

                editor->styleSetFore(SCE_LUA_STRINGEOL, LuaEditorColors::OPEN_STRING);
                editor->styleSetBold(SCE_LUA_STRINGEOL, false);

                editor->styleSetFore(SCE_LUA_WORD2, LuaEditorColors::KNOWN_FUNCTION);
                editor->styleSetBold(SCE_LUA_WORD2, false);

                editor->styleSetFore(SCE_LUA_WORD3, LuaEditorColors::CONSTANT);
                editor->styleSetBold(SCE_LUA_WORD3, false);

                editor->styleSetFore(SCE_LUA_WORD4, LuaEditorColors::ERRORS);
                editor->styleSetBold(SCE_LUA_WORD4, false);

//           editor->styleSetFore(SCE_LUA_WORD5, Colors::MAROON);
//           editor->styleSetBold(SCE_LUA_WORD5, true);
//
//           editor->styleSetFore(SCE_LUA_WORD6, Colors::CORAL);
//           editor->styleSetBold(SCE_LUA_WORD6, true);
//
//           editor->styleSetFore(SCE_LUA_WORD7, Colors::KHAKI);
//           editor->styleSetBold(SCE_LUA_WORD7, true);

//           editor->styleSetFore(SCE_LUA_WORD8, Colors::VIOLET);
//           editor->styleSetBold(SCE_LUA_WORD8, true);
//
                editor->styleSetFore(SCE_LUA_LABEL, LuaEditorColors::PRE_PROC);
                editor->styleSetBold(SCE_LUA_LABEL, false);

                //SCE_LUA_WORD

                editor->setKeyWords(0, "break do else elseif end for function goto if in local repeat return then until while");

                //SCE_LUA_WORD2
                editor->setKeyWords(1, "print");//todo load here know functions

                //SCE_LUA_WORD3
                editor->setKeyWords(2, "false true and or not");

                //SCE_LUA_WORD4
                editor->setKeyWords(3, "nil");
            }
        };

    }
}
#endif // MYHTMLSTYLE_H
