#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _editor = new aic::CodeEditor(this);
    setCentralWidget(_editor->getCentralWidget());
}

MainWindow::~MainWindow()
{
    delete ui;
}

