#pragma once
#include <QObject>
#include <QVector>

struct ValidationError {
    int     line;       // 1-indexed
    QString message;
};

/*
 * ScriptValidator
 * 对编辑器中的文本做轻量静态检查，
 * 返回错误列表供编辑器在侧边栏显示。
 * 不依赖 ScriptParser，直接逐行分析。
 */
class ScriptValidator : public QObject {
    Q_OBJECT
public:
    explicit ScriptValidator(QObject* parent = nullptr);

    QVector<ValidationError> validate(const QString& text) const;

private:
    static const QStringList s_knownTags;
};
