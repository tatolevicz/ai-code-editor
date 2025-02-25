#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>

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
