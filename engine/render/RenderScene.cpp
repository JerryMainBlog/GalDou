#include "RenderScene.h"
#include "core/EventBus.h"
#include <QDebug>

RenderScene::RenderScene(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, W, H);
    setBackgroundBrush(Qt::black);

    m_animator = new Animator(this);

    auto& bus = EventBus::instance();
    bus.subscribe("render.bg",   this, [this](QVariant d){ onSetBg(d); });
    bus.subscribe("render.show", this, [this](QVariant d){ onShow(d);  });
    bus.subscribe("render.hide", this, [this](QVariant d){ onHide(d);  });
}

void RenderScene::onSetBg(const QVariant& data) {
    QString path = data.toString();
    QPixmap pix(path);
    if (pix.isNull()) {
        qWarning() << "[RenderScene] BG not found:" << path;
        return;
    }

    if (m_bg) removeItem(m_bg);

    pix  = pix.scaled(W, H, Qt::KeepAspectRatioByExpanding,
                     Qt::SmoothTransformation);
    m_bg = addPixmap(pix);
    m_bg->setZValue(0);
    m_animator->fadeIn(m_bg, 300);
}

void RenderScene::onShow(const QVariant& data) {
    QVariantMap m    = data.toMap();
    QString path     = m["path"].toString();
    QString pos      = m["pos"].toString();

    QPixmap pix(path);
    if (pix.isNull()) {
        qWarning() << "[RenderScene] Char not found:" << path;
        return;
    }

    // 同路径立绘已存在则替换
    if (m_chars.contains(path)) {
        removeItem(m_chars[path]);
        delete m_chars[path];
    }

    pix = pix.scaledToHeight(H * 0.75, Qt::SmoothTransformation);
    auto* item = addPixmap(pix);
    item->setZValue(1);

    // 定位
    qreal x = (W - pix.width()) / 2.0;
    if (pos == "left")  x = W * 0.05;
    if (pos == "right") x = W * 0.55;
    qreal y = H - pix.height();
    item->setPos(x, y);

    m_chars[path] = item;
    m_animator->fadeIn(item, 400);
}

void RenderScene::onHide(const QVariant& data) {
    QString path = data.toString();
    if (!m_chars.contains(path)) return;

    auto* item = m_chars.take(path);
    m_animator->fadeOut(item, 400, true);
}
