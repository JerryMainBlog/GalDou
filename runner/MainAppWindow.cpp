#include "MainAppWindow.h"
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QDir>
#include <QFileInfo>
#include <QShortcut>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

MainAppWindow::MainAppWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("GalDou — 开发套件");
    resize(1600, 900);
    setStyleSheet("background-color: #1E1E1E; color: #D4D4D4;");

    m_editorPanel = new EditorPanel(this);

    setupPreviewPanel();

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_editorPanel);
    splitter->addWidget(m_previewWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({ 700, 900 });
    splitter->setStyleSheet("QSplitter::handle { background: #444444; width: 2px; }");
    setCentralWidget(splitter);

    setupToolBar();
    setupMenuBar();
    setupStatusBar();

    connect(m_editorPanel, &EditorPanel::fileSaved,
            this, &MainAppWindow::onScriptSaved);
    connect(m_engine, &GameEngine::gameEnded,
            this, &MainAppWindow::onGameEnded);

    new QShortcut(QKeySequence("Ctrl+S"), this, [this](){
        m_editorPanel->saveCurrentFile();
    });
    new QShortcut(QKeySequence("F5"), this, [this](){ onRun(); });
    new QShortcut(QKeySequence("F6"), this, [this](){ onStop(); });
}

void MainAppWindow::setupPreviewPanel() {
    m_scene  = new RenderScene(this);
    m_audio  = new AudioManager(this);
    m_engine = new GameEngine(this);

    m_view = new QGraphicsView(m_scene);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setFrameStyle(QFrame::NoFrame);
    m_view->setStyleSheet("background: #000000;");
    m_view->installEventFilter(this);

    m_dialog = new DialogBox(m_view);
    m_choice = new ChoiceWidget(m_view);

    m_previewWidget = new QWidget(this);
    m_previewWidget->setMinimumWidth(400);

    auto* layout = new QVBoxLayout(m_previewWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* header = new QLabel("  ▶  预览区  （保存脚本后点击运行）", m_previewWidget);
    header->setStyleSheet(
        "background:#252526; color:#858585; padding:6px 12px; font-size:12px;"
        "border-bottom:1px solid #333333;");
    header->setFixedHeight(30);

    layout->addWidget(header);
    layout->addWidget(m_view);
}

void MainAppWindow::setupToolBar() {
    auto* tb = addToolBar("主工具栏");
    tb->setMovable(false);
    tb->setStyleSheet(R"(
        QToolBar {
            background: #2D2D2D;
            border-bottom: 1px solid #444;
            padding: 2px 8px;
            spacing: 4px;
        }
        QToolButton {
            color: #CCCCCC; background: transparent;
            border: none; border-radius: 3px;
            padding: 4px 12px; font-size: 13px;
        }
        QToolButton:hover    { background: #484848; }
        QToolButton:pressed  { background: #094771; }
        QToolButton:disabled { color: #555555; }
    )");

    tb->addAction("📂 打开", this, [this](){
        QString path = QFileDialog::getOpenFileName(
            this, "打开脚本", QDir::homePath(), "GalDou 脚本 (*.gal)");
        if (!path.isEmpty()) m_editorPanel->openFile(path);
    });

    tb->addAction("💾 保存  Ctrl+S", this, [this](){
        m_editorPanel->saveCurrentFile();
    });

    tb->addSeparator();

    m_actRun = tb->addAction("▶  运行  F5");
    m_actRun->setEnabled(false);
    connect(m_actRun, &QAction::triggered, this, &MainAppWindow::onRun);

    m_actStop = tb->addAction("⏹  停止  F6");
    m_actStop->setEnabled(false);
    connect(m_actStop, &QAction::triggered, this, &MainAppWindow::onStop);

    m_actReload = tb->addAction("↺  重载");
    m_actReload->setEnabled(false);
    connect(m_actReload, &QAction::triggered, this, &MainAppWindow::onReload);
}

void MainAppWindow::setupMenuBar() {
    auto* mb = menuBar();
    mb->setStyleSheet(R"(
        QMenuBar { background:#2D2D2D; color:#CCCCCC; }
        QMenuBar::item:selected { background:#094771; }
        QMenu { background:#252526; color:#CCCCCC; border:1px solid #454545; }
        QMenu::item:selected { background:#094771; }
    )");

    auto* fm = mb->addMenu("文件(&F)");
    fm->addAction("打开脚本\tCtrl+O", this, [this](){
        QString path = QFileDialog::getOpenFileName(
            this, "打开脚本", QDir::homePath(), "GalDou 脚本 (*.gal)");
        if (!path.isEmpty()) m_editorPanel->openFile(path);
    });
    fm->addAction("保存\tCtrl+S", this, [this](){ m_editorPanel->saveCurrentFile(); });
    fm->addSeparator();
    fm->addAction("退出", this, &QWidget::close);

    auto* rm = mb->addMenu("运行(&R)");
    rm->addAction("▶ 运行\tF5",  this, &MainAppWindow::onRun);
    rm->addAction("⏹ 停止\tF6",  this, &MainAppWindow::onStop);
    rm->addAction("↺ 重新加载",  this, &MainAppWindow::onReload);

    auto* hm = mb->addMenu("帮助(&H)");
    hm->addAction("关于", this, [this](){
        QMessageBox::about(this, "关于 GalDou",
            "<b>GalDou v0.1</b><br>Galgame 开发套件<br><br>"
            "左侧编写脚本，右侧实时预览。<br>"
            "保存后点 ▶ 运行，点击预览区推进对话。");
    });
}

void MainAppWindow::setupStatusBar() {
    m_cursorLabel = new QLabel("就绪", this);
    m_cursorLabel->setStyleSheet("color:#858585; padding:0 8px;");
    m_statusLabel = new QLabel("", this);
    m_statusLabel->setStyleSheet("color:#4EC9B0; padding:0 8px; font-weight:bold;");
    statusBar()->addWidget(m_cursorLabel);
    statusBar()->addPermanentWidget(m_statusLabel);
    statusBar()->setStyleSheet("background:#007ACC; color:white;");
}

void MainAppWindow::onRun() {
    if (m_currentScriptPath.isEmpty()) {
        statusBar()->showMessage("请先保存脚本再运行", 3000);
        return;
    }
    m_scene->clear();
    m_scene->setBackgroundBrush(Qt::black);
    m_engine->loadScript(m_currentScriptPath);
    m_engine->start();
    m_isRunning = true;
    m_actRun->setEnabled(false);
    m_actStop->setEnabled(true);
    m_actReload->setEnabled(true);
    m_statusLabel->setText("● 运行中");
    relayoutPreview();
}

void MainAppWindow::onStop() {
    m_isRunning = false;
    m_scene->clear();
    m_scene->setBackgroundBrush(Qt::black);
    m_actRun->setEnabled(true);
    m_actStop->setEnabled(false);
    m_statusLabel->setText("");
    statusBar()->showMessage("已停止", 2000);
}

void MainAppWindow::onReload() {
    onStop();
    onRun();
}

void MainAppWindow::onScriptSaved(const QString& path) {
    m_currentScriptPath = path;
    m_actRun->setEnabled(true);
    m_actReload->setEnabled(m_isRunning);
    setWindowTitle("GalDou — " + QFileInfo(path).fileName());
    statusBar()->showMessage("已保存: " + QFileInfo(path).fileName(), 3000);
    if (m_isRunning) onReload();
}

void MainAppWindow::onGameEnded() {
    m_isRunning = false;
    m_actRun->setEnabled(true);
    m_actStop->setEnabled(false);
    m_statusLabel->setText("■ 已结束");
    statusBar()->showMessage("脚本播放完毕", 3000);
}

void MainAppWindow::relayoutPreview() {
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    int w = m_view->width();
    int h = m_view->height();
    m_dialog->setGeometry(0, h - 190, w, 185);
    m_choice->move((w - 500) / 2, (h - 300) / 2);
    m_choice->resize(500, 300);
}

void MainAppWindow::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    relayoutPreview();
}

bool MainAppWindow::eventFilter(QObject* obj, QEvent* e) {
    if (obj == m_view && e->type() == QEvent::MouseButtonPress && m_isRunning) {
        m_engine->next();
        return true;
    }
    return QMainWindow::eventFilter(obj, e);
}

void MainAppWindow::closeEvent(QCloseEvent* e) {
    e->accept();
}