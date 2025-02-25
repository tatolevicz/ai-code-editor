//
// Created by Arthur Motelevicz on 06/12/23.
//

#include "MagiaEditorWidget.h"
#include "MagiaEditor.h"
#include "MagiaDebugger.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QVBoxLayout>
#include "ConsoleOutput.h"

namespace mg{

    void MagiaEditorWidget::resizeEvent(QResizeEvent *event){
        QWidget::resizeEvent(event);
        _centralWidget->resize(event->size());
    }

    MagiaEditorWidget::MagiaEditorWidget(QWidget *parent) : QWidget(parent) {

        _centralWidget = new QWidget(this);  // Widget central
        auto* layout = new QVBoxLayout(_centralWidget);  // Layout vertical
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        _editor = new mg::MagiaEditor(_centralWidget);
        _editor->setup();

        _scriptToolBar = new QToolBar("Control Bar", this);
        _scriptToolBar->setStyleSheet("background-color: #24283B;  border-right: 1px solid #24283B;");
        _scriptToolBar->setIconSize(QSize(16, 16));
        _scriptToolBar->setMovable(false);

        auto* bottomWidget = new QWidget(this);
        bottomWidget->setMaximumHeight(150);

        auto* bottomLayout = new QHBoxLayout(this);
        bottomWidget->setLayout(bottomLayout);
        bottomLayout->setContentsMargins(0, 0, 0, 0);
        bottomLayout->setSpacing(0);

        //debug toolbar
        _debugToolBar = new QToolBar("Debug Bar", this);
        _debugToolBar->setOrientation(Qt::Vertical);
        _debugToolBar->setStyleSheet("background-color: #24283B;  border-right: 1px solid #24283B;");
        _debugToolBar->setIconSize(QSize(14, 14));  // Define o tamanho dos ícones
        _debugToolBar->setMovable(false);  // Torna a barra de ferramentas não-movível

        auto* spacerLeft = new QWidget();
        spacerLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _scriptToolBar->addWidget(spacerLeft);

        //called here in the middle of the spacers to make the icons be in the middle of the toolbar
        setupActions();

        auto* spacerRight = new QWidget();
        spacerRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _scriptToolBar->addWidget(spacerRight);

        layout->addWidget(_scriptToolBar);  // Adiciona a barra de ferramentas à janela principal
        layout->addWidget(_editor);

        bottomLayout->addWidget(_debugToolBar);

        _console = new ConsoleOutput(bottomWidget);
        _console->setReadOnly(true);
        _console->setMaximumHeight(150);
        _console->setStyleSheet("background-color: black; color: white; padding: 10px;");
        _console->setFont(QFont("Courier New", 16));
        _console->move(0, this->height() - 150);

        bottomLayout->addWidget(_console);

        layout->addWidget(bottomWidget);

        connect(_editor, &mg::MagiaEditor::scriptFinished, this, &MagiaEditorWidget::onScriptFinished);
        connect(_editor, &mg::MagiaEditor::scriptPaused, this, &MagiaEditorWidget::onScriptPaused);
        connect(_editor, &mg::MagiaEditor::scriptStarted, this, &MagiaEditorWidget::onScriptStarted);

        _editor->setPrintCallback([this](const std::string& print){
            QMetaObject::invokeMethod(this, [this, print](){
                _console->appendPlainText(print.c_str());
            });
        });

        //set the initial state
        MagiaDebugger::state = MagiaDebugger::DebuggerState::Coding;
        updateActions();
    }


    void MagiaEditorWidget::setupActions() {
        _playAction =   new QAction(QIcon(":/resources/images/play_active.svg"), tr("Run Script"), this);
        _debugAction =  new QAction(QIcon(":/resources/images/debug_active.svg"), tr("Debug Script"), this);
        _stopAction =   new QAction(QIcon(":/resources/images/stop_active.svg"), tr("Stop Script"), this);
        _stepOverAction = new QAction(QIcon(":/resources/images/step_over_active.svg"), tr("Step Over"), this);
        _continueAction = new QAction(QIcon(":/resources/images/continue_active.svg"),tr("Continue"), this);

        _scriptToolBar->addAction(_playAction);
        _scriptToolBar->addAction(_debugAction);
        _scriptToolBar->addAction(_stopAction);

        _debugToolBar->addAction(_stepOverAction);
        _debugToolBar->addAction(_continueAction);

        connectActions();
    }

    void MagiaEditorWidget::connectActions() {
        connect(_playAction, &QAction::triggered, this, &MagiaEditorWidget::onPlayClicked);
        connect(_debugAction, &QAction::triggered, this, &MagiaEditorWidget::onDebugClicked);
        connect(_stopAction, &QAction::triggered, this, &MagiaEditorWidget::onStopClicked);
        connect(_stepOverAction, &QAction::triggered, this, &MagiaEditorWidget::onStepOver);
        connect(_continueAction, &QAction::triggered, this, &MagiaEditorWidget::onContinue);
    }

    void MagiaEditorWidget::onPlayClicked() {
        _editor->execute();
        updateActions();
    }

    void MagiaEditorWidget::onDebugClicked() {
        _editor->executeDebug();
        updateActions();
    }

    void MagiaEditorWidget::onStopClicked() {
        _editor->stopExecution();
        updateActions();
    }

    void MagiaEditorWidget::onStepOver() {
        _editor->stepExecution();
        updateActions();
    }

    void MagiaEditorWidget::onContinue() {
        _editor->continueExecution();
        updateActions();
    }

    void MagiaEditorWidget::updateActions(){
        switch (MagiaDebugger::state) {
            case MagiaDebugger::DebuggerState::Coding:
                _playAction->setEnabled(true);
                _debugAction->setEnabled(true);
                _stopAction->setEnabled(false);
                _stepOverAction->setEnabled(false);
                _continueAction->setEnabled(false);
                break;

            case MagiaDebugger::DebuggerState::Running:
                _playAction->setEnabled(false);
                _debugAction->setEnabled(false);
                _stopAction->setEnabled(true);
                _stepOverAction->setEnabled(false);
                _continueAction->setEnabled(false);
                break;

            case MagiaDebugger::DebuggerState::Step_over:
            case MagiaDebugger::DebuggerState::Debugging:
            case MagiaDebugger::DebuggerState::Paused:
                _playAction->setEnabled(false);
                _debugAction->setEnabled(false);
                _stopAction->setEnabled(true);
                _stepOverAction->setEnabled(true);
                _continueAction->setEnabled(true);
                break;
            case MagiaDebugger::DebuggerState::Stopping:
                _playAction->setEnabled(false);
                _debugAction->setEnabled(false);
                _stopAction->setEnabled(false);
                _stepOverAction->setEnabled(false);
                _continueAction->setEnabled(false);
                break;
        }

        _playAction->setIcon(QIcon(MagiaDebugger::state == MagiaDebugger::DebuggerState::Coding ?
                                   ":/resources/images/play_active.svg" :
                                   ":/resources/images/play_inactive.svg"));

        _debugAction->setIcon(QIcon(MagiaDebugger::state == MagiaDebugger::DebuggerState::Coding ?
                                   ":/resources/images/debug_active.svg" :
                                   ":/resources/images/debug_inactive.svg"));

        _stopAction->setIcon(QIcon(MagiaDebugger::state == MagiaDebugger::DebuggerState::Coding ?
                                   ":/resources/images/stop_inactive.svg" :
                                   ":/resources/images/stop_active.svg"));

        _stepOverAction->setIcon(QIcon(MagiaDebugger::state == MagiaDebugger::DebuggerState::Paused||
                                       MagiaDebugger::state == MagiaDebugger::DebuggerState::Step_over ?
                                   ":/resources/images/step_over_active.svg" :
                                   ":/resources/images/step_over_inactive.svg"));

        _continueAction->setIcon(QIcon(MagiaDebugger::state == MagiaDebugger::DebuggerState::Paused||
                                       MagiaDebugger::state == MagiaDebugger::DebuggerState::Step_over ?
                                   ":/resources/images/continue_active.svg" :
                                   ":/resources/images/continue_inactive.svg"));

    }

    void MagiaEditorWidget::onScriptPaused() {
        updateActions();
    }

    void MagiaEditorWidget::onScriptFinished(){
        updateActions();
    }

    void MagiaEditorWidget::onScriptStarted() {
        _console->clear();
    }

    QWidget* MagiaEditorWidget::getCentralWidget(){
        return _centralWidget;
    }
}