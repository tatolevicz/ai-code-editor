//
// Created by Arthur Motelevicz on 06/12/23.
//
#include "ConsoleOutput.h"

namespace mg {

    ConsoleOutput::ConsoleOutput(QWidget *parent)
    : QPlainTextEdit(parent) {}

    void ConsoleOutput::contextMenuEvent(QContextMenuEvent *event) {
        QMenu *menu = createStandardContextMenu();
        menu->addSeparator();

        auto *clearAction = new QAction("Clear", this);
        connect(clearAction, &QAction::triggered, this, &ConsoleOutput::clear);
        menu->addAction(clearAction);

        menu->exec(event->globalPos());
        delete menu;
    }

}
