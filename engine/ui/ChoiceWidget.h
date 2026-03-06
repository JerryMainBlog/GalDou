#pragma once
#include <QWidget>

class ChoiceWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChoiceWidget(QWidget* parent = nullptr);

private:
    void onShow(const QVariant& data);
    void onHide(const QVariant& data);
    void clearButtons();
};
