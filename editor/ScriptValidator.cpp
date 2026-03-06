#include "ScriptValidator.h"
#include <QStringList>
#include <QRegularExpression>

const QStringList ScriptValidator::s_knownTags = {
    "scene", "bg", "bgm", "show", "hide",
    "say", "choice", "option",
    "set", "if", "end"
};

ScriptValidator::ScriptValidator(QObject* parent) : QObject(parent) {}

QVector<ValidationError> ScriptValidator::validate(const QString& text) const {
    QVector<ValidationError> errors;
    QStringList lines = text.split('\n');

    bool hasScene = false;
    QRegularExpression tagRe(R"(^\[([^\]]+)\])");

    for (int i = 0; i < lines.size(); ++i) {
        const QString line = lines[i].trimmed();
        int lineNo = i + 1;

        if (line.isEmpty() || line.startsWith('#')) continue;

        if (!line.startsWith('[')) {
            errors.push_back({ lineNo, "行必须以 [ 开头，或为注释 (#)" });
            continue;
        }

        auto m = tagRe.match(line);
        if (!m.hasMatch()) {
            errors.push_back({ lineNo, "缺少闭合的 ]" });
            continue;
        }

        QString tag = m.captured(1).trimmed().toLower();

        if (!s_knownTags.contains(tag)) {
            errors.push_back({ lineNo, QString("未知命令标签: [%1]").arg(tag) });
            continue;
        }

        if (tag == "scene") {
            hasScene = true;
            QString rest = line.mid(m.capturedEnd()).trimmed();
            if (rest.isEmpty())
                errors.push_back({ lineNo, "[scene] 缺少场景名称" });
        }

        if (tag == "say") {
            QString rest = line.mid(m.capturedEnd()).trimmed();
            // say 需要至少一个参数（说话人可选，但文本必须有）
            QRegularExpression quotedRe(R"("[^"]*")");
            auto qm = quotedRe.globalMatch(rest);
            int count = 0;
            while (qm.hasNext()) { qm.next(); count++; }
            if (count == 0)
                errors.push_back({ lineNo, "[say] 需要至少一个带引号的文本参数" });
        }

        if (tag == "if") {
            QString rest = line.mid(m.capturedEnd()).trimmed();
            QStringList parts = rest.split(' ', Qt::SkipEmptyParts);
            if (parts.size() < 3)
                errors.push_back({ lineNo, "[if] 格式: [if] 条件表达式 场景_true 场景_false" });
        }

        if (tag == "option") {
            QString rest = line.mid(m.capturedEnd()).trimmed();
            QRegularExpression quotedRe(R"("[^"]*")");
            auto qm = quotedRe.globalMatch(rest);
            int count = 0;
            while (qm.hasNext()) { qm.next(); count++; }
            if (count < 2)
                errors.push_back({ lineNo, "[option] 格式: [option] \"按钮文字\" \"目标场景\"" });
        }
    }

    if (!hasScene && !text.trimmed().isEmpty())
        errors.push_back({ 1, "脚本中没有找到任何 [scene] 定义" });

    return errors;
}
