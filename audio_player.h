#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QDir>

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    void playAudio(const QString &filePath);
    void continuePlayAudio();  // 新增接口
    void pauseAudio();
    void stopAudio();
    void setVolume(int volume);
    QMediaPlayer::PlaybackState getState() const;

signals:
    void errorOccurred(const QString &errorMessage);
    void playFinished();

private slots:
    void onStateChanged(QMediaPlayer::PlaybackState state);
    void onErrorOccurred(QMediaPlayer::Error error);

private:
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
};

#endif // AUDIO_PLAYER_H

//----------------------------------------------
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>

class AutoStartManager
{
public:
    // 设置开机自启动
    static bool enableAutoStart(bool enable)
    {
        // 使用应用名称作为注册表键名（确保唯一性）
        const QString appName = QCoreApplication::applicationName();
        if (appName.isEmpty()) {
            qWarning() << "Application name is empty!";
            return false;
        }

        // 获取当前应用的可执行文件路径
        QString appPath = QCoreApplication::applicationFilePath();

        // 关键修复：确保使用Windows原生路径分隔符（反斜杠）
        appPath = QDir::toNativeSeparators(appPath);

        if (!QFileInfo::exists(appPath)) {
            qWarning() << "Application path does not exist:" << appPath;
            return false;
        }

        // 使用Windows注册表操作
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                           QSettings::NativeFormat);

        if (enable) {
            // 添加带引号的路径（处理空格问题）
            // 注意：路径本身不包含引号，QSettings会自动处理
            settings.setValue(appName, QString("\"%1\"").arg(appPath));
            qDebug() << "Set autostart path:" << QString("\"%1\"").arg(appPath);
        } else {
            // 移除注册表项
            settings.remove(appName);
            qDebug() << "Removed autostart";
        }

        return true;
    }

    // 检查是否已设置开机自启动
    static bool isAutoStartEnabled()
    {
        const QString appName = QCoreApplication::applicationName();
        if (appName.isEmpty()) return false;

        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                           QSettings::NativeFormat);

        // 获取注册表中的路径（移除可能的引号）
        QString regPath = settings.value(appName).toString().remove('"');
        QString appPath = QCoreApplication::applicationFilePath();

        // 比较路径是否相同（忽略大小写和斜杠方向）
        return QFileInfo(regPath).canonicalFilePath().compare(
                   QFileInfo(appPath).canonicalFilePath(),
                   Qt::CaseInsensitive) == 0;
    }
};
