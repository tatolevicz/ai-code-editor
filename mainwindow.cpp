#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>
#include "MagiaEditorWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _editor = new mg::MagiaEditorWidget(this);
    setCentralWidget(_editor->getCentralWidget());
}

MainWindow::~MainWindow()
{
    delete ui;
}

