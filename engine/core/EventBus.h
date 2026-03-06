/*
 * 模块之间不直接调用，只通过 EventBus 传消息，
 * 这样 GameEngine 不需要持有 DialogBox 的指针，
 * DialogBox 也不认识 GameEngine，完全解耦。
 */
#pragma once //头文件保护
#include <QObject>
#include <QVariant>
#include <functional>
#include <unordered_map>
#include <vector>

class EventBus : public QObject {
    Q_OBJECT
    public:
        static EventBus& instance();

        void publish(const QString& event, const QVariant& data = {});

        void subscribe(const QString& event, QObject* context, std::function<void(QVariant)> callback);

    private:
        struct Subscriber {
            QObject* context;
            std::function<void(QVariant)> callback;
        };

        std::unordered_map<QString, std::vector<Subscriber>> m_subs;
};
