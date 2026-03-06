#pragma once
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class AudioManager : public QObject {
    Q_OBJECT
public:
    explicit AudioManager(QObject* parent = nullptr);

    void playBGM(const QString& path);
    void stopBGM();
    void setVolume(float vol); // 0.0 ~ 1.0

private:
    QMediaPlayer*  m_player;
    QAudioOutput*  m_output;
};
