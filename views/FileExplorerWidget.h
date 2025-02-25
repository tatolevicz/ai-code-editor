#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QDir>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include "MgStyles.h"

class FileExplorerWidget : public QWidget
{
Q_OBJECT

public:
  explicit FileExplorerWidget(QWidget *parent = nullptr) : QWidget(parent)
  {
    setupUI();
  }

  void setRootPath(const QString& path)
  {
    if (_fileModel) {
      QModelIndex index = _fileModel->setRootPath(path);
      _treeView->setRootIndex(index);
      _currentPath = path;
    }
  }

private:
  void setupUI()
  {
    setFixedWidth(250);
    // Fundo geral com DARK_BACKGROUND
    setStyleSheet(QString("FileExplorerWidget { background-color: #%1; }")
                 .arg(mg::styles::LuaEditorColors::DARK_BACKGROUND, 6, 16, QChar('0')));

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Cabeçalho com fundo DARK_BACKGROUND
    auto headerWidget = new QWidget(this);
    headerWidget->setStyleSheet(QString("background-color: #%1;")
                              .arg(mg::styles::LuaEditorColors::DARK_BACKGROUND, 6, 16, QChar('0')));
    auto headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(8, 4, 8, 4);

    auto titleLabel = new QLabel("EXPLORER", headerWidget);
    titleLabel->setStyleSheet(QString("QLabel { color: #%1; font-size: 11px; font-weight: bold; }")
                            .arg(mg::styles::LuaEditorColors::IDENTIFIER, 6, 16, QChar('0')));
    headerLayout->addWidget(titleLabel);

    // Botão New File
    auto newFileButton = new QPushButton("+", headerWidget);
    newFileButton->setFixedSize(20, 20);
    newFileButton->setStyleSheet(
        QString("QPushButton { border: 1px solid #%1; border-radius: 3px; background: transparent; color: #%2; }"
                "QPushButton:hover { background: #%3; }")
                .arg(mg::styles::LuaEditorColors::PRE_PROC, 6, 16, QChar('0'))
                .arg(mg::styles::LuaEditorColors::IDENTIFIER, 6, 16, QChar('0'))
                .arg(mg::styles::LuaEditorColors::LINE_ACIVE, 6, 16, QChar('0'))
    );
    headerLayout->addWidget(newFileButton);

    // Botão Refresh
    auto refreshButton = new QPushButton("⟳", headerWidget);
    refreshButton->setFixedSize(20, 20);
    refreshButton->setStyleSheet(
        QString("QPushButton { border: 1px solid #%1; border-radius: 3px; background: transparent; color: #%2; }"
                "QPushButton:hover { background: #%3; }")
                .arg(mg::styles::LuaEditorColors::PRE_PROC, 6, 16, QChar('0'))
                .arg(mg::styles::LuaEditorColors::IDENTIFIER, 6, 16, QChar('0'))
                .arg(mg::styles::LuaEditorColors::LINE_ACIVE, 6, 16, QChar('0'))
    );
    headerLayout->addWidget(refreshButton);

    mainLayout->addWidget(headerWidget);

    // TreeView com fundo BACKGROUND e texto IDENTIFIER
    _treeView = new QTreeView(this);
    _treeView->setStyleSheet(
        QString("QTreeView { background-color: #%1; color: #%2; border: none; }"
                "QTreeView::item { height: 24px; }"
                "QTreeView::item:hover { background: #%3; }"
                "QTreeView::item:selected { background: #%4; color: black; }")
                .arg(mg::styles::LuaEditorColors::BACKGROUND, 6, 16, QChar('0'))
                .arg(mg::styles::LuaEditorColors::IDENTIFIER, 6, 16, QChar('0'))
                .arg(mg::styles::LuaEditorColors::LINE_ACIVE, 6, 16, QChar('0'))
                .arg(mg::styles::LuaEditorColors::PRE_PROC, 6, 16, QChar('0'))
    );

    _fileModel = new QFileSystemModel(this);
    _fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    _fileModel->setNameFilterDisables(false);
    _treeView->setModel(_fileModel);
    _treeView->setHeaderHidden(true);
    _treeView->setColumnHidden(1, true);  // Tamanho
    _treeView->setColumnHidden(2, true);  // Tipo
    _treeView->setColumnHidden(3, true);  // Data de modificação

    mainLayout->addWidget(_treeView);

    // Menu de contexto
    _treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_treeView, &QTreeView::customContextMenuRequested,
            this, &FileExplorerWidget::showContextMenu);

    // Conexões de sinais
    connect(_treeView, &QTreeView::doubleClicked,
            this, &FileExplorerWidget::onItemDoubleClicked);
    connect(newFileButton, &QPushButton::clicked,
            this, &FileExplorerWidget::onNewFileClicked);
    connect(refreshButton, &QPushButton::clicked,
            this, &FileExplorerWidget::onRefreshClicked);

    setLayout(mainLayout);
  }

  QString getCurrentPath() const
  {
    QModelIndex index = _treeView->currentIndex();
    if (index.isValid()) {
      return _fileModel->filePath(index);
    }
    return _currentPath;
  }

  void createNewFile(const QString& parentPath)
  {
    bool ok;
    QString fileName = QInputDialog::getText(this, "New File",
                                             "Enter file name:", QLineEdit::Normal,
                                             "new_file.txt", &ok);
    if (ok && !fileName.isEmpty()) {
      QString filePath = QDir(parentPath).filePath(fileName);
      QFile file(filePath);
      if (file.exists()) {
        QMessageBox::warning(this, "Error", "File already exists!");
        return;
      }
      if (file.open(QIODevice::WriteOnly)) {
        file.close();
        emit fileCreated(filePath);
      } else {
        QMessageBox::warning(this, "Error", "Failed to create file!");
      }
    }
  }

  void createNewFolder(const QString& parentPath)
  {
    bool ok;
    QString folderName = QInputDialog::getText(this, "New Folder",
                                               "Enter folder name:", QLineEdit::Normal,
                                               "new_folder", &ok);
    if (ok && !folderName.isEmpty()) {
      QDir dir(parentPath);
      if (!dir.mkdir(folderName)) {
        QMessageBox::warning(this, "Error", "Failed to create folder!");
      }
    }
  }

  void deleteItem(const QString& path)
  {
    QFileInfo fileInfo(path);
    QString itemType = fileInfo.isDir() ? "folder" : "file";

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete",
                                  QString("Are you sure you want to delete this %1?\n%2")
                                      .arg(itemType)
                                      .arg(fileInfo.fileName()),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
      bool success;
      if (fileInfo.isDir()) {
        QDir dir(path);
        success = dir.removeRecursively();
      } else {
        QFile file(path);
        success = file.remove();
      }

      if (!success) {
        QMessageBox::warning(this, "Error",
                             QString("Failed to delete %1!").arg(itemType));
      }
    }
  }

  void renameItem(const QString& oldPath)
  {
    QFileInfo fileInfo(oldPath);
    bool ok;
    QString newName = QInputDialog::getText(this, "Rename",
                                            "Enter new name:", QLineEdit::Normal,
                                            fileInfo.fileName(), &ok);
    if (ok && !newName.isEmpty()) {
      QFile file(oldPath);
      QString newPath = QDir(fileInfo.path()).filePath(newName);

      if (QFile::exists(newPath)) {
        QMessageBox::warning(this, "Error", "A file with that name already exists!");
        return;
      }

      if (!file.rename(newPath)) {
        QMessageBox::warning(this, "Error", "Failed to rename file!");
      }
    }
  }

private slots:
  void onItemDoubleClicked(const QModelIndex &index)
  {
    QString filePath = _fileModel->filePath(index);
    if (!_fileModel->isDir(index)) {
      emit fileSelected(filePath);
    }
  }

  void onNewFileClicked()
  {
    createNewFile(getCurrentPath());
  }

  void onRefreshClicked()
  {
    if (!_currentPath.isEmpty()) {
      setRootPath(_currentPath);
    }
  }

  void showContextMenu(const QPoint &pos)
  {
    QMenu contextMenu(this);
    QString currentPath = getCurrentPath();
    QFileInfo fileInfo(currentPath);

    _newFile = new QAction("New File", this);
    _newFolder  = new QAction("New Folder", this);
    _deleteItem = new QAction("Delete", this);
    _renameItem = new QAction("Rename", this);

    connect(_newFile, &QAction::triggered, [this, currentPath]() {
      createNewFile(currentPath);
    });

    connect(_newFolder, &QAction::triggered, [this, currentPath]() {
      createNewFolder(currentPath);
    });

    connect(_deleteItem, &QAction::triggered, [this, currentPath]() {
      deleteItem(currentPath);
    });

    connect(_renameItem, &QAction::triggered, [this, currentPath]() {
      renameItem(currentPath);
    });

    contextMenu.addAction(_newFile);
    contextMenu.addAction(_newFolder);
    contextMenu.addSeparator();

    // Só mostra Delete e Rename se um item estiver selecionado
    QModelIndex index = _treeView->currentIndex();
    if (index.isValid()) {
      contextMenu.addAction(_deleteItem);
      contextMenu.addAction(_renameItem);
    }

    contextMenu.exec(_treeView->mapToGlobal(pos));
  }

signals:
  void fileSelected(const QString &filePath);
  void fileCreated(const QString &filePath);

private:
  QTreeView *_treeView{nullptr};
  QFileSystemModel *_fileModel{nullptr};
  QString _currentPath;
  QAction* _newFile;
  QAction* _newFolder;
  QAction* _deleteItem;
  QAction* _renameItem;
};

#endif // FILEEXPLORERWIDGET_H
