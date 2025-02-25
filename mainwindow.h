#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include "views/CodeEditor.h"
#include "views/AIChatWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget* _centralWidget{nullptr};
    QHBoxLayout* _mainLayout{nullptr};
    aic::CodeEditor* _editor{nullptr};
    AIChatWidget* _aiChat{nullptr};

private slots:
    void handleAIPrompt(const QString& prompt);
};
#endif // MAINWINDOW_H
