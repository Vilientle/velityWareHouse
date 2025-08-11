#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    readConfig();

    manger = new QNetworkAccessManager(this);
    m_alarm = new Alarm();
    audioPlayer = new AudioPlayer(this);
    autoStartManager = new AutoStartManager();
    settinngWindow = new SettinngWindow();

    initUI();
    initQTimers();

    loadRoleAct();
    showAnimation(cur_role_act);
    showOnRightBotton();
    registerQuickKey();

    await_timer->start(beAwaitTime);

    if(autoStartManager->isAutoStartEnabled()){
        // QMessageBox::information(this, "", "已设置开机自动启动");
        // if(autoStartManager->enableAutoStart(false))
        //     QMessageBox::information(this, "", "已取消设置");
    }else{
        // QMessageBox::information(this, "", "未设置开机自动启动");
        // if(autoStartManager->enableAutoStart(true))
        //     QMessageBox::information(this, "", "已成功设置");
        autoStartManager->enableAutoStart(true);
    }



}

Dialog::~Dialog()
{

    delete ui;
    m_alarm->deleteLater();
}

void Dialog::startCutScreen()
{
    if(m_cutScreen){
        m_cutScreen->deleteLater();
        m_cutScreen = nullptr;
    }
    m_cutScreen = new CutScreen();
    m_cutScreen->showFullScreen();
}

void Dialog::initUI()
{
    updateWindowShow();
    ui->conLabel->setText("");
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    this->setAttribute(Qt::WA_TranslucentBackground);//背景透明
    this->installEventFilter(this);

    ui->copyBth->hide();
    ui->closeAlarmBtn->hide();
    m_alarm->hide();
    settinngWindow->hide();

    TrayIcon();

    connect(this, &Dialog::roleStatusIsChanged, this, &Dialog::on_rolestatus_change);
    connect(m_alarm, &Alarm::setAlarmTime, this, &Dialog::on_alarm_beSet);
    connect(m_alarm, &Alarm::setAlarmTrue, [this](){alarmShow = true;});
    connect(settinngWindow, &SettinngWindow::signal_setSelf_model, this, &Dialog::on_setSelfLMM);
    connect(this, &Dialog::signal_left_times, settinngWindow, &SettinngWindow::on_setLabel);

    moveConChange = true;

}

void Dialog::initQTimers()
{
    mouseLeave_timer = new QTimer(this);
    index_timer = new QTimer(this);
    await_timer = new QTimer(this);
    sad_timer = new QTimer(this);
    chat_timer = new QTimer(this);
    codeTurnChat_timer = new QTimer(this);
    alarm_timer = new QTimer(this);
    closeEidt_timer = new QTimer(this);
    setQtimerOut();
}

void Dialog::setQtimerOut()
{
    connect(mouseLeave_timer, &QTimer::timeout, [this](){
        mouseOnWindow = false;
        moreBtnsShow = false;
        QString lineEditText = ui->lineEdit->text().trimmed(); // trimmed()去除首尾空格
        QString textEditText = ui->textEdit->toPlainText().trimmed();

        updateWindowShow();
        mouseLeave_timer->stop();
    });

    connect(index_timer,&QTimer::timeout,[this](){
        static int index = 0;
        auto paths = this->action_map.value(this->cur_role_act);
        this->cur_role_pix = paths[index++ % paths.size()];

        this->update();
        ui->label->update();
    });

    connect(await_timer,&QTimer::timeout,[this](){
        await_timer->stop();
        cur_role_status = RoleStatus::await;
        emit roleStatusIsChanged();
    });
    connect(sad_timer,&QTimer::timeout,[this](){
        sad_timer->stop();
        ui->conLabel->setText("");

        m_chat_index++;
        if(m_chat_index >= 20){
            cur_role_status = RoleStatus::sad;
            cur_role_act = RoleAct::crying;
            emit roleStatusIsChanged();
            m_chat_index = 0;
        }else{
            chat_timer->start(5* chatTime);
        }
    });
    connect(chat_timer,&QTimer::timeout,[this](){
        chat_timer->stop();
        ui->conLabel->setText("");
        on_rolestatus_change();
    });

    connect(codeTurnChat_timer,&QTimer::timeout,[this](){
        //QMessageBox::information(this, "提示", "codeTurnChat!");
        codeTurnChat_timer->stop();
        ui->copyBth->hide();
        on_chatBtn_released();
    });

    connect(alarm_timer,&QTimer::timeout,[this](){
        //QMessageBox::information(this,"","闹钟开启");
        alarm_timer->stop();
        audioPlayer->playAudio(":/assert/audio/love.wav");
        audioPlayer->setVolume(80);
        ui->closeAlarmBtn->show();
    });

    connect(closeEidt_timer,&QTimer::timeout,[this](){
        closeEidt_timer->stop();
        ui->lineEdit->hide();
        ui->textEdit->hide();
    });
}

void Dialog::stopAllQtimers()
{

}

bool Dialog::eventFilter(QObject *obj, QEvent *event)
{
    auto w=dynamic_cast<QDialog*>(obj);

    if(w)
    {
        if(event->type()==QEvent::MouseButtonPress)
        {
            auto e=dynamic_cast<QMouseEvent*>(event);
            if(e)
            {
                m_pos=e->pos();
            }
        }
        else if(event->type()==QEvent::MouseMove)
        {
            auto e=dynamic_cast<QMouseEvent*>(event);
            if(e)
            {
                if(e->buttons()&Qt::MouseButton::LeftButton)
                {
                    w->move(e->globalPosition().toPoint()- m_pos);

                    if(cur_role_act != RoleAct::nervous)
                        ex_role_act = cur_role_act;
                    cur_role_act = RoleAct::nervous;
                    ui->conLabel->setText(setChatContant());
                    chat_timer->start(chatTime);
                }
            }
        }
    }

    return QObject::eventFilter(obj,event);
}

void Dialog::enterEvent(QEnterEvent *event)
{
    QDialog::enterEvent(event);
    mouseOnWindow = true;
    //editShow = true;
    updateWindowShow();
    mouseLeave_timer->stop();
}

void Dialog::leaveEvent(QEvent *event)
{
    QDialog::leaveEvent(event);
    mouseLeave_timer->start(1 * 1000);
    checkEdits();
    if(cur_role_act == RoleAct::nervous){
        cur_role_act = ex_role_act;
        ui->conLabel->setText("");
    }

    moveConChange = true;
}

void Dialog::updateWindowShow()
{
    if(mouseOnWindow){
        ui->setBtn->show();
        ui->chatBtn->show();
        ui->douBtn->show();
        ui->moreBtn->show();


    }else{
        ui->setBtn->hide();
        ui->chatBtn->hide();
        ui->douBtn->hide();
        ui->moreBtn->hide();
        ui->stuBtn->hide();
        ui->codeBtn->hide();
        ui->cutBtn->hide();
        ui->alarmBtn->hide();
        ui->sedBtn->hide();
        ui->lineEdit->hide();
        ui->sedBtn_2->hide();
        ui->textEdit->hide();
    }
    if(moreBtnsShow){
        ui->stuBtn->show();
        ui->codeBtn->show();
        ui->cutBtn->show();
        ui->alarmBtn->show();
    }else{
        ui->stuBtn->hide();
        ui->codeBtn->hide();
        ui->cutBtn->hide();
        ui->alarmBtn->hide();
    }

    if(editShow){
        if(textEditMode){
            ui->sedBtn_2->show();
            ui->textEdit->show();
            ui->sedBtn->hide();
            ui->lineEdit->hide();
            ui->copyBth->show();
        }else{
            ui->sedBtn->show();
            ui->lineEdit->show();
            ui->sedBtn_2->hide();
            ui->textEdit->hide();
            ui->copyBth->hide();
        }
    }else{
        ui->sedBtn->hide();
        ui->lineEdit->hide();
        ui->sedBtn_2->hide();
        ui->textEdit->hide();
    }

    update();
}

void Dialog::checkEdits()
{
    QString lineEditText = ui->lineEdit->text().trimmed(); // trimmed()去除首尾空格
    QString textEditText = ui->textEdit->toPlainText().trimmed();

    // 只要有一个不为空，editShow就为true
    //editShow = !(lineEditText.isEmpty() && textEditText.isEmpty());
    if(!lineEditText.isEmpty() || !textEditText.isEmpty())
    {
        editShow = true;
    }
}

void Dialog::TrayIcon()
{
    auto actIcon = new QSystemTrayIcon(this);
    // 设置托盘图标（需准备一个图标文件，如 trayicon.png）
    actIcon->setIcon(QIcon(":/assert/wait/1.png"));
    actIcon->setToolTip("芙宁娜");
    actIcon->show();

    auto actMenu = new QMenu(this);

    // 创建菜单项
    auto showAction = new QAction("显示", this);
    auto hideAction = new QAction("隐藏", this);
    auto highSetAction = new QAction("高级设置", this);
    auto quitAction = new QAction("退出", this);

    // 添加菜单项到菜单（可添加分隔线）
    actMenu->addAction(showAction);
    actMenu->addAction(hideAction);
    actMenu->addSeparator();
    actMenu->addAction(highSetAction);
    actMenu->addSeparator();
    actMenu->addAction(quitAction);

    // 将菜单设置为托盘图标的右键菜单
    actIcon->setContextMenu(actMenu);

    connect(showAction, &QAction::triggered, [this](){
        this->setVisible(true);
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        // 重新显示窗口使设置生效
        this->show();
        showOnRightBotton();
    });
    connect(hideAction, &QAction::triggered, [this](){
        this->setVisible(false);
    });
    connect(quitAction, &QAction::triggered, [this](){
        saveConfig();
        setAttribute(Qt::WA_DeleteOnClose);
        this->close();
        QApplication::quit();
        exit(0);
    });
}

RoleAct Dialog::randmodRoleAct()
{
    RoleAct randAct;
    int rad = rand() % 11;

    switch (rad) {

    case 0:
        randAct = RoleAct::wait;
        break;
    case 1:
        randAct = RoleAct::slime;
        break;
    case 2:
        randAct = RoleAct::amazed;
        break;
    case 3:
        randAct = RoleAct::forward;
        break;
    case 4:
        randAct = RoleAct::grin;
        break;
    case 5:
        randAct = RoleAct::wait;
        break;
    case 6:
        randAct = RoleAct::slime;
        break;
    case 7:
        randAct = RoleAct::wait;
        break;
    case 8:
        randAct = RoleAct::slime;
        break;
    case 9:
        randAct = RoleAct::happy;
        break;
    case 10:
        randAct = RoleAct::happy;
        break;
    }


    return randAct;
}

void Dialog::disableTopMost()
{
    // 1. 获取当前窗口的所有标志
    Qt::WindowFlags flags = this->windowFlags();

    // 2. 移除Qt::WindowStaysOnTopHint标志（关键）
    flags &= ~Qt::WindowStaysOnTopHint;

    // 3. 重新应用窗口标志
    this->setWindowFlags(flags);

    // 4. 重新显示窗口使设置生效（修改标志后需重新show）
    this->show();
}

void Dialog::isableTopMost()
{
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    // 重新显示窗口使设置生效
    this->show();
}

void Dialog::on_setBtn_released()
{
    QMenu *menu = new QMenu(this);

    QAction *action1 = menu->addAction("置顶");
    QAction *action2 = menu->addAction("取消置顶");
    menu->addSeparator(); // 添加分隔线
    QAction *action3 = menu->addAction("隐藏");
    menu->addSeparator(); // 添加分隔线
    QAction *action4 = menu->addAction("高级设置");

    connect(action1, &QAction::triggered, this, [this](){
        isableTopMost();
    });
    connect(action2, &QAction::triggered, this, [this](){
        disableTopMost();
    });
    connect(action3, &QAction::triggered, this, [this](){
        this->setVisible(false);
    });
    connect(action4, &QAction::triggered, this, [this](){
        emit signal_left_times(freeAskLeft, model_self, url_self.toString(), key_self, selfseted);
        settinngWindow->show();
    });

    QPoint mousePos = QCursor::pos(); // 获取当前鼠标在屏幕上的位置
    menu->exec(mousePos); // 在鼠标位置弹出菜单

    menu->deleteLater();
}


void Dialog::on_moreBtn_released()
{
    moreBtnsShow = moreBtnsShow ? false : true;
    updateWindowShow();
}

void Dialog::showAnimation(RoleAct k)
{
    index_timer->stop();
    this->cur_role_act=k;
    index_timer->start(50);
}

void Dialog::loadRoleAct()
{
    auto addRes=[this](RoleAct k,QString path,int count)
    {
        QList<QUrl> paths;
        char buf[260];
        for (int i = 1; i <= count; ++i) {
            memset(buf, 0,sizeof(buf));
            //sprintf_s(buf, path.toStdString().c_str(), i);
            sprintf(buf, path.toStdString().c_str(), i);
            paths.append(QUrl::fromLocalFile(buf));
        }
        action_map.insert(k,paths);
    };

    addRes(RoleAct::crying,":/assert/crying/%d.png", 141);
    addRes(RoleAct::happy,":/assert/happy/%d.png", 25);
    //addRes(RoleAct::lonely,":/assert/lonely/%d.png", 59);
    addRes(RoleAct::nervous,":/assert/nervous/%d.png", 37);
    addRes(RoleAct::slime,":/assert/slime/%d.png", 62);
    addRes(RoleAct::wait,":/assert/wait/%d.png", 26);
    addRes(RoleAct::amazed,":/assert/amazed/%d.png", 75);
    addRes(RoleAct::forward,":/assert/forward/%d.png", 52);
    addRes(RoleAct::grin,":/assert/grin/%d.png", 64);

}

void Dialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(ui->label);

    QPixmap pix;
    pix.load(this->cur_role_pix.toLocalFile());

    //painter.drawPixmap(0, 0, pix);
    ui->label->setStyleSheet("background-color: transparent;");
    ui->label->setPixmap(pix.scaled(
        ui->label->size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        ));


}

void Dialog::showOnRightBotton()
{
    // 1. 获取屏幕可用区域（排除任务栏等系统区域）
    QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();

    // 2. 计算窗口右下角的目标位置
    // 窗口左上角坐标 = 屏幕右下角坐标 - 窗口自身宽高
    int x = screenRect.right() - this->width();
    int y = screenRect.bottom() - this->height();

    // 3. 确保窗口不会超出屏幕范围（针对低版本Qt可能需要）
    x = qMax(x, 0); // 避免x为负数
    y = qMax(y, 0); // 避免y为负数

    // 4. 设置窗口位置
    this->move(x, y);
}

QString Dialog::setChatContant()
{
    int i = rand()% 3;
    if(cur_role_act == RoleAct::amazed){
        cur_chat_contant = m_amazed[i];
    }
    if(cur_role_act == RoleAct::crying){
        cur_chat_contant = m_crying[i];
    }
    if(cur_role_act == RoleAct::forward){
        cur_chat_contant = m_forward[i];
    }
    if(cur_role_act == RoleAct::grin){
        cur_chat_contant = m_grin[i];
    }
    if(cur_role_act == RoleAct::happy){
        cur_chat_contant = m_happy[i];
    }
    if(cur_role_act == RoleAct::nervous && moveConChange){
        cur_chat_contant = m_nervous[i];
        moveConChange = false;
    }
    if(cur_role_act == RoleAct::slime){
        cur_chat_contant = m_slime[i];
    }
    if(cur_role_act == RoleAct::wait){
        cur_chat_contant = m_wait[i];
    }

    return cur_chat_contant;
}

void Dialog::registerQuickKey()
{
    // 创建全局快捷键
    GlobalHotkey *hotkey = new GlobalHotkey(this);
    // 注册Alt+X快捷键
    if(hotkey->registerHotkey(Qt::Key_X, Qt::AltModifier)) {
        connect(hotkey, &GlobalHotkey::activated, this, [=]() {
            qDebug() << "全局Alt+X被按下（窗口可以不在焦点）";

            on_cutBtn_released();

        });
        // 关键：安装事件过滤器（之前缺少这一步）
        qApp->installNativeEventFilter(hotkey);
    } else {
        qDebug() << "注册全局快捷键失败";
    }

}

void Dialog::setCurUrl(QUrl url)
{
    url_self = url;
}

void Dialog::setCurModel(QString model)
{
    model_self = model;
}

void Dialog::setCurKey(QString key)
{
    key_self = key;
}

void Dialog::readConfig()
{
    QSettings settings("config.ini", QSettings::IniFormat);

    // 读取配置值，如果不存在则使用默认值
    freeAskLeft = settings.value("freeAskLeft", freeAskLeft).toInt();
    url_self = QUrl(settings.value("url_self", url_deepseek.toString()).toString());
    key_self = settings.value("key_self", key_deepseek).toString();
    model_self = settings.value("model_self", model_deepseek).toString();
    selfseted = settings.value("selfseted", false).toBool();
}

void Dialog::saveConfig()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    // 保存配置值
    settings.setValue("freeAskLeft", freeAskLeft);
    settings.setValue("url_self", url_self.toString());
    settings.setValue("key_self", key_self);
    settings.setValue("model_self", model_self);
    settings.setValue("selfseted", selfseted);
}

void Dialog::on_douBtn_released()
{
    RoleAct newAct;
    await_timer->stop();
    m_chat_index = 0;

    while(1){
        newAct = randmodRoleAct();
        if(newAct != cur_role_act)
            break;
    }
    cur_role_act = newAct;
    cur_role_status = RoleStatus::normal;
    ui->conLabel->setText(setChatContant());
    chat_timer->start(chatTime);

    await_timer->start(beAwaitTime);
}


void Dialog::on_chatBtn_released()
{
    taskEnd = false;
    editShow = editShow ? false : true;
    textEditMode = false;
    curSysContant = sysContant2;
    updateWindowShow();
}

void Dialog::on_rolestatus_change()
{
    if(cur_role_status == RoleStatus::await){
        cur_role_act = randmodRoleAct();
        ui->conLabel->setText(setChatContant());
        sad_timer->start(chatTime);
    }
    if(cur_role_status == RoleStatus::sad){
        cur_role_act = RoleAct::crying;
        int i = rand()% 7;
        if(i == 0) ui->conLabel->setText(setChatContant());
        else ui->conLabel->setText("");
        chat_timer->start(chatTime);
    }
    if(cur_role_status == RoleStatus::AI){
        await_timer->stop();
        sad_timer->stop();
        chat_timer->stop();
    }
}


void Dialog::on_cutBtn_released()
{
    if(cutScreen){
        cutScreen->deleteLater();
        cutScreen = nullptr;
    }
    cutScreen = new CutScreen();
    cutScreen->showFullScreen();
}


void Dialog::on_sedBtn_2_released()
{
    QString str1 = ui->lineEdit->text();
    QString str2 = ui->textEdit->toPlainText();
    QString lineContant = str1 + str2;
    ui->lineEdit->clear();
    ui->textEdit->clear();
    ui->sedBtn->setEnabled(false);
    ui->sedBtn_2->setEnabled(false);
    ui->douBtn->setEnabled(false);
    ui->conLabel->setText("思考中...");
    ui->conLabel->show();

    closeEidt_timer->stop();
    closeEidt_timer->start(3* 60* 1000);

    codeTurnChat_timer->start(3* 60* 1000);

    if(!selfseted){
        if(freeAskLeft > 0){
            freeAskLeft--;
            url_self = url_siliconflow;
            model_self = model_siliconflow_1;
            key_self = key_siliconflow;
        }else{
            ui->conLabel->setText("免费次数用完了，去高级设置看看吧");
            ui->sedBtn->setEnabled(true);
            ui->sedBtn_2->setEnabled(true);
            ui->douBtn->setEnabled(true);
            return;
        }
    }

    QNetworkRequest request;

    request.setUrl(url_self);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(key_self).toUtf8());

    QJsonArray messages;

    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = curSysContant;
    if (systemMsg["content"] == sysContant4 || systemMsg["content"] == sysContant3){
        codingAss = true;
    }else{
        codingAss = false;
    }

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = lineContant;

    messages.append(systemMsg);
    messages.append(userMsg);

    QJsonObject requestBody;
    requestBody["messages"] = messages;
    requestBody["model"] = model_self;
    requestBody["max_tokens"] = 2048;
    requestBody["stream"] = true;
    requestBody["temperature"] = temp;

    //发送请求
    QNetworkReply* reply = manger->post(request, QJsonDocument(requestBody).toJson());

    //处理数据
    connect(reply, &QNetworkReply::readyRead, [=](){

        while(reply->canReadLine())
        {
            QString line = reply->readLine().trimmed();
            qDebug()<< line;
            if(line.startsWith("data: "))
            {
                line.remove(0, 6);
                QJsonParseError error;

                QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &error);

                if(error.error == QJsonParseError::NoError)  //是Json对象
                {
                    QString content = doc.object()["choices"].toArray().first()
                    .toObject()["delta"].toObject()["content"]
                        .toString();
                    if(!content.isEmpty())
                    {
                        QString ex = ui->conLabel->text();
                        if(ex == "思考中...") ex = "";
                        QString ne = ex + content;
                        if(textEditMode) m_answer = ne;
                        ui->conLabel->show();
                        ui->conLabel->setText(ne);
                    }
                }
            }
        }
    });

    connect(reply, &QNetworkReply::finished, this, [=](){
        ui->sedBtn->setEnabled(true);
        ui->sedBtn_2->setEnabled(true);
        ui->douBtn->setEnabled(true);

        await_timer->start(beAwaitTime);

        if(codingAss){
            if(taskEnd)ui->conLabel->setText("任务完成！点击左边的按钮复制吧！");
            ui->copyBth->setEnabled(true);
        }

        reply->deleteLater();
    });


    // 网络错误处理
    connect(reply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError error) {
        qDebug() << "Network Error:" << error << reply->errorString();
        ui->conLabel->setText("网络好像开小差了");
        ui->sedBtn->setEnabled(true);
        ui->sedBtn_2->setEnabled(true);
        ui->douBtn->setEnabled(true);
        await_timer->stop();
        await_timer->start(beAwaitTime);
    });
}


void Dialog::on_sedBtn_released()
{
    on_sedBtn_2_released();
}


void Dialog::on_stuBtn_released()
{
    closeEidt_timer->stop();
    closeEidt_timer->start(3* 60* 1000);
    taskEnd = false;
    textEditMode = true;
    editShow = editShow ? false : true;
    ui->copyBth->setEnabled(false);
    curSysContant = sysContant3;
    updateWindowShow();
    ui->copyBth->show();
}


void Dialog::on_codeBtn_released()
{
    closeEidt_timer->stop();
    closeEidt_timer->start(3* 60* 1000);
    taskEnd = true;
    textEditMode = true;
    editShow = editShow ? false : true;
    ui->copyBth->setEnabled(false);
    curSysContant = sysContant4;
    updateWindowShow();
    ui->copyBth->show();
}


void Dialog::on_copyBth_released()
{
    ui->copyBth->setEnabled(false);
    // 将 this->m_answer 的内容复制到剪贴板
    // 获取系统剪贴板实例
    QClipboard *clipboard = QApplication::clipboard();
    // 将文本设置到剪贴板
    clipboard->setText(m_answer);
    //QMessageBox::information(this, "", m_answer);

    m_answer = "";

}


void Dialog::on_closeAlarmBtn_released()
{
    ui->closeAlarmBtn->hide();
    audioPlayer->stopAudio();
    alarmShow = true;
}


void Dialog::on_alarmBtn_released()
{
    if(!alarmShow){
        QPoint mousePos = QCursor::pos();
        m_alarm->move(mousePos);
        return;
    }

    m_alarm->show();
    alarmShow = false;
    QPoint mousePos = QCursor::pos();
    m_alarm->move(mousePos);
}

void Dialog::on_alarm_beSet(int H, int M, int S, bool alarmShow)
{
    alarm_timer->stop();
    alarm_timer->start((H*1000*60*60) + (M*1000*60) + (S*1000));
    this->alarmShow = alarmShow;
    QMessageBox::information(this, "闹钟", "⏲开始计时");
}

void Dialog::on_setSelfLMM(QUrl url, QString model, QString key, double t)
{
    setCurUrl(url);
    setCurModel(model);
    setCurKey(key);
    this->temp = t;

    if(key == ""){
        selfseted = false;
        return;
    }

    //QMessageBox::information(this, "", "selfseed is true");
    selfseted = true;
}





