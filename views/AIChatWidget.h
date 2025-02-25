#ifndef AICHATWIDGET_H
#define AICHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

class AIChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIChatWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setFixedWidth(400);
        setupUI();
    }

private:
    QTextEdit *responseArea;
    QLineEdit *promptInput;
    QPushButton *sendButton;
    QPushButton *closeButton;

    void setupUI()
    {
        auto mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        
        // Header with close button
        auto headerLayout = new QHBoxLayout();
        headerLayout->setContentsMargins(4, 4, 4, 4);
        
        // Spacer to push close button to the right
        headerLayout->addStretch();
        
        // Close button setup
        closeButton = new QPushButton("×", this);
        closeButton->setFixedSize(20, 20);
        closeButton->setStyleSheet("QPushButton { border: none; border-radius: 10px; background-color: #ff6b6b; color: white; font-weight: bold; }"
                                 "QPushButton:hover { background-color: #ff4444; }");
        headerLayout->addWidget(closeButton);
        mainLayout->addLayout(headerLayout);
        
        // Main content layout
        auto contentLayout = new QVBoxLayout();
        contentLayout->setContentsMargins(8, 0, 8, 8);
        
        // Response area setup
        responseArea = new QTextEdit(this);
        responseArea->setReadOnly(true);
        responseArea->setPlaceholderText("AI responses will appear here...");
        responseArea->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 4px; padding: 8px; }");
        contentLayout->addWidget(responseArea);

        // Prompt input setup
        promptInput = new QLineEdit(this);
        promptInput->setPlaceholderText("Type your prompt here...");
        promptInput->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ddd; border-radius: 4px; }");
        
        // Send button setup
        sendButton = new QPushButton("Send", this);
        sendButton->setStyleSheet("QPushButton { padding: 8px 16px; background-color: #007bff; color: white; border: none; border-radius: 4px; }"
                                "QPushButton:hover { background-color: #0056b3; }");

        // Create horizontal layout for input and button
        auto inputLayout = new QHBoxLayout();
        inputLayout->addWidget(promptInput);
        inputLayout->addWidget(sendButton);
        contentLayout->addLayout(inputLayout);
        
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
        QString prompt = promptInput->text();
        if (!prompt.isEmpty()) {
            responseArea->append("<b>You:</b> " + prompt);
            promptInput->clear();
            emit promptSubmitted(prompt);
        }
    }

signals:
    void promptSubmitted(const QString &prompt);
};

#endif // AICHATWIDGET_H
