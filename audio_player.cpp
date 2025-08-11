#include "audio_player.h"
#include <QDebug>

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this); // 创建音频输出对象

    // 将播放器与音频输出关联
    m_player->setAudioOutput(m_audioOutput);

    // 连接状态变化信号
    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, &AudioPlayer::onStateChanged);

    // 连接错误信号
    connect(m_player, &QMediaPlayer::errorOccurred,
            this, &AudioPlayer::onErrorOccurred);
}

AudioPlayer::~AudioPlayer()
{
    // 清理资源
    if (m_player) {
        m_player->stop();
        delete m_player;
        m_player = nullptr;
    }

    if (m_audioOutput) {
        delete m_audioOutput;
        m_audioOutput = nullptr;
    }
}

void AudioPlayer::playAudio(const QString &filePath)
{
    if (filePath.isEmpty()) {
        emit errorOccurred("文件路径为空");
        return;
    }

    QUrl url;
    // 检查是否是资源路径
    if (filePath.startsWith(":/")) {
        // 资源路径需要使用qrc协议
        url = QUrl("qrc" + filePath);
    } else {
        // 本地文件路径
        url = QUrl::fromLocalFile(filePath);
    }

    if (url.isValid()) {
        // 总是设置新源并重置位置
        m_player->setSource(url);
        m_player->setPosition(0);  // 确保从头开始
        m_player->play();
    } else {
        emit errorOccurred("无效的文件路径: " + filePath);
    }
}

void AudioPlayer::continuePlayAudio()
{
    if (!m_player || m_player->source().isEmpty()) {
        emit errorOccurred("没有可播放的音频");
        return;
    }

    // 根据当前状态决定行为
    switch (m_player->playbackState()) {
    case QMediaPlayer::PausedState:
        m_player->play();  // 从暂停位置继续
        break;
    case QMediaPlayer::StoppedState:
        // 如果是停止状态，从当前位置继续（可能是0或上次位置）
        m_player->play();
        break;
    case QMediaPlayer::PlayingState:
        // 已在播放状态，无需操作
        break;
    }
}

void AudioPlayer::pauseAudio()
{
    if (m_player) {
        m_player->pause();
    }
}

void AudioPlayer::stopAudio()
{
    if (m_player) {
        m_player->stop();
    }
}

void AudioPlayer::setVolume(int volume)
{
    if (m_audioOutput) {
        // 确保音量在0-100范围内
        int clampedVolume = qBound(0, volume, 100);
        // Qt 6中音量是0.0到1.0的浮点数，需要转换
        m_audioOutput->setVolume(clampedVolume / 100.0);
    }
}

QMediaPlayer::PlaybackState AudioPlayer::getState() const
{
    if (m_player) {
        return m_player->playbackState();
    }
    return QMediaPlayer::StoppedState;
}

void AudioPlayer::onStateChanged(QMediaPlayer::PlaybackState state)
{
    // 当状态变为停止且播放结束时，发送播放完成信号
    if (state == QMediaPlayer::StoppedState &&
        m_player->mediaStatus() == QMediaPlayer::EndOfMedia) {
        m_player->setPosition(0);  // 播放完成自动归零
        emit playFinished();
    }
}

void AudioPlayer::onErrorOccurred(QMediaPlayer::Error error)
{
    if (error != QMediaPlayer::NoError) {
        emit errorOccurred(m_player->errorString());
    }
}
