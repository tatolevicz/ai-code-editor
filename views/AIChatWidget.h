#ifndef AICHATWIDGET_H
#define AICHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include "MgStyles.h"

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
        
        // Set widget background color
        setStyleSheet(QString("AIChatWidget { background-color: #%1; }").arg(mg::styles::LuaEditorColors::DARK_BACKGROUND, 6, 16, QChar('0')));
        
        // Spacer to push close button to the right
        headerLayout->addStretch();
        
        // Close button setup
        closeButton = new QPushButton("×", this);
        closeButton->setFixedSize(20, 20);
        closeButton->setStyleSheet("QPushButton { border: none; border-radius: 10px; background-color: #B46981; color: white; font-weight: bold; }"
                                 "QPushButton:hover { background-color: #D98E6C; }");
        headerLayout->addWidget(closeButton);
        mainLayout->addLayout(headerLayout);
        
        // Main content layout
        auto contentLayout = new QVBoxLayout();
        contentLayout->setContentsMargins(8, 0, 8, 8);
        
        // Response area setup
        responseArea = new QTextEdit(this);
        responseArea->setReadOnly(true);
        responseArea->setPlaceholderText("AI responses will appear here...");
        responseArea->setStyleSheet(QString("QTextEdit { background-color: #%1; color: #%2; border: 1px solid #%3; border-radius: 4px; padding: 8px; }"
                                          "QTextEdit::placeholder { color: #%4; }")
                                          .arg(mg::styles::LuaEditorColors::BACKGROUND, 6, 16, QChar('0'))
                                          .arg(mg::styles::LuaEditorColors::IDENTIFIER, 6, 16, QChar('0'))
                                          .arg(mg::styles::LuaEditorColors::OPERATOR, 6, 16, QChar('0'))
                                          .arg(mg::styles::LuaEditorColors::COMMENT, 6, 16, QChar('0')));
        contentLayout->addWidget(responseArea);

        // Prompt input setup
        promptInput = new QLineEdit(this);
        promptInput->setPlaceholderText("Type your prompt here...");
        promptInput->setStyleSheet(QString("QLineEdit { padding: 8px; background-color: #%1; color: #%2; border: 1px solid #%3; border-radius: 4px; }"
                                         "QLineEdit::placeholder { color: #%4; }")
                                         .arg(mg::styles::LuaEditorColors::BACKGROUND, 6, 16, QChar('0'))
                                         .arg(mg::styles::LuaEditorColors::IDENTIFIER, 6, 16, QChar('0'))
                                         .arg(mg::styles::LuaEditorColors::OPERATOR, 6, 16, QChar('0'))
                                         .arg(mg::styles::LuaEditorColors::COMMENT, 6, 16, QChar('0')));
        
        // Send button setup
        sendButton = new QPushButton("Send", this);
        sendButton->setStyleSheet(QString("QPushButton { padding: 8px 16px; background-color: #%1; color: #%2; border: none; border-radius: 4px; }"
                                        "QPushButton:hover { background-color: #%3; }")
                                        .arg(mg::styles::LuaEditorColors::KEYWORD, 6, 16, QChar('0'))
                                        .arg(mg::styles::LuaEditorColors::IDENTIFIER, 6, 16, QChar('0'))
                                        .arg(mg::styles::LuaEditorColors::OPERATOR, 6, 16, QChar('0')));

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
