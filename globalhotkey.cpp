#include "globalhotkey.h"
#include <Windows.h>
#include <QDebug>

GlobalHotkey::GlobalHotkey(QObject *parent) : QObject(parent), hotkeyId(0)
{
}

GlobalHotkey::~GlobalHotkey()
{
    unregisterHotkey();
}

bool GlobalHotkey::registerHotkey(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
    // 先注销已有的快捷键
    if (hotkeyId != 0) {
        unregisterHotkey();
    }

    // 转换Qt按键为Windows虚拟键码
    int vkCode = 0;
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        vkCode = 'A' + (key - Qt::Key_A);
    } else {
        // 可以根据需要添加其他按键的转换
        return false;
    }

    // 转换Qt修饰符为Windows修饰符
    UINT mod = 0;
    if (modifiers & Qt::AltModifier) mod |= MOD_ALT;
    if (modifiers & Qt::ControlModifier) mod |= MOD_CONTROL;
    if (modifiers & Qt::ShiftModifier) mod |= MOD_SHIFT;
    if (modifiers & Qt::MetaModifier) mod |= MOD_WIN;

    // 生成唯一的hotkey ID
    hotkeyId = GlobalAddAtomA("MyHotkey");
    if (hotkeyId == 0) {
        qDebug() << "无法创建全局原子";
        return false;
    }

    // 注册全局快捷键
    if (!RegisterHotKey(nullptr, hotkeyId, mod, vkCode)) {
        qDebug() << "注册快捷键失败，错误码:" << GetLastError();
        GlobalDeleteAtom(hotkeyId);
        hotkeyId = 0;
        return false;
    }

    return true;
}

bool GlobalHotkey::unregisterHotkey()
{
    if (hotkeyId != 0) {
        bool result = UnregisterHotKey(nullptr, hotkeyId) != 0;
        GlobalDeleteAtom(hotkeyId);
        hotkeyId = 0;
        return result;
    }
    return false;
}

bool GlobalHotkey::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(result);

    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            if (static_cast<quintptr>(msg->wParam) == hotkeyId) {
                emit activated();
                return true; // 事件已处理
            }
        }
    }
    return false;
}
