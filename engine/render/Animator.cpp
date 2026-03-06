#include "Animator.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

Animator::Animator(QObject* parent) : QObject(parent) {}

void Animator::fadeIn(QGraphicsItem* item, int ms) {
    auto* fx = new QGraphicsOpacityEffect();
    item->setGraphicsEffect(fx);

    auto* anim = new QPropertyAnimation(fx, "opacity", this);
    anim->setDuration(ms);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void Animator::fadeOut(QGraphicsItem* item, int ms, bool deleteAfter) {
    auto* fx = new QGraphicsOpacityEffect();
    item->setGraphicsEffect(fx);

    auto* anim = new QPropertyAnimation(fx, "opacity", this);
    anim->setDuration(ms);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    if (deleteAfter) {
        connect(anim, &QPropertyAnimation::finished, this, [item](){
            delete item;
        });
    }
}
