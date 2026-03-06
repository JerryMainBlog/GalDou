#include "PreviewPanel.h"
#include "../engine/core/EventBus.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>

PreviewPanel::PreviewPanel(QWidget* parent) : QWidget(parent) {
    setStyleSheet("background-color: #000000;");
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(480, 270);

    // ── 占位提示（未加载脚本时显示）────────────────────────
    m_placeholder = new QLabel("← 打开脚本后点击 ▶ 运行", this);
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet("color: #555555; font-size: 16px;");
    m_placeholder->setGeometry(0, 0, width(), height());

    // ── 渲染场景 ────────────────────────────────────────────
    m_scene = new RenderScene(this);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setFrameStyle(QFrame::NoFrame);
    m_view->hide();

    // ── UI 层（叠在 view 上方）──────────────────────────────
    m_dialog = new DialogBox(m_view);
    m_choice = new ChoiceWidget(m_view);

    // ── 引擎 & 音频 ─────────────────────────────────────────
    m_engine = new GameEngine(this);
    m_audio  = new AudioManager(this);

    connect(m_engine, &GameEngine::gameEnded, this, [this](){
        emit gameEnded();
    });
}

void PreviewPanel::loadAndRun(const QString& scriptPath) {
    if (scriptPath.isEmpty()) return;

    // 停止旧的
    stop();

    m_currentScript = scriptPath;

    // 重置 EventBus 相关状态（隐藏对话框和选项）
    EventBus::instance().publish("dialog.hide");
    EventBus::instance().publish("choice.hide");

    if (!m_engine->loadScript(scriptPath)) return;

    m_placeholder->hide();
    m_view->show();
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    relayout();

    m_engine->start();
    setFocus();
}

void PreviewPanel::stop() {
    EventBus::instance().publish("audio.stop");
    EventBus::instance().publish("dialog.hide");
    EventBus::instance().publish("choice.hide");
    EventBus::instance().publish("render.bg",   QString(""));
}

void PreviewPanel::relayout() {
    int w = m_view->width();
    int h = m_view->height();
    m_dialog->setGeometry(0, h - 190, w, 185);
    m_choice->move((w - 500) / 2, (h - 300) / 2);
    m_choice->resize(500, 300);
}

void PreviewPanel::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    m_view->setGeometry(0, 0, width(), height());
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    m_placeholder->setGeometry(0, 0, width(), height());
    relayout();
}

void PreviewPanel::mousePressEvent(QMouseEvent*) {
    m_engine->next();
}

void PreviewPanel::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Space || e->key() == Qt::Key_Return)
        m_engine->next();
}
