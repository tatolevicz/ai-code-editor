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
    
    // Create and setup file explorer
    _fileExplorer = new FileExplorerWidget(_centralWidget);
    _mainLayout->addWidget(_fileExplorer);
    
    // Set the project root path
    QString projectPath = QDir::currentPath();
    _fileExplorer->setRootPath(projectPath);
    
    // Create and setup code editor
    _editor = new aic::CodeEditor(_centralWidget);
    _mainLayout->addWidget(_editor->getCentralWidget());
    
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
    
    // Add actions to window
    addAction(_toggleAIChatAction);
    addAction(_toggleFileExplorerAction);
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
