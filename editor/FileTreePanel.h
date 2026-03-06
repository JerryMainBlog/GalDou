#pragma once
#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QToolBar>
#include <QLabel>

/*
 * FileTreePanel
 * 左侧文件树：
 *  - 显示项目根目录下的所有 .gal 文件
 *  - 双击文件 → 发出 fileOpenRequested(path)
 *  - 工具栏：新建文件、打开文件夹
 */
class FileTreePanel : public QWidget {
    Q_OBJECT
public:
    explicit FileTreePanel(QWidget* parent = nullptr);

    void setRootPath(const QString& path);

signals:
    void fileOpenRequested(const QString& path);
    void newFileRequested();

private slots:
    void onItemDoubleClicked(const QModelIndex& index);
    void onOpenFolder();
    void onNewFile();

private:
    QTreeView*         m_tree;
    QFileSystemModel*  m_model;
    QLabel*            m_rootLabel;
};
