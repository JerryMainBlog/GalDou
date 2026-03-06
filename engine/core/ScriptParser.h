#pragma once
#include <QString>
#include <QVector>
#include <QMap>

struct Command {
    enum Type {
        BG, BGM, SHOW, HIDE,
        SAY, CHOICE, OPTION,
        SET, IF,
        SCENE, END,
        UNKNOWN
    };
    Type        type;
    QStringList args;
};

struct ChoiceOption {
    QString text;
    QString targetScene;
};

class ScriptParser {
public:
    bool load(const QString& filePath);

    QVector<Command>      getScene(const QString& name) const;
    QVector<ChoiceOption> getChoices(const QString& sceneName,
                                     int choiceCmdIndex) const;
    bool hasScene(const QString& name) const;

private:
    QMap<QString, QVector<Command>> m_scenes;
    Command parseLine(const QString& line);
};
