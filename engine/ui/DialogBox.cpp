#include "DialogBox.h"
#include "core/EventBus.h"
#include <QPainter>
#include <QVBoxLayout>

DialogBox::DialogBox(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedHeight(180);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setStyleSheet(
        "color: white; font-size: 18px; font-weight: bold;"
        "background: rgba(0,80,180,180); padding: 4px 12px; border-radius: 6px;"
        );
    m_nameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_textLabel = new QLabel(this);
    m_textLabel->setStyleSheet("color: white; font-size: 16px;");
    m_textLabel->setWordWrap(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 15, 30, 15);
    layout->addWidget(m_nameLabel);
    layout->addWidget(m_textLabel);

    m_timer = new QTimer(this);
    m_timer->setInterval(40);
    connect(m_timer, &QTimer::timeout, this, &DialogBox::showNextChar);

    auto& bus = EventBus::instance();
    bus.subscribe("dialog.say",  this, [this](QVariant d){ onSay(d);  });
    bus.subscribe("dialog.hide", this, [this](QVariant d){ onHide(d); });
    bus.subscribe("dialog.skip", this, [this](QVariant d){ onSkip(d); });
}

void DialogBox::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(0, 0, 0, 170));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect().adjusted(10, 5, -10, -5), 12, 12);
}

void DialogBox::onSay(const QVariant& data) {
    QVariantMap m = data.toMap();
    m_nameLabel->setText(m["speaker"].toString());
    m_nameLabel->setVisible(!m["speaker"].toString().isEmpty());

    m_fullText   = m["text"].toString();
    m_charIndex  = 0;
    m_animating  = true;
    m_textLabel->clear();
    m_timer->start();
    show();
}

void DialogBox::onHide(const QVariant&) {
    m_timer->stop();
    hide();
}

void DialogBox::onSkip(const QVariant&) {
    if (!m_animating) return;
    m_timer->stop();
    m_animating = false;
    m_textLabel->setText(m_fullText);
}

void DialogBox::showNextChar() {
    if (m_charIndex >= m_fullText.size()) {
        m_timer->stop();
        m_animating = false;
        return;
    }
    m_textLabel->setText(m_fullText.left(++m_charIndex));
}
