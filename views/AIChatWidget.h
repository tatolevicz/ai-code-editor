#ifndef AICHATWIDGET_H
#define AICHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
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

    void setupUI()
    {
        auto layout = new QVBoxLayout(this);
        
        // Response area setup
        responseArea = new QTextEdit(this);
        responseArea->setReadOnly(true);
        responseArea->setPlaceholderText("AI responses will appear here...");
        responseArea->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 4px; padding: 8px; }");
        layout->addWidget(responseArea);

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
        layout->addLayout(inputLayout);

        // Connect signals and slots
        connect(sendButton, &QPushButton::clicked, this, &AIChatWidget::onSendClicked);
        connect(promptInput, &QLineEdit::returnPressed, this, &AIChatWidget::onSendClicked);

        setLayout(layout);
    }

private slots:
    void onSendClicked()
    {
        QString prompt = promptInput->text();
        if (!prompt.isEmpty()) {
            // Add the user's prompt to the response area
            responseArea->append("<b>You:</b> " + prompt);
            
            // TODO: Implement AI response handling here
            // For now, we'll just clear the input
            promptInput->clear();
            
            // Emit signal for handling the prompt
            emit promptSubmitted(prompt);
        }
    }

signals:
    void promptSubmitted(const QString &prompt);
};

#endif // AICHATWIDGET_H
