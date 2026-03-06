#pragma once
#include <QObject>
#include <QVariantMap>

class FlagManager : public QObject {
    Q_OBJECT
public:
    static FlagManager& instance();

    void     set(const QString& key, QVariant value);
    QVariant get(const QString& key, QVariant defaultVal = {}) const;
    bool     test(const QString& expr) const;

    QVariantMap toJson() const;
    void fromJson(const QVariantMap& data);
    void reset();

private:
    QVariantMap m_flags;

    bool evalExpr(const QString& key, const QString& op,
                  const QString& val) const;
};
