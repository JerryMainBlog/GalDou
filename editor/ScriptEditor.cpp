#include "ScriptEditor.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextBlock>

// ── 构造 ─────────────────────────────────────────────────────
ScriptEditor::ScriptEditor(QWidget* parent)
    : QPlainTextEdit(parent)
{
    // 外观
    setFont(QFont("Consolas", 12));
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #1E1E1E;
            color: #D4D4D4;
            border: none;
            selection-background-color: #264F78;
        }
    )");

    // 行号区
    m_lineArea = new LineNumberArea(this);

    // 语法高亮
    m_highlighter = new GalHighlighter(document());

    // 校验器
    m_validator = new ScriptValidator(this);

    // 连接信号
    connect(this, &ScriptEditor::blockCountChanged,
            this, &ScriptEditor::updateLineNumberAreaWidth);
    connect(this, &ScriptEditor::updateRequest,
            this, &ScriptEditor::updateLineNumberArea);
    connect(this, &ScriptEditor::textChanged,
            this, &ScriptEditor::onTextChanged);

    updateLineNumberAreaWidth();
}

// ── 文件操作 ──────────────────────────────────────────────────
void ScriptEditor::openFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);
    setPlainText(in.readAll());

    m_path = path;
    document()->setModified(false);
    emit fileChanged(path);
}

void ScriptEditor::saveFile() {
    if (m_path.isEmpty()) return;

    QFile f(m_path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "保存失败", "无法写入文件：" + m_path);
        return;
    }

    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << toPlainText();
    document()->setModified(false);

    // 保存后校验
    auto errors = m_validator->validate(toPlainText());
    markErrors(errors);
    emit validationDone(errors);
    emit fileChanged(m_path);
}

bool ScriptEditor::isModified() const {
    return document()->isModified();
}

// ── 键盘事件 ──────────────────────────────────────────────────
void ScriptEditor::keyPressEvent(QKeyEvent* e) {
    // Ctrl+S 保存
    if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_S) {
        saveFile();
        return;
    }

    // Tab → 插入 4 个空格
    if (e->key() == Qt::Key_Tab) {
        insertPlainText("    ");
        return;
    }

    // Enter → 自动缩进（继承当前行缩进）
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextCursor c = textCursor();
        QString curLine = c.block().text();
        QString indent;
        for (QChar ch : curLine) {
            if (ch == ' ') indent += ' ';
            else break;
        }
        QPlainTextEdit::keyPressEvent(e);
        insertPlainText(indent);
        return;
    }

    QPlainTextEdit::keyPressEvent(e);
}

// ── 文本变化（轻量实时检查） ──────────────────────────────────
void ScriptEditor::onTextChanged() {
    // 只做轻量错误行标记，不向外发信号（避免频繁信号）
    auto errors = m_validator->validate(toPlainText());
    markErrors(errors);
}

// ── 错误行底色标记 ─────────────────────────────────────────────
void ScriptEditor::markErrors(const QVector<ValidationError>& errors) {
    m_errorLines.clear();
    for (const auto& e : errors)
        m_errorLines.insert(e.line);

    QList<QTextEdit::ExtraSelection> selections;
    QTextCharFormat errorFmt;
    errorFmt.setBackground(QColor("#3D1515")); // 深红底色

    QTextBlock block = document()->begin();
    int lineNo = 1;
    while (block.isValid()) {
        if (m_errorLines.contains(lineNo)) {
            QTextEdit::ExtraSelection sel;
            sel.format = errorFmt;
            sel.format.setProperty(QTextFormat::FullWidthSelection, true);
            sel.cursor = QTextCursor(block);
            sel.cursor.clearSelection();
            selections.append(sel);
        }
        block = block.next();
        lineNo++;
    }
    setExtraSelections(selections);
    m_lineArea->update(); // 重绘行号（错误行显示 ✕）
}

// ── 行号区 ─────────────────────────────────────────────────────
int ScriptEditor::lineNumberAreaWidth() const {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) { max /= 10; digits++; }
    return 8 + fontMetrics().horizontalAdvance('9') * digits + 16;
}

void ScriptEditor::updateLineNumberAreaWidth() {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ScriptEditor::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy) m_lineArea->scroll(0, dy);
    else    m_lineArea->update(0, rect.y(), m_lineArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void ScriptEditor::resizeEvent(QResizeEvent* e) {
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    m_lineArea->setGeometry(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height());
}

void ScriptEditor::paintLineNumbers(QPaintEvent* e) {
    QPainter p(m_lineArea);
    p.fillRect(e->rect(), QColor("#252526"));

    QTextBlock block = firstVisibleBlock();
    int blockNo      = block.blockNumber();
    int top          = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom       = top + qRound(blockBoundingRect(block).height());
    int lineH        = fontMetrics().height();

    while (block.isValid() && top <= e->rect().bottom()) {
        if (block.isVisible() && bottom >= e->rect().top()) {
            int lineNo = blockNo + 1;
            bool hasError = m_errorLines.contains(lineNo);

            // 错误行背景
            if (hasError)
                p.fillRect(0, top, m_lineArea->width(), lineH, QColor("#5A1A1A"));

            // 行号文字
            p.setPen(hasError ? QColor("#F44747") : QColor("#858585"));
            p.setFont(font());
            p.drawText(0, top, m_lineArea->width() - 4, lineH,
                       Qt::AlignRight | Qt::AlignVCenter,
                       QString::number(lineNo));
        }
        block  = block.next();
        top    = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNo;
    }
}
