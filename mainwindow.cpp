#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Create central widget and main layout
    _centralWidget = new QWidget(this);
    _mainLayout = new QHBoxLayout(_centralWidget);
    _mainLayout->setContentsMargins(0, 0, 0, 0);
    _mainLayout->setSpacing(0);
    
    // Create and setup code editor
    _editor = new aic::CodeEditor(_centralWidget);
    _mainLayout->addWidget(_editor->getCentralWidget());
    
    // Create and setup AI chat widget
    _aiChat = new AIChatWidget(_centralWidget);
    _mainLayout->addWidget(_aiChat);
    
    // Connect AI chat signals
    connect(_aiChat, &AIChatWidget::promptSubmitted, this, &MainWindow::handleAIPrompt);
    
    setCentralWidget(_centralWidget);
    
    // Setup actions and shortcuts
    setupActions();
}

void MainWindow::setupActions()
{
    // Create toggle action
    _toggleAIChatAction = new QAction("Toggle AI Chat", this);
    _toggleAIChatAction->setShortcut(QKeySequence("Ctrl+Shift+A"));
    _toggleAIChatAction->setStatusTip("Show/Hide AI Chat Widget");
    connect(_toggleAIChatAction, &QAction::triggered, this, &MainWindow::toggleAIChatWidget);
    
    // Add action to window
    addAction(_toggleAIChatAction);
}

void MainWindow::toggleAIChatWidget()
{
    if (_aiChat) {
        _aiChat->setVisible(!_aiChat->isVisible());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleAIPrompt(const QString& prompt)
{
    // TODO: Implement AI response handling here
    // For demonstration, we'll echo the prompt back
    _aiChat->findChild<QTextEdit*>()->append("<b>AI:</b> Processing: " + prompt);
}
