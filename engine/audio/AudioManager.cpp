#include "AudioManager.h"
#include "core/EventBus.h"
#include <QUrl>
#include <QDebug>

AudioManager::AudioManager(QObject* parent) : QObject(parent) {
    m_player = new QMediaPlayer(this);
    m_output = new QAudioOutput(this);
    m_player->setAudioOutput(m_output);
    m_output->setVolume(0.7f);
    m_player->setLoops(QMediaPlayer::Infinite);

    EventBus::instance().subscribe("audio.bgm", this,
                                   [this](QVariant data){ playBGM(data.toString()); });
    EventBus::instance().subscribe("audio.stop", this,
                                   [this](QVariant){ stopBGM(); });
}

void AudioManager::playBGM(const QString& path) {
    if (path.isEmpty()) return;
    qDebug() << "[AudioManager] Playing BGM:" << path;
    m_player->setSource(QUrl::fromLocalFile(path));
    m_player->play();
}

void AudioManager::stopBGM() {
    m_player->stop();
}

void AudioManager::setVolume(float vol) {
    m_output->setVolume(vol);
}
