#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

/*
 * GalHighlighter
 * 为 .gal 脚本提供语法高亮，规则：
 *   [scene]   → 紫色粗体（场景标题）
 *   [bg] [show] [hide] [bgm] 等命令标签 → 蓝色粗体
 *   [say]     → 绿色（对话命令）
 *   [choice] [option] → 橙色
 *   [set] [if] → 黄色（逻辑命令）
 *   "字符串"  → 灰白色（参数值）
 *   # 注释    → 深灰斜体
 */
class GalHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit GalHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };
    QVector<Rule> m_rules;

    void addRule(const QString& pattern, const QTextCharFormat& fmt);
};
