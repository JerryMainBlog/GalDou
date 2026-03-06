#include "EventBus.h"

EventBus& EventBus::instance() {
    static EventBus bus;
    return bus;
}

void EventBus::publish(const QString& event,const QVariant& data) {
    auto it = m_subs.find(event);
    if(it == m_subs.end()) return;

    auto& subs = it->second;
    subs.erase(
        std::remove_if(subs.begin(), subs.end(), [](const Subscriber& s)
            {
                return s.context == nullptr;
            }),
        subs.end()
    );

    for (auto& s: subs) {
        if (s.context) s.callback(data);
    }
}

void EventBus::subscribe(const QString& event, QObject* context, std::function<void(QVariant)> callback) {
    m_subs[event].push_back({context, std::move(callback)});

    connect(context, &QObject::destroyed, this, [this, event, context](){
        auto it = m_subs.find(event);
        if (it == m_subs.end()) return;
        for (auto& s : it->second)
            if (s.context == context) s.context = nullptr;
    });
}
