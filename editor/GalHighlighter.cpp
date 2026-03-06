#include "GalHighlighter.h"

GalHighlighter::GalHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    // ── 注释 #... ───────────────────────────────────────────
    QTextCharFormat commentFmt;
    commentFmt.setForeground(QColor("#6A9955"));
    commentFmt.setFontItalic(true);
    addRule(R"(#.*$)", commentFmt);

    // ── [scene] 场景标题 ────────────────────────────────────
    QTextCharFormat sceneFmt;
    sceneFmt.setForeground(QColor("#C586C0"));
    sceneFmt.setFontWeight(QFont::Bold);
    addRule(R"(\[scene\][^\n]*)", sceneFmt);

    // ── 对话命令 [say] ──────────────────────────────────────
    QTextCharFormat sayFmt;
    sayFmt.setForeground(QColor("#4EC9B0"));
    sayFmt.setFontWeight(QFont::Bold);
    addRule(R"(\[say\])", sayFmt);

    // ── 选项命令 [choice] [option] ──────────────────────────
    QTextCharFormat choiceFmt;
    choiceFmt.setForeground(QColor("#CE9178"));
    choiceFmt.setFontWeight(QFont::Bold);
    addRule(R"(\[(choice|option)\])", choiceFmt);

    // ── 逻辑命令 [set] [if] [end] ───────────────────────────
    QTextCharFormat logicFmt;
    logicFmt.setForeground(QColor("#DCDCAA"));
    logicFmt.setFontWeight(QFont::Bold);
    addRule(R"(\[(set|if|end)\])", logicFmt);

    // ── 媒体命令 [bg] [bgm] [show] [hide] ───────────────────
    QTextCharFormat mediaFmt;
    mediaFmt.setForeground(QColor("#569CD6"));
    mediaFmt.setFontWeight(QFont::Bold);
    addRule(R"(\[(bg|bgm|show|hide)\])", mediaFmt);

    // ── 字符串参数 "..." ─────────────────────────────────────
    QTextCharFormat stringFmt;
    stringFmt.setForeground(QColor("#CE9178"));
    addRule(R"("[^"]*")", stringFmt);
}

void GalHighlighter::addRule(const QString& pattern, const QTextCharFormat& fmt) {
    m_rules.push_back({ QRegularExpression(pattern), fmt });
}

void GalHighlighter::highlightBlock(const QString& text) {
    for (const auto& rule : m_rules) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
