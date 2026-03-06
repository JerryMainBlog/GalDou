#pragma once
#include <QPlainTextEdit>
#include <QWidget>
#include "GalHighlighter.h"
#include "ScriptValidator.h"

class LineNumberArea;

/*
 * ScriptEditor
 * 基于 QPlainTextEdit 的增强编辑器：
 *  - 左侧行号区
 *  - .gal 语法高亮
 *  - 保存时自动校验，错误行用红色底色标记
 *  - Ctrl+S 快捷键保存
 *  - 自动补全常用命令（Tab 键）
 */
class ScriptEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget* parent = nullptr);

    void openFile(const QString& path);
    void saveFile();
    bool isModified() const;

    QString currentPath() const { return m_path; }

signals:
    void fileChanged(const QString& path);     // 文件保存成功
    void validationDone(QVector<ValidationError> errors);

protected:
    void resizeEvent(QResizeEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect& rect, int dy);
    void onTextChanged();

private:
    friend class LineNumberArea;
    int  lineNumberAreaWidth() const;
    void paintLineNumbers(QPaintEvent*);
    void markErrors(const QVector<ValidationError>& errors);

    QString            m_path;
    GalHighlighter*    m_highlighter;
    ScriptValidator*   m_validator;
    QWidget*           m_lineArea;

    // 错误行集合（行号 1-indexed）
    QSet<int>          m_errorLines;
};

// ── 行号绘制区（辅助 Widget）────────────────────────────────
class LineNumberArea : public QWidget {
public:
    explicit LineNumberArea(ScriptEditor* editor) : QWidget(editor), m_editor(editor) {}
    QSize sizeHint() const override {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent* e) override { m_editor->paintLineNumbers(e); }
private:
    ScriptEditor* m_editor;
};
