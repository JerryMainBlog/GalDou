#pragma once
#include <QObject>
#include <QString>
#include "ScriptParser.h"

class GameEngine : public QObject {
    Q_OBJECT
public:
    explicit GameEngine(QObject* parent = nullptr);

    bool loadScript(const QString& path);
    void start();
    void next();
    void jumpToScene(const QString& name);

signals:
    void gameEnded();

private:
    void execute();
    void autoNext();

    ScriptParser     m_parser;
    QVector<Command> m_cmds;
    int              m_idx          = 0;
    QString          m_currentScene = "";
    bool             m_waiting      = false;
};
