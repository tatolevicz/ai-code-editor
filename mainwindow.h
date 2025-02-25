#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <qtermwidget.h>
#include "views/CodeEditor.h"
#include "views/AIChatWidget.h"
#include "views/FileExplorerWidget.h"

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
    void setupActions();
    void setupTerminal();
    
private slots:
    void handleAIPrompt(const QString& prompt);
    void toggleAIChatWidget();
    void toggleFileExplorer();
    void toggleTerminal();
    void handleFileSelected(const QString& filePath);

private:
    Ui::MainWindow *ui;
    QWidget* _centralWidget{nullptr};
    QHBoxLayout* _mainLayout{nullptr};
    QVBoxLayout* _editorLayout{nullptr};
    FileExplorerWidget* _fileExplorer{nullptr};
    aic::CodeEditor* _editor{nullptr};
    AIChatWidget* _aiChat{nullptr};
    QTermWidget* _terminal{nullptr};
    QAction* _toggleAIChatAction{nullptr};
    QAction* _toggleFileExplorerAction{nullptr};
    QAction* _toggleTerminalAction{nullptr};
};
#endif // MAINWINDOW_H
