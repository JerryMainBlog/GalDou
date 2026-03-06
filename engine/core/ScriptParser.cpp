#include "ScriptParser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

bool ScriptParser::load(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[ScriptParser] Cannot open:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    QString currentScene = "";

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;

        Command cmd = parseLine(line);

        if (cmd.type == Command::SCENE) {
            currentScene = cmd.args.value(0);
            continue;
        }
        m_scenes[currentScene].append(cmd);
    }

    qDebug() << "[ScriptParser] Loaded" << m_scenes.size()
             << "scenes from" << filePath;
    return true;
}

Command ScriptParser::parseLine(const QString& line) {
    Command cmd;

    if (!line.startsWith('[')) {
        cmd.type = Command::UNKNOWN;
        return cmd;
    }

    int close = line.indexOf(']');
    if (close == -1) { cmd.type = Command::UNKNOWN; return cmd; }

    QString tag  = line.mid(1, close - 1).trimmed().toLower();
    QString rest = line.mid(close + 1).trimmed();

    if      (tag == "bg")     cmd.type = Command::BG;
    else if (tag == "bgm")    cmd.type = Command::BGM;
    else if (tag == "show")   cmd.type = Command::SHOW;
    else if (tag == "hide")   cmd.type = Command::HIDE;
    else if (tag == "say")    cmd.type = Command::SAY;
    else if (tag == "choice") cmd.type = Command::CHOICE;
    else if (tag == "set")    cmd.type = Command::SET;
    else if (tag == "if")     cmd.type = Command::IF;
    else if (tag == "scene")  cmd.type = Command::SCENE;
    else if (tag == "end")    cmd.type = Command::END;
    else if (tag == "option") cmd.type = Command::OPTION;
    else                      cmd.type = Command::UNKNOWN;

    int i = 0;
    while (i < rest.size()) {
        while (i < rest.size() && rest[i] == ' ') i++;
        if (i >= rest.size()) break;

        if (rest[i] == '"') {
            i++;
            int start = i;
            while (i < rest.size() && rest[i] != '"') i++;
            cmd.args << rest.mid(start, i - start);
            if (i < rest.size()) i++;
        } else {
            int start = i;
            while (i < rest.size() && rest[i] != ' ') i++;
            cmd.args << rest.mid(start, i - start);
        }
    }

    return cmd;
}

QVector<Command> ScriptParser::getScene(const QString& name) const {
    return m_scenes.value(name);
}

bool ScriptParser::hasScene(const QString& name) const {
    return m_scenes.contains(name);
}

QVector<ChoiceOption> ScriptParser::getChoices(const QString& sceneName,
                                               int choiceCmdIndex) const {
    QVector<ChoiceOption> opts;
    const auto& cmds = m_scenes.value(sceneName);
    for (int i = choiceCmdIndex + 1; i < cmds.size(); ++i) {
        const auto& c = cmds[i];
        qDebug() << "[getChoices] type=" << c.type << "args=" << c.args;  // ← 加这行
        if (c.type == Command::OPTION && c.args.size() >= 2) {
            opts.push_back({c.args[0], c.args[1]});
        } else break;
    }
    return opts;
}
