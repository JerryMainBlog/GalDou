#include "EditorWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDir>
#include <QFileInfo>
#include <QKeySequence>
#include <QAction>
#include <QVBoxLayout>
#include <QLabel>
#include <QShortcut>

// ── 构造 ─────────────────────────────────────────────────────
EditorWindow::EditorWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("GalDou Editor");
    resize(1280, 800);
    setStyleSheet("background-color: #1E1E1E; color: #D4D4D4;");

    // ── 文件树 ────────────────────────────────────────────
    m_fileTree = new FileTreePanel(this);

    // ── Tab 编辑区 ─────────────────────────────────────────
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
        QTabBar::close-button { subcontrol-position: right; }
    )");

    // ── 错误面板 ───────────────────────────────────────────
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
    m_errorList->setMaximumHeight(140);

    auto* errorPanel = new QWidget(this);
    auto* epLayout = new QVBoxLayout(errorPanel);
    epLayout->setContentsMargins(0, 0, 0, 0);
    epLayout->setSpacing(0);
    epLayout->addWidget(errorHeader);
    epLayout->addWidget(m_errorList);

    // ── 右侧垂直分割：编辑区 + 错误面板 ────────────────────
    m_rightSplitter = new QSplitter(Qt::Vertical, this);
    m_rightSplitter->addWidget(m_tabs);
    m_rightSplitter->addWidget(errorPanel);
    m_rightSplitter->setStretchFactor(0, 4);
    m_rightSplitter->setStretchFactor(1, 1);
    m_rightSplitter->setStyleSheet("QSplitter::handle { background: #333333; }");

    // ── 主水平分割：左文件树 + 右编辑区 ────────────────────
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->addWidget(m_fileTree);
    m_mainSplitter->addWidget(m_rightSplitter);
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setSizes({ 220, 1060 });
    m_mainSplitter->setStyleSheet("QSplitter::handle { background: #333333; width: 1px; }");

    setCentralWidget(m_mainSplitter);

    setupMenuBar();
    setupStatusBar();

    // ── 信号连接 ──────────────────────────────────────────
    connect(m_fileTree, &FileTreePanel::fileOpenRequested,
            this, &EditorWindow::onFileOpenRequested);
    connect(m_tabs, &QTabWidget::tabCloseRequested,
            this, &EditorWindow::onTabCloseRequested);
    connect(m_errorList, &QListWidget::itemClicked,
            this, &EditorWindow::onErrorItemClicked);

    // 默认打开示例目录
    QString defaultDir = QDir::currentPath() + "/scripts";
    if (QDir(defaultDir).exists())
        m_fileTree->setRootPath(defaultDir);
}

// ── 菜单栏 ────────────────────────────────────────────────────
void EditorWindow::setupMenuBar() {
    auto* mb = menuBar();
    mb->setStyleSheet(R"(
        QMenuBar { background:#333333; color:#CCCCCC; }
        QMenuBar::item:selected { background:#094771; }
        QMenu { background:#252526; color:#CCCCCC; border: 1px solid #454545; }
        QMenu::item:selected { background:#094771; }
    )");

    // 文件菜单
    auto* fileMenu = mb->addMenu("文件(&F)");

    auto* actNew = fileMenu->addAction("新建脚本\tCtrl+N");
    connect(actNew, &QAction::triggered, m_fileTree, &FileTreePanel::newFileRequested);

    auto* actOpen = fileMenu->addAction("打开文件(&O)\tCtrl+O");
    connect(actOpen, &QAction::triggered, this, [this](){
        QString path = QFileDialog::getOpenFileName(
            this, "打开脚本", QDir::homePath(), "GalDou 脚本 (*.gal);;所有文件 (*)");
        if (!path.isEmpty()) openFile(path);
    });

    auto* actOpenDir = fileMenu->addAction("打开文件夹(&D)");
    connect(actOpenDir, &QAction::triggered, this, [this](){
        QString dir = QFileDialog::getExistingDirectory(
            this, "打开项目文件夹", QDir::homePath());
        if (!dir.isEmpty()) m_fileTree->setRootPath(dir);
    });

    fileMenu->addSeparator();

    auto* actSave = fileMenu->addAction("保存(&S)\tCtrl+S");
    connect(actSave, &QAction::triggered, this, [this](){
        if (auto* ed = currentEditor()) ed->saveFile();
    });

    fileMenu->addSeparator();
    auto* actExit = fileMenu->addAction("退出(&Q)\tAlt+F4");
    connect(actExit, &QAction::triggered, this, &QWidget::close);

    // 快捷键
    new QShortcut(QKeySequence("Ctrl+S"), this, [this](){
        if (auto* ed = currentEditor()) ed->saveFile();
    });
    new QShortcut(QKeySequence("Ctrl+W"), this, [this](){
        int idx = m_tabs->currentIndex();
        if (idx >= 0) onTabCloseRequested(idx);
    });
    new QShortcut(QKeySequence("Ctrl+O"), this, [this](){
        QString path = QFileDialog::getOpenFileName(
            this, "打开脚本", QDir::homePath(), "GalDou 脚本 (*.gal)");
        if (!path.isEmpty()) openFile(path);
    });

    // 帮助菜单
    auto* helpMenu = mb->addMenu("帮助(&H)");
    auto* actAbout = helpMenu->addAction("关于 GalDou Editor");
    connect(actAbout, &QAction::triggered, this, [this](){
        QMessageBox::about(this, "关于",
            "<b>GalDou Editor v0.1</b><br>"
            "Galgame 剧本开发套件<br><br>"
            "脚本语法参考 examples/ 目录中的示例文件。");
    });
}

void EditorWindow::setupStatusBar() {
    m_statusLabel = new QLabel("就绪", this);
    m_statusLabel->setStyleSheet("color: #858585; padding: 0 8px;");
    statusBar()->addPermanentWidget(m_statusLabel);
    statusBar()->setStyleSheet("background:#007ACC; color:white;");
}

// ── 打开文件 ──────────────────────────────────────────────────
void EditorWindow::openFile(const QString& path) {
    // 已打开则切换到该 Tab
    int existing = findTab(path);
    if (existing >= 0) {
        m_tabs->setCurrentIndex(existing);
        return;
    }

    auto* editor = new ScriptEditor(this);
    editor->openFile(path);

    QString fileName = QFileInfo(path).fileName();
    int idx = m_tabs->addTab(editor, fileName);
    m_tabs->setCurrentIndex(idx);

    // 连接编辑器信号
    connect(editor, &ScriptEditor::validationDone,
            this, &EditorWindow::onValidationDone);
    connect(editor, &QPlainTextEdit::cursorPositionChanged,
            this, &EditorWindow::onCursorPositionChanged);
    connect(editor, &QPlainTextEdit::modificationChanged,
            this, [this, idx](bool){ updateTabTitle(idx); });

    onCursorPositionChanged();
}

// ── Tab 关闭 ──────────────────────────────────────────────────
void EditorWindow::onTabCloseRequested(int index) {
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

// ── 错误面板更新 ──────────────────────────────────────────────
void EditorWindow::onValidationDone(const QVector<ValidationError>& errors) {
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
    statusBar()->showMessage(
        errors.isEmpty() ? "已保存" : QString("保存完成（%1 个错误）").arg(errors.size()), 3000);
}

// ── 点击错误跳转 ──────────────────────────────────────────────
void EditorWindow::onErrorItemClicked(QListWidgetItem* item) {
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

// ── 状态栏行列更新 ─────────────────────────────────────────────
void EditorWindow::onCursorPositionChanged() {
    auto* ed = currentEditor();
    if (!ed) return;
    auto c = ed->textCursor();
    m_statusLabel->setText(
        QString("行 %1，列 %2").arg(c.blockNumber() + 1).arg(c.columnNumber() + 1));
}

void EditorWindow::onTabModified() { updateTabTitle(m_tabs->currentIndex()); }

// ── 辅助函数 ──────────────────────────────────────────────────
ScriptEditor* EditorWindow::currentEditor() const {
    return qobject_cast<ScriptEditor*>(m_tabs->currentWidget());
}

int EditorWindow::findTab(const QString& path) const {
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto* ed = qobject_cast<ScriptEditor*>(m_tabs->widget(i));
        if (ed && ed->currentPath() == path) return i;
    }
    return -1;
}

void EditorWindow::updateTabTitle(int index) {
    auto* ed = qobject_cast<ScriptEditor*>(m_tabs->widget(index));
    if (!ed) return;
    QString name = QFileInfo(ed->currentPath()).fileName();
    m_tabs->setTabText(index, ed->isModified() ? "● " + name : name);
}

void EditorWindow::onFileOpenRequested(const QString& path) { openFile(path); }

// ── 关闭事件（检查未保存文件） ─────────────────────────────────
void EditorWindow::closeEvent(QCloseEvent* e) {
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto* ed = qobject_cast<ScriptEditor*>(m_tabs->widget(i));
        if (ed && ed->isModified()) {
            auto btn = QMessageBox::question(
                this, "未保存的修改",
                "有文件尚未保存，确定退出吗？",
                QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel);
            if (btn == QMessageBox::Cancel) { e->ignore(); return; }
            if (btn == QMessageBox::SaveAll) {
                for (int j = 0; j < m_tabs->count(); ++j) {
                    auto* e2 = qobject_cast<ScriptEditor*>(m_tabs->widget(j));
                    if (e2 && e2->isModified()) e2->saveFile();
                }
            }
            break;
        }
    }
    e->accept();
}
