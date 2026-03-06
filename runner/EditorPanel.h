#pragma once
#include <QWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QLabel>
#include <QSplitter>

#include "FileTreePanel.h"
#include "ScriptEditor.h"

/*
 * EditorPanel
 * 左侧编辑面板，可嵌入到任意父窗口：
 *  ┌─────────────────────────────┐
 *  │ 文件树                       │
 *  ├─────────────────────────────┤
 *  │ Tab 编辑区（多文件）          │
 *  ├─────────────────────────────┤
 *  │ 错误面板                     │
 *  └─────────────────────────────┘
 */
class EditorPanel : public QWidget {
    Q_OBJECT
public:
    explicit EditorPanel(QWidget* parent = nullptr);

    void openFile(const QString& path);
    void saveCurrentFile();
    QString currentFilePath() const;

signals:
    void fileSaved(const QString& path);       // 文件保存成功
    void cursorInfoChanged(const QString& info); // 行列信息

private slots:
    void onFileOpenRequested(const QString& path);
    void onTabCloseRequested(int index);
    void onValidationDone(const QVector<ValidationError>& errors);
    void onErrorItemClicked(QListWidgetItem* item);
    void onCursorPositionChanged();

private:
    ScriptEditor* currentEditor() const;
    int           findTab(const QString& path) const;
    void          updateTabTitle(int index);

    FileTreePanel* m_fileTree;
    QTabWidget*    m_tabs;
    QListWidget*   m_errorList;
    QSplitter*     m_splitter;
};
