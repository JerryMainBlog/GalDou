#include "ChoiceWidget.h"
#include "core/EventBus.h"
#include <QPushButton>
#include <QVBoxLayout>

ChoiceWidget::ChoiceWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setLayout(new QVBoxLayout(this));
    layout()->setSpacing(12);
    hide();

    auto& bus = EventBus::instance();
    bus.subscribe("choice.show", this, [this](QVariant d){ onShow(d); });
    bus.subscribe("choice.hide", this, [this](QVariant d){ onHide(d); });
}

void ChoiceWidget::onShow(const QVariant& data) {
    clearButtons();

    QVariantList list = data.toList();
    for (const QVariant& item : list) {
        QVariantMap m   = item.toMap();
        QString text    = m["text"].toString();
        QString target  = m["target"].toString();

        auto* btn = new QPushButton(text, this);
        btn->setStyleSheet(R"(
            QPushButton {
                background: rgba(20,20,60,200);
                color: white;
                font-size: 16px;
                border: 1px solid rgba(100,150,255,150);
                border-radius: 8px;
                padding: 12px 40px;
                min-width: 300px;
            }
            QPushButton:hover {
                background: rgba(50,80,180,230);
                border: 1px solid white;
            }
        )");

        connect(btn, &QPushButton::clicked, this, [target](){
            EventBus::instance().publish("choice.made", target);
        });

        layout()->addWidget(btn);
    }
    show();
}

void ChoiceWidget::onHide(const QVariant&) {
    clearButtons();
    hide();
}

void ChoiceWidget::clearButtons() {
    QLayoutItem* child;
    while ((child = layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}
