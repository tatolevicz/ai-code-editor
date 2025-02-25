//
// Created by Arthur Motelevicz on 05/12/23.
//

#ifndef TESTSCINTILLACMAKE_CONSOLEOUTPUT_H
#define TESTSCINTILLACMAKE_CONSOLEOUTPUT_H

#include <QPlainTextEdit>
#include <QMenu>

namespace mg {
    class ConsoleOutput : public QPlainTextEdit {
    Q_OBJECT
    public:
        ConsoleOutput(QWidget *parent = nullptr);
    protected:
        void contextMenuEvent(QContextMenuEvent *event) override;
    };

}

#endif //TESTSCINTILLACMAKE_CONSOLEOUTPUT_H
