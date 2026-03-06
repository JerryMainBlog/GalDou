#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QLabel>
#include <QListWidget>
#include "FileTreePanel.h"
#include "ScriptEditor.h"

/*
 * EditorWindow — GalDou 剧本编辑器主窗口
 *
 * 布局：
 *  ┌──────────┬─────────────────────────────────┐
 *  │ 文件树   │  Tab 编辑区（可多文件）           │
 *  │ (左侧)   │  [语法高亮 + 行号 + 错误标记]     │
 *  │          ├─────────────────────────────────┤
 *  │          │  底部错误面板                    │
 *  └──────────┴─────────────────────────────────┘
 *
 * 功能：
 *  - 多文件 Tab 同时编辑
 *  - Tab 标题显示修改状态 (*)
 *  - 底部错误列表，点击跳转到对应行
 *  - 状态栏显示当前行列
 *  - Ctrl+S 保存，Ctrl+W 关闭当前 Tab
 */
class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit EditorWindow(QWidget* parent = nullptr);

    void openFile(const QString& path);

protected:
    void closeEvent(QCloseEvent*) override;

private slots:
    void onFileOpenRequested(const QString& path);
    void onTabCloseRequested(int index);
    void onValidationDone(const QVector<ValidationError>& errors);
    void onErrorItemClicked(QListWidgetItem* item);
    void onCursorPositionChanged();
    void onTabModified();

private:
    ScriptEditor* currentEditor() const;
    int           findTab(const QString& path) const;
    void          updateTabTitle(int index);
    void          setupMenuBar();
    void          setupStatusBar();

    FileTreePanel* m_fileTree;
    QTabWidget*    m_tabs;
    QListWidget*   m_errorList;
    QLabel*        m_statusLabel;
    QSplitter*     m_mainSplitter;
    QSplitter*     m_rightSplitter;
};
