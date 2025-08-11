QT       += core gui network multimedia
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_FILE = app.rc
RC_ICONS = wait.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    alarm.cpp \
    audio_player.cpp \
    cutscreen.cpp \
    globalhotkey.cpp \
    main.cpp \
    dialog.cpp \
    settinngwindow.cpp

HEADERS += \
    alarm.h \
    audio_player.h \
    cutscreen.h \
    dialog.h \
    globalhotkey.h \
    settinngwindow.h

FORMS += \
    alarm.ui \
    dialog.ui \
    settinngwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assert.qrc
