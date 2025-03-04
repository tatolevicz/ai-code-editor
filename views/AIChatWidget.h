#ifndef AICHATWIDGET_H
#define AICHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QMetaObject>
#include <QLabel>
#include "MgStyles.h"
#include "MagiaTheme.h"

#ifndef Q_MOC_RUN
#include <ais>
#endif

class AIChatWidget : public QWidget
{
Q_OBJECT

public:
  explicit AIChatWidget(QWidget *parent = nullptr,
                        ais::CommandExecutor* executor = nullptr,
                        std::shared_ptr<ais::AgentProcessor> processor = nullptr)
      : QWidget(parent),
        _streamer(std::make_shared<ais::AIStreamer>()),
        _processor(processor ? processor : std::make_shared<ais::AgentProcessor>(executor)),
        _agent(std::make_shared<ais::AIAgent>(ais::agents::cascadeV2)),
        _pendingUpdate(false)
  {
    setFixedWidth(400);
    setupUI();
    setupAI();
  }

  // Método para adicionar saída do terminal ao chat e enviar para a IA
  void appendTerminalOutput(const QString& output) {
    // Adiciona a saída como uma mensagem do usuário para a IA
//    _agent->addAssistantMessageaddAssistantMessage(output.toStdString());

    // Faz uma nova chamada à IA para processar a saída
    _streamer->call(*_agent);
  }

  std::shared_ptr<ais::AIAgent> getAgent() {
    return _agent;
  }

  // Método que tenta atualizar o agente; se o agente estiver processando,
  // marca a atualização como pendente e tentará novamente assim que o processamento terminar.
  void updateAgent() {
      _pendingUpdate = true;
  }



private:
  void updateAgentInternal() {
    if (!_isProcessing) {
      _streamer->call(*_agent);
    } else {
      logW << "Agent is processing already. Update marked as pending.";
      _pendingUpdate = true;
    }
  }

  QTextEdit *responseArea;
  QLineEdit *promptInput;
  QPushButton *sendButton;
  QPushButton *closeButton;
  QLabel *titleLabel;

  // AI Components
  std::shared_ptr<ais::AIStreamer> _streamer;
  std::shared_ptr<ais::AgentProcessor> _processor;
  std::shared_ptr<ais::AIAgent> _agent;

  bool _isProcessing{false};
  bool _pendingUpdate{false};
  QTimer _processingTimer;

  void setupAI() {
    _streamer->setOnStart([this]() {
      QMetaObject::invokeMethod(this, [this]() {
        _isProcessing = true;
        sendButton->setEnabled(false);
        promptInput->setEnabled(false);
      }, Qt::QueuedConnection);
    });

    _streamer->setOnUpdate([this](const std::string &update) {
      QMetaObject::invokeMethod(this, [this, update]() {
        responseArea->insertPlainText(QString::fromStdString(update));
        _processor->process(update);
      }, Qt::QueuedConnection);
    });

    _streamer->setOnFinish([this](const std::string &answer)
    {
      _isProcessing = false;
      _agent->addAssistantMessage(answer);

      if (_pendingUpdate)
      {
        QMetaObject::invokeMethod(this, [this]()
        {
          _pendingUpdate = false;
          updateAgentInternal();
        });
        return;
//
//        QTimer::singleShot(2000, this, [this]() {
//          _pendingUpdate = false;
//          updateAgentInternal();
//        });
//        return;
      }

      QMetaObject::invokeMethod(this, [this]()
      {
        sendButton->setEnabled(true);
        promptInput->setEnabled(true);
        responseArea->append("\n");

      }, Qt::QueuedConnection);
    });

    // Timer para atualizar a UI conforme o estado do processamento
    _processingTimer.setInterval(100); // 100ms
    connect(&_processingTimer, &QTimer::timeout, this, [this]() {
      sendButton->setEnabled(!_isProcessing);
      promptInput->setEnabled(!_isProcessing);
    });
    _processingTimer.start();
  }

  void setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setStyleSheet(QString("AIChatWidget { background-color: #%1; border-left: 1px solid #%2; }")
                      .arg(mg::theme::Colors::SIDEBAR_BG, 6, 16, QChar('0'))
                      .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));

    // Header com título e botão de fechar
    auto headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(12, 8, 12, 8);
    headerLayout->setSpacing(0);

    titleLabel = new QLabel("AI Assistant", this);
    titleLabel->setStyleSheet(QString("QLabel { color: #%1; font-weight: bold; font-size: 14px; }")
                                  .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0')));

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    closeButton = new QPushButton("×", this);
    closeButton->setFixedSize(24, 24);
    closeButton->setStyleSheet(QString("QPushButton { border: none; border-radius: 12px; background-color: #%1; color: #%2; font-weight: bold; font-size: 16px; }"
                                       "QPushButton:hover { background-color: #%3; }")
                                   .arg(mg::theme::Colors::PRIMARY, 6, 16, QChar('0'))
                                   .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                                   .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0')));
    headerLayout->addWidget(closeButton);
    mainLayout->addLayout(headerLayout);

    // Linha separadora
    QFrame* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet(QString("QFrame { background-color: #%1; max-height: 1px; }")
                                 .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));
    mainLayout->addWidget(separator);

    // Layout principal do conteúdo
    auto contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(12, 12, 12, 12);
    contentLayout->setSpacing(12);

    responseArea = new QTextEdit(this);
    responseArea->setReadOnly(true);
    responseArea->setPlaceholderText("AI responses will appear here...");
    responseArea->setStyleSheet(QString("QTextEdit { background-color: #%1; color: #%2; border: 1px solid #%3; border-radius: 8px; padding: 12px; font-family: 'JetBrains Mono', monospace; }"
                                        "QTextEdit::placeholder { color: #%4; }")
                                    .arg(mg::theme::Colors::BACKGROUND, 6, 16, QChar('0'))
                                    .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                                    .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0'))
                                    .arg(mg::theme::Colors::COMMENT, 6, 16, QChar('0')));
    contentLayout->addWidget(responseArea);

    auto inputContainer = new QWidget(this);
    inputContainer->setStyleSheet(QString("QWidget { background-color: #%1; border: 1px solid #%2; border-radius: 8px; }")
                                      .arg(mg::theme::Colors::BACKGROUND, 6, 16, QChar('0'))
                                      .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));

    auto inputLayout = new QHBoxLayout(inputContainer);
    inputLayout->setContentsMargins(8, 8, 8, 8);
    inputLayout->setSpacing(8);

    promptInput = new QLineEdit(inputContainer);
    promptInput->setPlaceholderText("Type your prompt here...");
    promptInput->setStyleSheet(QString("QLineEdit { padding: 8px; background-color: transparent; color: #%1; border: none; font-family: 'JetBrains Mono', monospace; }"
                                       "QLineEdit::placeholder { color: #%2; }")
                                   .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                                   .arg(mg::theme::Colors::COMMENT, 6, 16, QChar('0')));

    sendButton = new QPushButton("Send", inputContainer);
    sendButton->setStyleSheet(QString("QPushButton { padding: 8px 16px; background-color: #%1; color: #%2; border: none; border-radius: 4px; font-weight: bold; }"
                                      "QPushButton:hover { background-color: #%3; }")
                                  .arg(mg::theme::Colors::PRIMARY, 6, 16, QChar('0'))
                                  .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                                  .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0')));

    inputLayout->addWidget(promptInput);
    inputLayout->addWidget(sendButton);
    contentLayout->addWidget(inputContainer);

    mainLayout->addLayout(contentLayout);

    connect(sendButton, &QPushButton::clicked, this, &AIChatWidget::onSendClicked);
    connect(promptInput, &QLineEdit::returnPressed, this, &AIChatWidget::onSendClicked);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::hide);

    setLayout(mainLayout);
  }

private slots:
  void onSendClicked() {
    auto prompt = promptInput->text();
    if (!prompt.isEmpty() && !_isProcessing) {
      _agent->addUserMessage(prompt.toStdString());
      responseArea->append("<b>You:</b> " + prompt);
      responseArea->append("<b>AI:</b> ");
      promptInput->clear();
      _streamer->call(*_agent);
      emit promptSubmitted(prompt);
    }
  }

signals:
  void promptSubmitted(const QString &prompt);
};

#endif // AICHATWIDGET_H
