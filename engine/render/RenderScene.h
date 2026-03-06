#pragma once
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMap>
#include "Animator.h"

class RenderScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit RenderScene(QObject* parent = nullptr);

    static constexpr int W = 1280;
    static constexpr int H = 720;

private:
    void onSetBg  (const QVariant& data);
    void onShow   (const QVariant& data);
    void onHide   (const QVariant& data);

    QGraphicsPixmapItem*              m_bg = nullptr;
    QMap<QString, QGraphicsPixmapItem*> m_chars;
    Animator*                         m_animator;
};
