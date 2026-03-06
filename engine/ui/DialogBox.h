#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>

class DialogBox : public QWidget {
    Q_OBJECT
public:
    explicit DialogBox(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void showNextChar();

private:
    void onSay (const QVariant& data);
    void onHide(const QVariant& data);
    void onSkip(const QVariant& data);

    QLabel*  m_nameLabel;
    QLabel*  m_textLabel;
    QString  m_fullText;
    int      m_charIndex = 0;
    bool     m_animating = false;
    QTimer*  m_timer;
};
