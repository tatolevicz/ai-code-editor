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
#include <QLineEdit>
#include <QProcess>
#include "MgStyles.h"
#include "MagiaTheme.h"

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
    // Modern purple background using the new theme
    setStyleSheet(QString("FileExplorerWidget { background-color: #%1; border-right: 1px solid #%2; }")
                 .arg(mg::theme::Colors::SIDEBAR_BG, 6, 16, QChar('0'))
                 .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Header with modern styling
    auto headerWidget = new QWidget(this);
    headerWidget->setStyleSheet(QString("background-color: #%1; border-bottom: 1px solid #%2;")
                              .arg(mg::theme::Colors::SIDEBAR_BG, 6, 16, QChar('0'))
                              .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));
    auto headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(12, 12, 12, 12);
    headerLayout->setSpacing(12);

    // Title and controls layout
    auto titleControlsLayout = new QHBoxLayout();
    titleControlsLayout->setSpacing(8);

    // Logo and title
    auto titleLabel = new QLabel("EXPLORER", headerWidget);
    titleLabel->setStyleSheet(QString("QLabel { color: #%1; font-size: 12px; font-weight: bold; }")
                            .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0')));
    titleControlsLayout->addWidget(titleLabel);
    titleControlsLayout->addStretch();

    // New File button with modern styling
    auto newFileButton = new QPushButton("+", headerWidget);
    newFileButton->setFixedSize(24, 24);
    newFileButton->setToolTip("New File");
    newFileButton->setStyleSheet(
        QString("QPushButton { border: none; border-radius: 4px; background-color: #%1; color: #%2; font-weight: bold; }"
                "QPushButton:hover { background-color: #%3; }")
                .arg(mg::theme::Colors::PRIMARY, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0'))
    );
    titleControlsLayout->addWidget(newFileButton);

    // Refresh button with modern styling
    auto refreshButton = new QPushButton("⟳", headerWidget);
    refreshButton->setFixedSize(24, 24);
    refreshButton->setToolTip("Refresh");
    refreshButton->setStyleSheet(
        QString("QPushButton { border: none; border-radius: 4px; background-color: #%1; color: #%2; font-weight: bold; }"
                "QPushButton:hover { background-color: #%3; }")
                .arg(mg::theme::Colors::PRIMARY, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0'))
    );
    titleControlsLayout->addWidget(refreshButton);

    headerLayout->addLayout(titleControlsLayout);

    // Add search input with modern styling
    auto searchContainer = new QWidget(headerWidget);
    searchContainer->setStyleSheet(QString("QWidget { background-color: #%1; border: 1px solid #%2; border-radius: 8px; }")
                                .arg(mg::theme::Colors::BACKGROUND, 6, 16, QChar('0'))
                                .arg(mg::theme::Colors::BORDER, 6, 16, QChar('0')));
    
    auto searchLayout = new QHBoxLayout(searchContainer);
    searchLayout->setContentsMargins(8, 4, 8, 4);
    searchLayout->setSpacing(4);
    
    // Search icon label (using unicode for simplicity)
    auto searchIcon = new QLabel("🔍", searchContainer);
    searchIcon->setStyleSheet(QString("QLabel { color: #%1; background-color: transparent; }")
                           .arg(mg::theme::Colors::COMMENT, 6, 16, QChar('0')));
    searchLayout->addWidget(searchIcon);
    
    // Search input
    auto searchInput = new QLineEdit(searchContainer);
    searchInput->setPlaceholderText("Search files...");
    searchInput->setStyleSheet(QString("QLineEdit { background-color: transparent; border: none; color: #%1; }"
                                    "QLineEdit::placeholder { color: #%2; }")
                                    .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                                    .arg(mg::theme::Colors::COMMENT, 6, 16, QChar('0')));
    searchLayout->addWidget(searchInput);
    
    headerLayout->addWidget(searchContainer);
    
    mainLayout->addWidget(headerWidget);

    // TreeView with modern styling
    _treeView = new QTreeView(this);
    _treeView->setStyleSheet(
        QString("QTreeView { background-color: #%1; color: #%2; border: none; font-family: 'JetBrains Mono', monospace; padding: 8px; }"
                "QTreeView::item { height: 28px; padding-left: 4px; }"
                "QTreeView::item:hover { background: #%3; border-radius: 4px; }"
                "QTreeView::item:selected { background: #%4; color: #%5; border-radius: 4px; }"
                "QTreeView::branch { background: transparent; }"
                "QTreeView::branch:has-children:!has-siblings:closed,"
                "QTreeView::branch:closed:has-children:has-siblings { image: url(none); }"
                "QTreeView::branch:open:has-children:!has-siblings,"
                "QTreeView::branch:open:has-children:has-siblings { image: url(none); }")
                .arg(mg::theme::Colors::BACKGROUND, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::ACTIVE_ITEM, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::PRIMARY, 6, 16, QChar('0'))
                .arg(mg::theme::Colors::DEFAULT_TEXT, 6, 16, QChar('0'))
    );
    
    _fileModel = new QFileSystemModel(this);
    _fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    _treeView->setModel(_fileModel);

    // Hide unnecessary columns
    for (int i = 1; i < _fileModel->columnCount(); ++i) {
        _treeView->hideColumn(i);
    }

    _treeView->setHeaderHidden(true);
    _treeView->setAnimated(true);
    _treeView->setIndentation(20);
    _treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect signals
    connect(_treeView, &QTreeView::customContextMenuRequested, this, &FileExplorerWidget::showContextMenu);
    connect(_treeView, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        QString path = _fileModel->filePath(index);
        if (!_fileModel->isDir(index)) {
            emit fileSelected(path);
        }
    });
    connect(newFileButton, &QPushButton::clicked, this, &FileExplorerWidget::onNewFileClicked);
    connect(refreshButton, &QPushButton::clicked, this, &FileExplorerWidget::onRefreshClicked);

    mainLayout->addWidget(_treeView);

    // Conexões de sinais
    connect(_treeView, &QTreeView::doubleClicked,
            this, &FileExplorerWidget::onItemDoubleClicked);

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
    _openInFinder = new QAction("Open in Finder", this);

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
    
    connect(_openInFinder, &QAction::triggered, [this, currentPath]() {
      openInFinder(currentPath);
    });

    contextMenu.addAction(_newFile);
    contextMenu.addAction(_newFolder);
    contextMenu.addSeparator();

    // Só mostra Delete e Rename se um item estiver selecionado
    QModelIndex index = _treeView->currentIndex();
    if (index.isValid()) {
      contextMenu.addAction(_deleteItem);
      contextMenu.addAction(_renameItem);
      
      // Adicionar separador e opção Open in Finder
      contextMenu.addSeparator();
      contextMenu.addAction(_openInFinder);
    }

    contextMenu.exec(_treeView->mapToGlobal(pos));
  }

  void openInFinder(const QString& path)
  {
    QFileInfo fileInfo(path);
    QString targetPath;
    
    // Se for um arquivo, abrir o diretório que o contém
    if (fileInfo.isFile()) {
      targetPath = fileInfo.dir().path();
    } else {
      // Se for um diretório, abrir o próprio diretório
      targetPath = path;
    }
    
    // No macOS, usamos o comando "open" para abrir no Finder
    QProcess::startDetached("open", QStringList() << targetPath);
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
  QAction* _openInFinder;
};

#endif // FILEEXPLORERWIDGET_H
