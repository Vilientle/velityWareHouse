#ifndef GLOBALHOTKEY_H
#define GLOBALHOTKEY_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <Windows.h>

class GlobalHotkey : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit GlobalHotkey(QObject *parent = nullptr);
    ~GlobalHotkey();

    bool registerHotkey(Qt::Key key, Qt::KeyboardModifiers modifiers);
    bool unregisterHotkey();

signals:
    void activated();

protected:
    // 实现QAbstractNativeEventFilter的纯虚函数
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    ATOM hotkeyId; // 使用正确的ATOM类型
};

#endif // GLOBALHOTKEY_H
