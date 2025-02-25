#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeView>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QDir>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QLabel>

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
        
        auto mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Create header
        auto headerLayout = new QHBoxLayout();
        headerLayout->setContentsMargins(8, 4, 8, 4);

        auto titleLabel = new QLabel("EXPLORER", this);
        titleLabel->setStyleSheet("QLabel { color: #6c757d; font-size: 11px; font-weight: bold; }");
        headerLayout->addWidget(titleLabel);

        // Add header buttons
        auto newFileButton = new QPushButton("+", this);
        newFileButton->setFixedSize(20, 20);
        newFileButton->setStyleSheet("QPushButton { border: 1px solid #ddd; border-radius: 3px; background: transparent; }"
                                   "QPushButton:hover { background: #e9ecef; }");
        headerLayout->addWidget(newFileButton);

        auto refreshButton = new QPushButton("⟳", this);
        refreshButton->setFixedSize(20, 20);
        refreshButton->setStyleSheet("QPushButton { border: 1px solid #ddd; border-radius: 3px; background: transparent; }"
                                   "QPushButton:hover { background: #e9ecef; }");
        headerLayout->addWidget(refreshButton);

        mainLayout->addLayout(headerLayout);

        // Create tree view
        _treeView = new QTreeView(this);
        _treeView->setStyleSheet(
            "QTreeView { background-color: #ffffff; border: none; }"
            "QTreeView::item { height: 24px; }"
            "QTreeView::item:hover { background: #f8f9fa; }"
            "QTreeView::item:selected { background: #e9ecef; color: black; }"
        );
        
        _fileModel = new QFileSystemModel(this);
        _fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
        _fileModel->setNameFilterDisables(false);
        
        _treeView->setModel(_fileModel);
        _treeView->setHeaderHidden(true);
        _treeView->setColumnHidden(1, true);  // Size column
        _treeView->setColumnHidden(2, true);  // Type column
        _treeView->setColumnHidden(3, true);  // Date modified column
        
        mainLayout->addWidget(_treeView);

        // Context menu
        _treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(_treeView, &QTreeView::customContextMenuRequested,
                this, &FileExplorerWidget::showContextMenu);

        // Connect signals
        connect(_treeView, &QTreeView::doubleClicked,
                this, &FileExplorerWidget::onItemDoubleClicked);
        connect(newFileButton, &QPushButton::clicked,
                this, &FileExplorerWidget::onNewFileClicked);
        connect(refreshButton, &QPushButton::clicked,
                this, &FileExplorerWidget::onRefreshClicked);

        setLayout(mainLayout);
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
        // TODO: Implement new file dialog
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
        
        QAction newFile("New File", this);
        QAction newFolder("New Folder", this);
        QAction deleteItem("Delete", this);
        QAction renameItem("Rename", this);
        
        contextMenu.addAction(&newFile);
        contextMenu.addAction(&newFolder);
        contextMenu.addSeparator();
        contextMenu.addAction(&deleteItem);
        contextMenu.addAction(&renameItem);
        
        // TODO: Connect actions to appropriate slots
        
        contextMenu.exec(_treeView->mapToGlobal(pos));
    }

signals:
    void fileSelected(const QString &filePath);

private:
    QTreeView *_treeView{nullptr};
    QFileSystemModel *_fileModel{nullptr};
    QString _currentPath;
};

#endif // FILEEXPLORERWIDGET_H
