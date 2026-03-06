#include "EditorPanel.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QLabel>

EditorPanel::EditorPanel(QWidget* parent) : QWidget(parent) {
    setMinimumWidth(350);

    // ── 文件树 ────────────────────────────────────────
    m_fileTree = new FileTreePanel(this);

    // ── Tab 编辑区 ─────────────────────────────────────
    m_tabs = new QTabWidget(this);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setStyleSheet(R"(
        QTabWidget::pane    { border: none; background: #1E1E1E; }
        QTabBar::tab {
            background: #2D2D2D; color: #969696;
            padding: 6px 14px; border: none;
            border-right: 1px solid #1E1E1E;
            min-width: 100px;
        }
        QTabBar::tab:selected { background: #1E1E1E; color: #FFFFFF; border-top: 1px solid #007ACC; }
        QTabBar::tab:hover    { background: #252526; color: #CCCCCC; }
    )");

    // ── 错误面板 ───────────────────────────────────────
    auto* errorHeader = new QLabel("  ⚠ 错误与警告", this);
    errorHeader->setStyleSheet(
        "background:#252526; color:#CCCCCC; padding:4px 8px; font-size:12px;");

    m_errorList = new QListWidget(this);
    m_errorList->setStyleSheet(R"(
        QListWidget {
            background:#1E1E1E; color:#F44747;
            border:none; font-size:12px;
        }
        QListWidget::item:hover    { background:#2A2D2E; }
        QListWidget::item:selected { background:#094771; color:white; }
    )");
    m_errorList->setMaximumHeight(120);

    auto* errorPanel = new QWidget(this);
    auto* epLayout = new QVBoxLayout(errorPanel);
    epLayout->setContentsMargins(0, 0, 0, 0);
    epLayout->setSpacing(0);
    epLayout->addWidget(errorHeader);
    epLayout->addWidget(m_errorList);

    // ── 右侧垂直分割：编辑区 + 错误面板 ────────────────
    auto* editSplitter = new QSplitter(Qt::Vertical, this);
    editSplitter->addWidget(m_tabs);
    editSplitter->addWidget(errorPanel);
    editSplitter->setStretchFactor(0, 4);
    editSplitter->setStretchFactor(1, 1);
    editSplitter->setStyleSheet("QSplitter::handle { background: #333333; }");

    // ── 主垂直分割：文件树 + 编辑区 ────────────────────
    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->addWidget(m_fileTree);
    m_splitter->addWidget(editSplitter);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 3);
    m_splitter->setStyleSheet("QSplitter::handle { background: #333333; height: 1px; }");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_splitter);

    // ── 信号连接 ──────────────────────────────────────
    connect(m_fileTree, &FileTreePanel::fileOpenRequested,
            this, &EditorPanel::onFileOpenRequested);
    connect(m_tabs, &QTabWidget::tabCloseRequested,
            this, &EditorPanel::onTabCloseRequested);
    connect(m_errorList, &QListWidget::itemClicked,
            this, &EditorPanel::onErrorItemClicked);
}

// ── 打开文件 ──────────────────────────────────────────────────
void EditorPanel::openFile(const QString& path) {
    int existing = findTab(path);
    if (existing >= 0) { m_tabs->setCurrentIndex(existing); return; }

    auto* editor = new ScriptEditor(this);
    editor->openFile(path);

    QString fileName = QFileInfo(path).fileName();
    int idx = m_tabs->addTab(editor, fileName);
    m_tabs->setCurrentIndex(idx);

    connect(editor, &ScriptEditor::validationDone,
            this, &EditorPanel::onValidationDone);
    connect(editor, &QPlainTextEdit::cursorPositionChanged,
            this, &EditorPanel::onCursorPositionChanged);
    connect(editor, &QPlainTextEdit::modificationChanged,
            this, [this, idx](bool){ updateTabTitle(idx); });
    connect(editor, &ScriptEditor::fileChanged,
            this, &EditorPanel::fileSaved);
}

void EditorPanel::saveCurrentFile() {
    if (auto* ed = currentEditor()) ed->saveFile();
}

QString EditorPanel::currentFilePath() const {
    if (auto* ed = currentEditor()) return ed->currentPath();
    return {};
}

// ── Tab 关闭 ──────────────────────────────────────────────────
void EditorPanel::onTabCloseRequested(int index) {
    auto* editor = qobject_cast<ScriptEditor*>(m_tabs->widget(index));
    if (!editor) return;

    if (editor->isModified()) {
        auto btn = QMessageBox::question(
            this, "未保存的修改",
            QString("文件 \"%1\" 有未保存的修改，确定关闭吗？")
                .arg(QFileInfo(editor->currentPath()).fileName()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (btn == QMessageBox::Cancel) return;
        if (btn == QMessageBox::Save) editor->saveFile();
    }
    m_tabs->removeTab(index);
    delete editor;
}

// ── 错误面板 ──────────────────────────────────────────────────
void EditorPanel::onValidationDone(const QVector<ValidationError>& errors) {
    m_errorList->clear();
    for (const auto& e : errors) {
        auto* item = new QListWidgetItem(
            QString("  第 %1 行 — %2").arg(e.line).arg(e.message));
        item->setData(Qt::UserRole, e.line);
        m_errorList->addItem(item);
    }
    if (errors.isEmpty()) {
        auto* ok = new QListWidgetItem("  ✔ 无错误");
        ok->setForeground(QColor("#4EC9B0"));
        m_errorList->addItem(ok);
    }
}

void EditorPanel::onErrorItemClicked(QListWidgetItem* item) {
    int line = item->data(Qt::UserRole).toInt();
    if (line <= 0) return;
    auto* ed = currentEditor();
    if (!ed) return;
    QTextBlock block = ed->document()->findBlockByLineNumber(line - 1);
    QTextCursor c(block);
    ed->setTextCursor(c);
    ed->centerCursor();
    ed->setFocus();
}

void EditorPanel::onCursorPositionChanged() {
    auto* ed = currentEditor();
    if (!ed) return;
    auto c = ed->textCursor();
    emit cursorInfoChanged(
        QString("行 %1，列 %2").arg(c.blockNumber() + 1).arg(c.columnNumber() + 1));
}

void EditorPanel::onFileOpenRequested(const QString& path) { openFile(path); }

// ── 辅助 ──────────────────────────────────────────────────────
ScriptEditor* EditorPanel::currentEditor() const {
    return qobject_cast<ScriptEditor*>(m_tabs->currentWidget());
}

int EditorPanel::findTab(const QString& path) const {
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto* ed = qobject_cast<ScriptEditor*>(m_tabs->widget(i));
        if (ed && ed->currentPath() == path) return i;
    }
    return -1;
}

void EditorPanel::updateTabTitle(int index) {
    auto* ed = qobject_cast<ScriptEditor*>(m_tabs->widget(index));
    if (!ed) return;
    QString name = QFileInfo(ed->currentPath()).fileName();
    m_tabs->setTabText(index, ed->isModified() ? "● " + name : name);
}
