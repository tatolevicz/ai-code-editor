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
    
    // Inicializa o AgentProcessor com este MainWindow como CommandExecutor
    _agentProcessor = std::make_shared<ais::AgentProcessor>(this);
    
    // Create and setup AI chat widget
    _aiChat = new AIChatWidget(_centralWidget, this, _agentProcessor);
    _mainLayout->addWidget(_aiChat);
    
    // Connect signals
    connect(_aiChat, &AIChatWidget::promptSubmitted, this, &MainWindow::handleAIPrompt);
    connect(_fileExplorer, &FileExplorerWidget::fileSelected, this, &MainWindow::handleFileSelected);
    
    setCentralWidget(_centralWidget);
    
    // Setup actions and shortcuts
    setupActions();
    
    // Registra os callbacks para o processador de ações da IA
    registerAICallbacks();
}

void MainWindow::registerAICallbacks()
{
    // Registrar callback para execução de comandos no terminal
    _agentProcessor->registerActionCallback("run_command", [this](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("command")) {
            std::string command = doc["command"].GetString();
            std::vector<std::string> arguments;
            if(doc.HasMember("arguments") && doc["arguments"].IsArray()) {
                auto& args = doc["arguments"];
                for (rapidjson::SizeType i = 0; i < args.Size(); i++) {
                    arguments.push_back(args[i].GetString());
                }
            }
            
            // Construir comando completo
            std::string fullCommand = command;
            for (const auto &arg : arguments) {
              fullCommand += " " + arg;
            }
            
            // Executar o comando no terminal
            this->executeTerminalCommand(fullCommand);
        }
    });
    
    // Registrar callback para escrita em arquivos
    _agentProcessor->registerActionCallback("write_file", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("file") && doc.HasMember("content")) {
            std::string filePath = doc["file"].GetString();
            std::string content = doc["content"].GetString();
            try {
                mgutils::Files::writeFile(filePath, content);
                logW << "Arquivo criado com sucesso: " << filePath;
            } catch (...) {
                logE << "Erro ao escrever arquivo: " << filePath;
            }
        }
    });
    
    // Registrar callback para criação de diretórios
    _agentProcessor->registerActionCallback("make_dir", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("relative_path")) {
            std::string relativePath = doc["relative_path"].GetString();
            try {
                mgutils::Files::createDirectory(relativePath);
                logW << "Diretório criado com sucesso: " << relativePath;
            } catch (...) {
                logE << "Erro ao criar diretório: " << relativePath;
            }
        }
    });
    
    // Registrar callback para visualização de arquivos
    _agentProcessor->registerActionCallback("view_file", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("file")) {
            std::string filePath = doc["file"].GetString();
            try {
                std::string content = mgutils::Files::readFile(filePath);
                logI << "Conteúdo do arquivo " << filePath << ":\n" << content;
            } catch (...) {
                logE << "Erro ao visualizar arquivo: " << filePath;
            }
        }
    });
    
    // Registrar callback para edição de arquivos
    _agentProcessor->registerActionCallback("edit_file", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("file") && doc.HasMember("changes")) {
            std::string filePath = doc["file"].GetString();
            std::string changes = doc["changes"].GetString();
            try {
                if(!mgutils::Files::fileExists(filePath))
                    mgutils::Files::createFile(filePath);
                mgutils::Files::writeFile(filePath, changes);
                logW << "Arquivo editado com sucesso: " << filePath;
            } catch (...) {
                logE << "Erro ao editar arquivo: " << filePath;
            }
        }
    });
    
    // Registrar callback para busca de arquivos
    _agentProcessor->registerActionCallback("find", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("pattern")) {
            std::string pattern = doc["pattern"].GetString();
            std::string directory = ".";
            if(doc.HasMember("directory")) {
                directory = doc["directory"].GetString();
            }
            logW << "Buscando arquivos com padrão " << pattern << " em " << directory;
            // Implementar busca usando mgutils::Files ou outra biblioteca
        }
    });
    
    // Registrar callback para grep search
    _agentProcessor->registerActionCallback("grep_search", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("pattern") && doc.HasMember("file")) {
            std::string pattern = doc["pattern"].GetString();
            std::string file = doc["file"].GetString();
            try {
                std::string content = mgutils::Files::readFile(file);
                std::stringstream ss(content);
                std::string line;
                int lineNumber = 0;
                logW << "Resultados da busca por '" << pattern << "' em " << file << ":";
                std::regex patternRegex(pattern);
                while (std::getline(ss, line)) {
                    lineNumber++;
                    if (std::regex_search(line, patternRegex)) {
                        logI << file << ":" << lineNumber << ": " << line;
                    }
                }
            } catch (...) {
                logE << "Erro ao realizar busca grep: " << pattern << " em " << file;
            }
        }
    });
    
    // Registrar callback para listar diretórios
    _agentProcessor->registerActionCallback("list_directory", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("directory")) {
            std::string directory = doc["directory"].GetString();
            logW << "Listando conteúdo do diretório: " << directory;
            // Implementar listagem de diretórios usando mgutils::Files ou outra biblioteca
        }
    });
    
    // Registrar callback para ler conteúdo de URL
    _agentProcessor->registerActionCallback("read_url_content", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("url")) {
            std::string url = doc["url"].GetString();
            logW << "Lendo conteúdo da URL: " << url;
            // Aqui seria implementada a lógica para fazer requisição HTTP
        }
    });
    
    // Registrar callback para busca na web
    _agentProcessor->registerActionCallback("search_web", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("query")) {
            std::string query = doc["query"].GetString();
            std::string domain = "";
            if(doc.HasMember("domain")) {
                domain = doc["domain"].GetString();
            }
            logW << "Realizando busca na web por: " << query;
            if (!domain.empty()) {
                logW << "Domínio: " << domain;
            }
            // Aqui seria implementada a lógica para realizar a busca na web
        }
    });
    
    // Registrar callback para visualizar item de código
    _agentProcessor->registerActionCallback("view_code_item", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("file") && doc.HasMember("identifier")) {
            std::string file = doc["file"].GetString();
            std::string identifier = doc["identifier"].GetString();
            logW << "Visualizando definição de " << identifier << " em " << file;
            // Aqui seria implementada a lógica para extrair e exibir a definição do item de código
        }
    });
    
    // Registrar callback para visualizar chunk de documento web
    _agentProcessor->registerActionCallback("view_web_document_content_chunk", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("url") && doc.HasMember("chunk")) {
            std::string url = doc["url"].GetString();
            int chunk = doc["chunk"].GetInt();
            logI << "Visualizando chunk " << chunk << " do documento em " << url;
            // Aqui seria implementada a lógica para recuperar e exibir o chunk específico
        }
    });
    
    // Registrar callback para busca na base de código
    _agentProcessor->registerActionCallback("codebase_search", [](const mgutils::JsonDocument& doc) {
        if(doc.HasMember("query")) {
            std::string query = doc["query"].GetString();
            std::string corpus = "";
            if(doc.HasMember("corpus")) {
                corpus = doc["corpus"].GetString();
            }
            logW << "Realizando busca na base de código por: " << query;
            if (!corpus.empty()) {
                logW << "Corpus: " << corpus;
            }
            // Aqui seria implementada a lógica para realizar a busca semântica na base de código
        }
    });
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
    
    // Conectar o sinal receivedData para capturar a saída do terminal
    connect(_terminal, &QTermWidget::receivedData, this, &MainWindow::handleTerminalOutput);
    
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
}

void MainWindow::handleAIPrompt(const QString& prompt)
{
    // TODO: Implement AI response handling here
    // For demonstration, we'll echo the prompt back
//    _aiChat->findChild<QTextEdit*>()->append("<b>AI:</b> Processing: " + prompt);
}

// Modifique o método de execução para incluir o marcador
void MainWindow::executeTerminalCommand(const std::string &command)
{
  if (_terminal) {
    // Converter e montar o comando com o marcador
    QString qCommand = QString::fromStdString(command);
    QString marker = "echo __COMMAND_DONE__\n";
    qCommand += "\n" + marker;

    // Limpa o buffer de saída para o novo comando
    commandOutput.clear();

    // Envia o comando para o terminal
    _terminal->sendText(qCommand);
  }
}

// No slot de saída, detecte o marcador e envie a saída acumulada para a IA
void MainWindow::handleTerminalOutput(const QString& text)
{
  commandOutput += text;

  // Verifica se o marcador foi recebido
  if (commandOutput.contains("__COMMAND_DONE__")) {
    // Remove o marcador da saída
    commandOutput.replace("__COMMAND_DONE__", "");

    // Cria e envia a mensagem para a IA
    if (_aiChat) {
      QString message = "Terminal output:\n```\n" + commandOutput + "\n```";
      _aiChat->appendTerminalOutput(message);
    }

    // Limpa o buffer para o próximo comando
    commandOutput.clear();
  }

  logD << "Terminal recebeu: " << text.toStdString();
}

