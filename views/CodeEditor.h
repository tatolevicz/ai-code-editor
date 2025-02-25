//
// Created by Arthur Motelevicz on 24/02/25.
//

#ifndef QWIDGET_LUA_EDITOR_CODEEDITOR_H
#define QWIDGET_LUA_EDITOR_CODEEDITOR_H


#include <QWidget>
#include "MagiaEditor.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QVBoxLayout>
#include <QResizeEvent>
#include "GenericEditor.h"
#include <QFile>
#include <QTextStream>

namespace aic
{
  class CodeEditor : public QWidget {
  Q_OBJECT
  public:
    CodeEditor(QWidget* parent = nullptr): QWidget(parent) {

        _centralWidget = new QWidget(this);  // Widget central
        auto* layout = new QVBoxLayout(_centralWidget);  // Layout vertical
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        _editor = new GenericEditor(_centralWidget);
        _editor->setup();


        //called here in the middle of the spacers to make the icons be in the middle of the toolbar
        setupActions();

        layout->addWidget(_editor);
//
//        _editor->setPrintCallback([this](const std::string& print){
//          QMetaObject::invokeMethod(this, [this, print](){
//            _console->appendPlainText(print.c_str());
//          });
//        });

        updateActions();
    }

    void resizeEvent(QResizeEvent *event) override
    {
      QWidget::resizeEvent(event);
      _centralWidget->resize(event->size());
    }

//  public slots:

    QWidget* getCentralWidget()
    {
      return _centralWidget;
    }

    void openFile(const QString& filePath)
    {
      QFile file(filePath);
      if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();
        _editor->setText(content.toUtf8().constData());
        _editor->setLexerForFile(filePath);  // Set the appropriate lexer based on file extension
        _currentFilePath = filePath;
      }
    }

  private:
    void updateActions() {}

    void setupActions()
    {
      connectActions();
    }

    void connectActions(){}

    GenericEditor* _editor{nullptr};
    QWidget* _centralWidget{nullptr};
    QString _currentFilePath;
  };
}
#endif //QWIDGET_LUA_EDITOR_CODEEDITOR_H
