#include "FileTreePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>

FileTreePanel::FileTreePanel(QWidget* parent) : QWidget(parent) {
    setMinimumWidth(200);
    setMaximumWidth(320);
    setStyleSheet("background-color: #252526; color: #CCCCCC;");

    // ── 顶部标题 + 工具栏 ──────────────────────────────────
    m_rootLabel = new QLabel("EXPLORER", this);
    m_rootLabel->setStyleSheet(
        "color: #BBBBBB; font-size: 11px; font-weight: bold;"
        "padding: 8px 12px 4px 12px; letter-spacing: 1px;"
    );

    auto* btnNew = new QPushButton("＋ 新建", this);
    btnNew->setStyleSheet(R"(
        QPushButton {
            background: #0E639C; color: white;
            border: none; border-radius: 3px;
            padding: 3px 10px; font-size: 12px;
        }
        QPushButton:hover { background: #1177BB; }
    )");

    auto* btnOpen = new QPushButton("📂 打开", this);
    btnOpen->setStyleSheet(R"(
        QPushButton {
            background: #3C3C3C; color: white;
            border: none; border-radius: 3px;
            padding: 3px 10px; font-size: 12px;
        }
        QPushButton:hover { background: #505050; }
    )");

    auto* btnBar = new QHBoxLayout();
    btnBar->setContentsMargins(8, 0, 8, 4);
    btnBar->addWidget(btnNew);
    btnBar->addWidget(btnOpen);
    btnBar->addStretch();

    // ── 文件树 ────────────────────────────────────────────
    m_model = new QFileSystemModel(this);
    m_model->setNameFilters({ "*.gal" });
    m_model->setNameFilterDisables(false);  // 隐藏非 .gal 文件

    m_tree = new QTreeView(this);
    m_tree->setModel(m_model);
    m_tree->setHeaderHidden(true);
    // 只显示第一列（文件名）
    m_tree->hideColumn(1);
    m_tree->hideColumn(2);
    m_tree->hideColumn(3);
    m_tree->setStyleSheet(R"(
        QTreeView {
            background: #252526;
            color: #CCCCCC;
            border: none;
            font-size: 13px;
        }
        QTreeView::item:hover       { background: #2A2D2E; }
        QTreeView::item:selected    { background: #094771; color: white; }
        QTreeView::branch:has-children:closed { image: url(none); }
    )");
    m_tree->setIndentation(16);

    // ── 布局 ─────────────────────────────────────────────
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_rootLabel);
    layout->addLayout(btnBar);
    layout->addWidget(m_tree);

    // ── 信号连接 ──────────────────────────────────────────
    connect(m_tree, &QTreeView::doubleClicked, this, &FileTreePanel::onItemDoubleClicked);
    connect(btnOpen, &QPushButton::clicked, this, &FileTreePanel::onOpenFolder);
    connect(btnNew,  &QPushButton::clicked, this, &FileTreePanel::onNewFile);
}

void FileTreePanel::setRootPath(const QString& path) {
    QModelIndex root = m_model->setRootPath(path);
    m_tree->setRootIndex(root);

    QString dirName = QDir(path).dirName();
    m_rootLabel->setText("EXPLORER — " + dirName.toUpper());
}

void FileTreePanel::onItemDoubleClicked(const QModelIndex& index) {
    QString path = m_model->filePath(index);
    if (!m_model->isDir(index))
        emit fileOpenRequested(path);
}

void FileTreePanel::onOpenFolder() {
    QString dir = QFileDialog::getExistingDirectory(
        this, "打开项目文件夹", QDir::homePath());
    if (!dir.isEmpty())
        setRootPath(dir);
}

void FileTreePanel::onNewFile() {
    // 获取当前根目录
    QString rootPath = m_model->rootPath();
    if (rootPath.isEmpty()) {
        QMessageBox::information(this, "提示", "请先打开一个项目文件夹");
        return;
    }

    bool ok;
    QString name = QInputDialog::getText(
        this, "新建脚本", "文件名（不含扩展名）:", QLineEdit::Normal, "scene1", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QString path = rootPath + "/" + name.trimmed() + ".gal";
    QFile f(path);
    if (f.exists()) {
        QMessageBox::warning(this, "已存在", "文件已存在：" + path);
        return;
    }

    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // 写入模板内容
        QTextStream out(&f);
        out.setEncoding(QStringConverter::Utf8);
        out << "# " << name << ".gal — 由 GalDou Editor 创建\n\n"
            << "[scene] start\n"
            << "[bg] \"backgrounds/bg_room.png\"\n"
            << "[bgm] \"audio/theme.mp3\"\n"
            << "[show] \"characters/char_a.png\" center\n"
            << "[say] \"角色A\" \"你好，这是第一句对话。\"\n"
            << "[say] \"\" \"（旁白：故事从这里开始...）\"\n"
            << "[end]\n";
        f.close();
        emit fileOpenRequested(path);
    }
}
