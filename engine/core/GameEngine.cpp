#include "GameEngine.h"
#include "EventBus.h"
#include "FlagManager.h"
#include <QDebug>

GameEngine::GameEngine(QObject* parent) : QObject(parent) {
    EventBus::instance().subscribe("choice.made", this,
                                   [this](QVariant data){ jumpToScene(data.toString()); });
}

bool GameEngine::loadScript(const QString& path) {
    return m_parser.load(path);
}

void GameEngine::start() {
    jumpToScene("");
}

void GameEngine::jumpToScene(const QString& name) {
    m_currentScene = name;
    m_cmds         = m_parser.getScene(name);
    m_idx          = 0;
    m_waiting      = false;
    EventBus::instance().publish("choice.hide");
    execute();
}

void GameEngine::next() {
    if (!m_waiting) return;

    EventBus::instance().publish("dialog.skip");
    m_waiting = false;
    m_idx++;
    execute();
}

void GameEngine::execute() {
    while (m_idx < m_cmds.size()) {
        const Command& cmd = m_cmds[m_idx];

        switch (cmd.type) {

        case Command::BG:
            EventBus::instance().publish("render.bg", cmd.args.value(0));
            autoNext(); return;

        case Command::BGM:
            EventBus::instance().publish("audio.bgm", cmd.args.value(0));
            autoNext(); return;

        case Command::SHOW: {
            QVariantMap m;
            m["path"] = cmd.args.value(0);
            m["pos"]  = cmd.args.value(1, "center");
            EventBus::instance().publish("render.show", m);
            autoNext(); return;
        }

        case Command::HIDE:
            EventBus::instance().publish("render.hide", cmd.args.value(0));
            autoNext(); return;

        case Command::SAY: {
            QVariantMap m;
            m["speaker"] = cmd.args.value(0);
            m["text"]    = cmd.args.value(1);
            EventBus::instance().publish("dialog.say", m);
            m_waiting = true;
            return;
        }

        case Command::CHOICE: {
            auto opts = m_parser.getChoices(m_currentScene, m_idx);
            QVariantList list;
            for (auto& o : opts) {
                QVariantMap m;
                m["text"]   = o.text;
                m["target"] = o.targetScene;
                list << m;
            }
            EventBus::instance().publish("choice.show", list);
            m_waiting = true;
            return;
        }

        case Command::SET:
            if (cmd.args.size() >= 2)
                FlagManager::instance().set(cmd.args[0], cmd.args[1]);
            m_idx++; break;

        case Command::IF: {
            QString expr       = cmd.args.value(0);
            QString sceneTrue  = cmd.args.value(1);
            QString sceneFalse = cmd.args.value(2);
            jumpToScene(FlagManager::instance().test(expr)
                            ? sceneTrue : sceneFalse);
            return;
        }

        case Command::END:
            EventBus::instance().publish("dialog.hide");
            emit gameEnded();
            return;

        default:
            m_idx++; break;
        }
    }
}

void GameEngine::autoNext() {
    m_idx++;
    execute();
}
