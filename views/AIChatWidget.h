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

//#include <AIStreamer.h>
//#include <AgentProcessor.h>
//#include <AIAgent.h>
#ifndef Q_MOC_RUN
#include <ais>
#endif

class AIChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIChatWidget(QWidget *parent = nullptr, ais::CommandExecutor* executor = nullptr) : QWidget(parent),
        _streamer(std::make_shared<ais::AIStreamer>()),
        _processor(std::make_shared<ais::AgentProcessor>(executor)),
        _agent(std::make_shared<ais::AIAgent>(ais::agents::cascade))
    {
        setFixedWidth(400);
        setupUI();
        setupAI();
    }

private:
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
    QTimer _processingTimer;

    void setupAI()
    {
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

        _streamer->setOnFinish([this](const std::string& answer)
        {
          _agent->addAssistantMessage(answer);
            QMetaObject::invokeMethod(this, [this]()
            {
                _isProcessing = false;
                sendButton->setEnabled(true);
                promptInput->setEnabled(true);
                responseArea->append("\n"); // Add a line break for the next message
            }, Qt::QueuedConnection);
        });

        // Timer para verificar se podemos enviar nova mensagem
        _processingTimer.setInterval(100); // 100ms
        connect(&_processingTimer, &QTimer::timeout, this, [this]() {
            sendButton->setEnabled(!_isProcessing);
            promptInput->setEnabled(!_isProcessing);
        });
        _processingTimer.start();
    }

    void setupUI()
    {
        auto mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        
        // Set widget background color using the new theme colors
        setStyleSheet(QString("AIChatWidget { background-color: #%1; border-left: 1px solid #%2; }")
                     .arg(mg::theme::Colors::SIDEBAR_BG, 6, 16, QChar('0'))
                     .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));
        
        // Header with title and close button
        auto headerLayout = new QHBoxLayout();
        headerLayout->setContentsMargins(12, 8, 12, 8);
        headerLayout->setSpacing(0);
        
        // Add AI Chat title with icon
        titleLabel = new QLabel("AI Assistant", this);
        titleLabel->setStyleSheet(QString("QLabel { color: #%1; font-weight: bold; font-size: 14px; }")
                               .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0')));
        
        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        
        // Close button setup with modern design
        closeButton = new QPushButton("×", this);
        closeButton->setFixedSize(24, 24);
        closeButton->setStyleSheet(QString("QPushButton { border: none; border-radius: 12px; background-color: #%1; color: #%2; font-weight: bold; font-size: 16px; }"
                                  "QPushButton:hover { background-color: #%3; }")
                                  .arg(mg::theme::Colors::PRIMARY, 6, 16, QChar('0'))
                                  .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                                  .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0')));
        headerLayout->addWidget(closeButton);
        mainLayout->addLayout(headerLayout);
        
        // Add separator line
        QFrame* separator = new QFrame(this);
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        separator->setStyleSheet(QString("QFrame { background-color: #%1; max-height: 1px; }")
                               .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));
        mainLayout->addWidget(separator);
        
        // Main content layout
        auto contentLayout = new QVBoxLayout();
        contentLayout->setContentsMargins(12, 12, 12, 12);
        contentLayout->setSpacing(12);
        
        // Response area setup with modern styling
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
        
        // Input area with rounded corners and modern styling
        auto inputContainer = new QWidget(this);
        inputContainer->setStyleSheet(QString("QWidget { background-color: #%1; border: 1px solid #%2; border-radius: 8px; }")
                                    .arg(mg::theme::Colors::BACKGROUND, 6, 16, QChar('0'))
                                    .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));
                                    
        auto inputLayout = new QHBoxLayout(inputContainer);
        inputLayout->setContentsMargins(8, 8, 8, 8);
        inputLayout->setSpacing(8);
        
        // Prompt input setup
        promptInput = new QLineEdit(inputContainer);
        promptInput->setPlaceholderText("Type your prompt here...");
        promptInput->setStyleSheet(QString("QLineEdit { padding: 8px; background-color: transparent; color: #%1; border: none; font-family: 'JetBrains Mono', monospace; }"
                                      "QLineEdit::placeholder { color: #%2; }")
                                      .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                                      .arg(mg::theme::Colors::COMMENT, 6, 16, QChar('0')));
        
        // Send button setup with modern purple styling
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
        
        // Connect signals and slots
        connect(sendButton, &QPushButton::clicked, this, &AIChatWidget::onSendClicked);
        connect(promptInput, &QLineEdit::returnPressed, this, &AIChatWidget::onSendClicked);
        connect(closeButton, &QPushButton::clicked, this, &QWidget::hide);
        
        setLayout(mainLayout);
    }

private slots:
    void onSendClicked()
    {
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
