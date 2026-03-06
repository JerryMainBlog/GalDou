#include "FlagManager.h"
#include <QStringList>

FlagManager& FlagManager::instance() {
    static FlagManager mgr;
    return mgr;
}

void FlagManager::set(const QString& key, QVariant value) {
    m_flags[key] = value;
}

QVariant FlagManager::get(const QString& key, QVariant defaultVal) const {
    return m_flags.value(key, defaultVal);
}

bool FlagManager::test(const QString& expr) const {
    const QStringList ops = {"==", "!=", ">=", "<=", ">", "<"};
    for (const QString& op : ops) {
        int idx = expr.indexOf(op);
        if (idx == -1) continue;
        QString key = expr.left(idx).trimmed();
        QString val = expr.mid(idx + op.length()).trimmed();
        return evalExpr(key, op, val);
    }
    // 没有运算符，直接判断 bool 值
    return m_flags.value(expr.trimmed()).toBool();
}

bool FlagManager::evalExpr(const QString& key, const QString& op,
                           const QString& val) const {
    QVariant lhs = m_flags.value(key);

    // 先尝试数值比较
    bool lOk, rOk;
    double l = lhs.toDouble(&lOk);
    double r = val.toDouble(&rOk);

    if (lOk && rOk) {
        if (op == "==") return l == r;
        if (op == "!=") return l != r;
        if (op == ">=") return l >= r;
        if (op == "<=") return l <= r;
        if (op == ">")  return l > r;
        if (op == "<")  return l < r;
    }

    // 字符串比较
    QString ls = lhs.toString();
    if (op == "==") return ls == val;
    if (op == "!=") return ls != val;
    return false;
}

QVariantMap FlagManager::toJson() const { return m_flags; }
void FlagManager::fromJson(const QVariantMap& data) { m_flags = data; }
void FlagManager::reset() { m_flags.clear(); }
