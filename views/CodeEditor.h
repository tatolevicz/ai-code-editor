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
#include "MagiaTheme.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

namespace aic
{
  class CodeEditor : public QWidget {
  Q_OBJECT
  public:
    CodeEditor(QWidget* parent = nullptr): QWidget(parent) {
        setupUI();
    }

    void resizeEvent(QResizeEvent *event) override
    {
      QWidget::resizeEvent(event);
      _centralWidget->resize(event->size());
    }

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
        
        // Update file info in the header
        QFileInfo fileInfo(filePath);
        _fileNameLabel->setText(fileInfo.fileName());
        _filePathLabel->setText(fileInfo.absolutePath());
      }
    }

  private:
    void setupUI() {
      // Set background color for the main widget
      setStyleSheet(QString("CodeEditor { background-color: #%1; }")
                   .arg(mg::theme::Colors::BACKGROUND, 6, 16, QChar('0')));
      
      _centralWidget = new QWidget(this);
      auto mainLayout = new QVBoxLayout(_centralWidget);
      mainLayout->setContentsMargins(0, 0, 0, 0);
      mainLayout->setSpacing(0);
      
      // Create header with file info
      auto headerWidget = new QWidget(_centralWidget);
      headerWidget->setStyleSheet(QString("QWidget { background-color: #%1; }")
                                .arg(mg::theme::Colors::SIDEBAR_BG, 6, 16, QChar('0')));
      
      auto headerLayout = new QHBoxLayout(headerWidget);
      headerLayout->setContentsMargins(12, 8, 12, 8);
      
      // File info (name and path)
      auto fileInfoWidget = new QWidget(headerWidget);
      auto fileInfoLayout = new QVBoxLayout(fileInfoWidget);
      fileInfoLayout->setContentsMargins(0, 0, 0, 0);
      fileInfoLayout->setSpacing(2);
      
      _fileNameLabel = new QLabel(fileInfoWidget);
      _fileNameLabel->setStyleSheet(QString("QLabel { color: #%1; font-size: 13px; font-weight: bold; }")
                                  .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0')));
      
      _filePathLabel = new QLabel(fileInfoWidget);
      _filePathLabel->setStyleSheet(QString("QLabel { color: #%1; font-size: 11px; }")
                                  .arg(mg::theme::Colors::COMMENT, 6, 16, QChar('0')));
      
      fileInfoLayout->addWidget(_fileNameLabel);
      fileInfoLayout->addWidget(_filePathLabel);
      
      headerLayout->addWidget(fileInfoWidget);
      headerLayout->addStretch();
      
      // Action buttons
      auto runButton = new QPushButton("▶ Run", headerWidget);
      runButton->setStyleSheet(
          QString("QPushButton { background-color: #%1; color: #%2; border: none; border-radius: 4px; padding: 5px 15px; font-weight: bold; }"
                  "QPushButton:hover { background-color: #%3; }")
                  .arg(mg::theme::Colors::PRIMARY, 6, 16, QChar('0'))
                  .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                  .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0'))
      );
      
      auto saveButton = new QPushButton("💾 Save", headerWidget);
      saveButton->setStyleSheet(
          QString("QPushButton { background-color: transparent; color: #%1; border: 1px solid #%2; border-radius: 4px; padding: 5px 15px; }"
                  "QPushButton:hover { background-color: #%3; }")
                  .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                  .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0'))
                  .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0'))
      );
      
      headerLayout->addWidget(saveButton);
      headerLayout->addWidget(runButton);
      
      mainLayout->addWidget(headerWidget);
      
      // Add separator line similar to AI chat widget
      QFrame* separator = new QFrame(_centralWidget);
      separator->setFrameShape(QFrame::HLine);
      separator->setFrameShadow(QFrame::Sunken);
      separator->setStyleSheet(QString("QFrame { background-color: #%1; max-height: 1px; }")
                             .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));
      mainLayout->addWidget(separator);
      
      // Create editor
      _editor = new GenericEditor(_centralWidget);
      _editor->setup();
      mainLayout->addWidget(_editor);
      
      // Connect actions
      connectActions();
      
      _centralWidget->setLayout(mainLayout);
    }
    
    void updateActions() {}

    void setupActions()
    {
      connectActions();
    }

    void connectActions(){}

    GenericEditor* _editor{nullptr};
    QWidget* _centralWidget{nullptr};
    QString _currentFilePath;
    QLabel* _fileNameLabel{nullptr};
    QLabel* _filePathLabel{nullptr};
  };
}
#endif //QWIDGET_LUA_EDITOR_CODEEDITOR_H
