#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>
#include <QShortcut>
#include <QDir>

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
    
    // Create vertical layout for editor and terminal
    _editorLayout = new QVBoxLayout();
    _editorLayout->setContentsMargins(0, 0, 0, 0);
    _editorLayout->setSpacing(0);
    
    // Create and setup file explorer
    _fileExplorer = new FileExplorerWidget(_centralWidget);
    _mainLayout->addWidget(_fileExplorer);
    
    // Set the project root path
    QString projectPath = QDir::currentPath();
    _fileExplorer->setRootPath(projectPath);
    
    // Create and setup code editor
    _editor = new aic::CodeEditor(_centralWidget);
    _editorLayout->addWidget(_editor->getCentralWidget(), 7); // 70% of space
    
    // Setup terminal
    setupTerminal();
    
    // Add editor layout to main layout
    _mainLayout->addLayout(_editorLayout, 1);
    
    // Create and setup AI chat widget
    _aiChat = new AIChatWidget(_centralWidget);
    _mainLayout->addWidget(_aiChat);
    
    // Connect signals
    connect(_aiChat, &AIChatWidget::promptSubmitted, this, &MainWindow::handleAIPrompt);
    connect(_fileExplorer, &FileExplorerWidget::fileSelected, this, &MainWindow::handleFileSelected);
    
    setCentralWidget(_centralWidget);
    
    // Setup actions and shortcuts
    setupActions();
}

void MainWindow::setupTerminal()
{
    _terminal = new QTermWidget(0, _centralWidget);
    _terminal->setColorScheme("Solarized");
    _terminal->setTerminalFont(QFont("Monospace", 10));
    _terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
    _terminal->setWorkingDirectory(QDir::currentPath());
    _editorLayout->addWidget(_terminal, 3); // 30% of space
}

void MainWindow::setupActions()
{
    // Create toggle action for AI Chat
    _toggleAIChatAction = new QAction("Toggle AI Chat", this);
    _toggleAIChatAction->setShortcut(QKeySequence("Ctrl+Shift+A"));
    _toggleAIChatAction->setStatusTip("Show/Hide AI Chat Widget");
    connect(_toggleAIChatAction, &QAction::triggered, this, &MainWindow::toggleAIChatWidget);
    
    // Create toggle action for File Explorer
    _toggleFileExplorerAction = new QAction("Toggle File Explorer", this);
    _toggleFileExplorerAction->setShortcut(QKeySequence("Ctrl+B"));
    _toggleFileExplorerAction->setStatusTip("Show/Hide File Explorer");
    connect(_toggleFileExplorerAction, &QAction::triggered, this, &MainWindow::toggleFileExplorer);
    
    // Create toggle action for Terminal
    _toggleTerminalAction = new QAction("Toggle Terminal", this);
    _toggleTerminalAction->setShortcut(QKeySequence("Ctrl+`"));
    _toggleTerminalAction->setStatusTip("Show/Hide Terminal");
    connect(_toggleTerminalAction, &QAction::triggered, this, &MainWindow::toggleTerminal);
    
    // Add actions to window
    addAction(_toggleAIChatAction);
    addAction(_toggleFileExplorerAction);
    addAction(_toggleTerminalAction);
}

void MainWindow::toggleAIChatWidget()
{
    if (_aiChat) {
        _aiChat->setVisible(!_aiChat->isVisible());
    }
}

void MainWindow::toggleFileExplorer()
{
    if (_fileExplorer) {
        _fileExplorer->setVisible(!_fileExplorer->isVisible());
    }
}

void MainWindow::toggleTerminal()
{
    if (_terminal) {
        _terminal->setVisible(!_terminal->isVisible());
    }
}

void MainWindow::handleFileSelected(const QString& filePath)
{
    if (_editor) {
        _editor->openFile(filePath);
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
//    _aiChat->findChild<QTextEdit*>()->append("<b>AI:</b> Processing: " + prompt);
}
