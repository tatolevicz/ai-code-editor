#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>
#include <QShortcut>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Create central widget and main layout
    _centralWidget = new QWidget(this);
    _centralWidget->setStyleSheet(QString("background-color: #%1;")
                                .arg(mg::theme::Colors::SIDEBAR_BG, 6, 16, QChar('0')));
    
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
    _aiChat = new AIChatWidget(_centralWidget, this);
    _mainLayout->addWidget(_aiChat);
    
    // Connect signals
    connect(_aiChat, &AIChatWidget::promptSubmitted, this, &MainWindow::handleAIPrompt);
    connect(_fileExplorer, &FileExplorerWidget::fileSelected, this, &MainWindow::handleFileSelected);
    
    setCentralWidget(_centralWidget);
    
    // Setup actions and shortcuts
    setupActions();
    
    // Inicializa o AgentProcessor com este MainWindow como CommandExecutor
    _agentProcessor = new ais::AgentProcessor(this);
}

void MainWindow::setupTerminal()
{
    _terminal = new QTermWidget(0, _centralWidget);
    
    // Configure shell program
    _terminal->setShellProgram("/bin/zsh"); // Use zsh as default shell on macOS
    
    // Set environment variables
    QStringList environment = QProcess::systemEnvironment();
    environment << "TERM=xterm-256color";
    _terminal->setEnvironment(environment);
    
    // Customizar cores e aparência do terminal
    // Usar um esquema de cores que combina com o tema Magia
    _terminal->setColorScheme("Linux"); // Ubuntu tem cores roxas/azuis escuras

    // Configurar a opacidade do terminal para um efeito mais moderno
    _terminal->setTerminalOpacity(1.0); // Opacidade completa
    
    // Limpar qualquer imagem de fundo
    _terminal->setTerminalBackgroundImage("");
    
    // Modo de fundo - usar valor numérico já que o enum não está acessível
    // Valores comuns para TerminalBackgroundMode: 0=None, 1=Stretch, 2=Tile, 3=Center, 4=Transparent
    _terminal->setTerminalBackgroundMode(0); // Modo normal sem transparência
    
    // Aplicar estilo CSS para complementar a aparência do terminal
    // Isso afeta o widget de contêiner e seus elementos internos
    QString terminalStyle = QString(
        "QWidget {"
        "  background-color: #%1;"
        "  color: #%2;"
        "  border: 1px solid #%3;"
        "  border-radius: 4px;"
        "}"
        // Remover background de labels e elementos de texto
        "QLabel, QTextEdit, QLineEdit, QPlainTextEdit {"
        "  background: transparent;"
        "  background-color: transparent;"
        "  border: none;"
        "}"
        // Para ter certeza que elementos internos são transparentes
        "QTermWidget > * {"
        "  background-color: transparent;"
        "}"
    ).arg(
        QString::number(mg::theme::Colors::TERMINAL_BG, 16).rightJustified(6, '0'),
        QString::number(mg::theme::Colors::DEFAULT_TEXT, 16).rightJustified(6, '0'),
        QString::number(mg::theme::Colors::BORDER, 16).rightJustified(6, '0')
    );
    
    _terminal->setStyleSheet(terminalStyle);
    
    // Configurar margens para dar um espaçamento elegante
    _terminal->setMargin(4);
    
    // Usar uma fonte consistente com o restante do editor
    _terminal->setTerminalFont(QFont("JetBrains Mono", 14));
    _terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
    _terminal->setWorkingDirectory(QDir::currentPath());
    
    // Enable flow control and other terminal features
    _terminal->setFlowControlEnabled(true);
    _terminal->setFlowControlWarningEnabled(true);
    
    // Melhorar a aparência dos caracteres de linha
    _terminal->setDrawLineChars(true);
    
    // Texto em negrito também mais intenso
    _terminal->setBoldIntense(true);
    
    // Enable context menu
    _terminal->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_terminal, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QMenu *menu = new QMenu(_terminal);
        
        // Add common terminal actions
        menu->addAction("Copy", _terminal, &QTermWidget::copyClipboard);
        menu->addAction("Paste", _terminal, &QTermWidget::pasteClipboard);
        menu->addSeparator();
        menu->addAction("Clear", _terminal, &QTermWidget::clear);
        
        menu->exec(_terminal->mapToGlobal(pos));
        menu->deleteLater();
    });
    
    // Connect terminal signals
    connect(_terminal, &QTermWidget::finished, this, [this]() {
        // Restart shell when it exits
        _terminal->startShellProgram();
    });
    
    // Start the shell
    _terminal->startShellProgram();
    
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
    delete _agentProcessor; // Liberar a memória do AgentProcessor
}

void MainWindow::handleAIPrompt(const QString& prompt)
{
    // TODO: Implement AI response handling here
    // For demonstration, we'll echo the prompt back
//    _aiChat->findChild<QTextEdit*>()->append("<b>AI:</b> Processing: " + prompt);
}

// Implementação do método executeTerminalCommand da interface CommandExecutor
void MainWindow::executeTerminalCommand(const std::string &command)
{
    if (_terminal) {
        // Converte a string de std::string para QString
        QString qCommand = QString::fromStdString(command);
        
        // Adiciona uma nova linha para executar o comando quando pressionado Enter
        qCommand += "\n";
        
        // Envia o comando para o terminal
        _terminal->sendText(qCommand);
    }
}
