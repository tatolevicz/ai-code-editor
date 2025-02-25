#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QAction>
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
    
private slots:
    void handleAIPrompt(const QString& prompt);
    void toggleAIChatWidget();
    void toggleFileExplorer();
    void handleFileSelected(const QString& filePath);

private:
    Ui::MainWindow *ui;
    QWidget* _centralWidget{nullptr};
    QHBoxLayout* _mainLayout{nullptr};
    FileExplorerWidget* _fileExplorer{nullptr};
    aic::CodeEditor* _editor{nullptr};
    AIChatWidget* _aiChat{nullptr};
    QAction* _toggleAIChatAction{nullptr};
    QAction* _toggleFileExplorerAction{nullptr};
};
#endif // MAINWINDOW_H
