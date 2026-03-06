#pragma once
#include <QObject>
#include <QGraphicsItem>

class Animator : public QObject {
    Q_OBJECT
public:
    explicit Animator(QObject* parent = nullptr);

    void fadeIn (QGraphicsItem* item, int ms = 400);
    void fadeOut(QGraphicsItem* item, int ms = 400,
                 bool deleteAfter = false);
};
