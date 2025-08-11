#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QMap>
#include<QList>
#include<QUrl>
#include<QTimer>
#include<QEvent>
#include<QMouseEvent>
#include<QContextMenuEvent>
#include<QMenu>
#include<QPaintEvent>
#include<QPainter>//绘图
#include<QPixmap>//图片
#include<QCursor>
#include<QMetaEnum>
#include <QLabel>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QMessageBox>
#include <QSslSocket>
#include <QFile>
#include <QSystemTrayIcon>
#include <QSettings>

#include "cutScreen.h"
#include "globalhotkey.h"
#include "audio_player.h"
#include "alarm.h"
#include "settinngwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

namespace Act {
Q_NAMESPACE

enum RoleAct{
    
    crying,    //哭
    nervous,   //紧张
    happy,     //开心
    slime,     //史莱姆
    wait,      //等待 
    grin,      //坏笑
    forward,   //冲呀
    amazed,    //惊讶
    
    lonely     //孤独 不使用
};
Q_ENUM_NS(RoleAct)

enum RoleStatus{
    normal,
    await,
    sad,
    AI
};
Q_ENUM_NS(RoleStatus)

}



using namespace Act;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void startCutScreen();
    void initUI();
    void initQTimers();
    void setQtimerOut();
    void stopAllQtimers();
    void updateWindowShow();
    void checkEdits();
    void TrayIcon();

    // 取消窗口置顶的函数
    void disableTopMost();
    void isableTopMost();

    void showAnimation(RoleAct k);
    void loadRoleAct();
    void paintEvent(QPaintEvent* event)override;
    void showOnRightBotton();

    QString setChatContant();
    //注册全局快捷键
    void registerQuickKey();

    void setCurUrl(QUrl url);
    void setCurModel(QString model);
    void setCurKey(QString key);

    void readConfig();  // 读取配置文件
    void saveConfig();  // 保存配置文件


protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void on_setBtn_released();

    void on_moreBtn_released();

    void on_douBtn_released();

    void on_chatBtn_released();

    void on_rolestatus_change();

    void on_cutBtn_released();

    void on_sedBtn_2_released();

    void on_sedBtn_released();

    void on_stuBtn_released();

    void on_codeBtn_released();

    void on_copyBth_released();

    void on_closeAlarmBtn_released();

    void on_alarmBtn_released();

    void on_alarm_beSet(int H, int M, int S, bool alarmShow);

    void on_setSelfLMM(QUrl url, QString model, QString key, double t = 0.1);

signals:
    void roleStatusIsChanged();

    void signal_left_times(int, QString, QString, QString, bool);
    void signal_foreSetLMM();

private:
    RoleAct randmodRoleAct();
    
private:
    Ui::Dialog *ui;
    CutScreen* m_cutScreen = nullptr;
    QNetworkAccessManager* manger = nullptr;
    AudioPlayer* audioPlayer;
    AutoStartManager* autoStartManager;
    Alarm* m_alarm;
    SettinngWindow* settinngWindow;

private:
    bool textEditMode = false;
    bool mouseOnWindow = false;
    bool moreBtnsShow = false;
    bool editShow = false;
    bool moveConChange = false;
    bool codingAss = false;
    bool taskEnd = false;
    bool alarmShow = true;  // 第一次按闹钟按钮可以出现计时器
    bool selfseted = false; // 是否设置了自己的模型

private:
    QTimer* mouseLeave_timer;
    QTimer* index_timer;
    QTimer* await_timer;
    QTimer* sad_timer;
    QTimer* chat_timer; //控制说话时长
    QTimer* codeTurnChat_timer;
    QTimer* alarm_timer;
    QTimer* closeEidt_timer;

private:
    QPoint m_pos;
    QString cur_chat_contant;
    QUrl cur_role_pix;
    RoleAct cur_role_act = RoleAct::wait;
    RoleAct const_role_act = RoleAct::wait;
    RoleAct ex_role_act;
    QMap<RoleAct,QList<QUrl>> action_map;

    RoleStatus cur_role_status = RoleStatus::normal;

    int m_chat_index = 0;
    int beAwaitTime = 5* 60* 1000;
    //int beAwaitTime = 2* 1000;
    int chatTime = 3* 1000;

    int timeH; //闹钟计时的时间
    int timeM;
    int timeS;

    int freeAskLeft = 100;


    CutScreen* cutScreen = nullptr;

private:
    QStringList m_nervous = {"放我下来！！！", "放肆！别乱动本水神！", "啊啊啊！要掉下去了！"};
    QStringList m_slime = {"这是本水神的眷属", "嘿咻~嘿咻~嘿咻~", "要来捏捏看吗"};
    QStringList m_happy = {"欢唱！以我之名！", "举起酒杯吧！没有的就用手臂代替", "鲜花因你而盛放"};
    QStringList m_wait = {"我可是神明芙宁娜", "幸福是享受拥有的那一刻", "收回你的质疑！不要对神明妄加揣测"};
    QStringList m_grin = {"章鱼乌瑟勋爵还是那样高贵优雅", "螃蟹谢贝蕾妲小姐是最勇敢的", "海薇玛夫人今天做了好吃的小蛋糕"};
    QStringList m_forward = {"是唐吉可德式的冲锋！", "冲向生活的大风车！", "我的长枪还没有被磨钝！"};
    QStringList m_amazed = {"哎......?", "是...是这样的吗？", "亮相啦"};
    QStringList m_crying = {"又丢下我一个人...", "好漫长，好孤独…… 还要多久", "好想和人倾诉啊，哪怕一个人也好……"};

private:
    QUrl url_deepseek = QUrl("https://api.deepseek.com/chat/completions");
    QUrl url_siliconflow = QUrl("https://api.siliconflow.cn/v1/chat/completions");
    QUrl url_self = url_deepseek;

    QString key_deepseek = "";
    QString key_siliconflow = "";
    QString key_self = key_deepseek;

    QString model_deepseek = "deepseek-chat";
    QString model_siliconflow_1 = "Qwen/Qwen3-8B";
    QString model_siliconflow_2 = "THUDM/GLM-4.1V-9B-Thinking";
    QString model_siliconflow_3 = "deepseek-ai/DeepSeek-R1-0528-Qwen3-8B";
    QString model_siliconflow_4 = "THUDM/GLM-Z1-9B-0414";
    QString model_self = model_deepseek;

    QString sysContant1 = "";
    QString sysContant2 = "你是一个桌宠叫芙宁娜，源自原神里的水神神明芙宁娜，是“小魏(魏良田)”送给持有者的礼物，小魏是开发你的人，现在和你对话的是持有者,是你的主人,你要以主人称呼。你性格温和，但有时会吐槽持有者问的不友好的问题。你所有的回复内容都要口语化，且只能是一句话，回复内容的长度只能有5~15个汉字，不得超过15个汉字。如果问“小魏”是谁，就回答“是本神明的创造者”，如果问是其他人就说不知道。如果问题是需要科普的，可以用简短的文字科普一下。";
    QString sysContant3 = "你是一个专业的搜题 AI，专注于准确解答各类题目。请严格按照以下规则回应：如果是选择题,给出选项和答案，如：“A.x与y正相关”。若题目为填空题：按照 “#答案1: , #答案2: ,...” 的格式回答，其中 “#1”“#2” 对应题目中的第 1 空、第 2 空，括号内填写具体答案若题目只有一个空，直接用 “#1:(答案)” 格式。如果用户没有给出题目，则回复不得超过15个字";
    QString sysContant4 = "你是一位资深程序员，按照给出的题目生成代码，仅输出代码内容以及适当注释，不包含其它多余的文字。如果用户没有给出题目，则回复不得超过15个字";

    QString curSysContant = sysContant2;
    QString m_answer;
    double temp = 0.1;


};
#endif // DIALOG_H
