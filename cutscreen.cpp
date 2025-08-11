#include "cutscreen.h"

CutScreen::CutScreen(QWidget *parent) : QDialog(parent)
{
    // 关键设置：确保窗口无边框且置顶
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // 启用高分屏缩放支持
    setAttribute(Qt::WA_AcceptTouchEvents, false);
    setAttribute(Qt::WA_NoSystemBackground);


    this->setWindowFlags(this->windowFlags() |Qt::WindowStaysOnTopHint);
    m_screenMenu = new QMenu(this);
    m_screenMenu->addAction("保存截图",this,&CutScreen::slot_saveCapturedscreen);
    m_screenMenu->addAction("保存全屏截图",this,&CutScreen::slot_saveFullScreen);
    m_screenMenu->addAction("退出",this,&CutScreen::close_Capturedscreen);
}

CutScreen::~CutScreen()
{

}

void CutScreen::showEvent(QShowEvent *event)
{
    // 使用定时器延迟执行，确保窗口完全显示
    QTimer::singleShot(50, this, [this]() {
        // 获取当前屏幕（窗口所在的屏幕）
        QScreen *screen = nullptr;
        if (auto win = windowHandle()) {
            screen = win->screen();
        }
        if (!screen) {
            screen = QGuiApplication::primaryScreen();
        }

        // 保存设备像素比
        m_pixelRatio = screen->devicePixelRatio();

        // 获取屏幕几何信息（逻辑坐标）
        QRect screenGeometry = screen->geometry();

        // 确保窗口位置和大小正确
        setGeometry(screenGeometry);
        resize(screenGeometry.size());

        // 抓取屏幕图像（物理像素）
        m_screenPicture = screen->grabWindow(0,
                                             screenGeometry.x(),
                                             screenGeometry.y(),
                                             screenGeometry.width(),
                                             screenGeometry.height());

        // 设置设备像素比
        m_screenPicture.setDevicePixelRatio(m_pixelRatio);

        // 创建半透明覆盖层 - 直接创建与屏幕相同大小的覆盖层
        backgroundPicture = QPixmap(screenGeometry.size() * m_pixelRatio);
        backgroundPicture.setDevicePixelRatio(m_pixelRatio);
        backgroundPicture.fill(Qt::transparent);

        QPainter bgPainter(&backgroundPicture);
        bgPainter.setCompositionMode(QPainter::CompositionMode_Source);
        bgPainter.drawPixmap(0, 0, m_screenPicture);

        // 添加蓝色半透明覆盖层
        bgPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        bgPainter.fillRect(backgroundPicture.rect(), QColor(173, 216, 230, 50));
        bgPainter.end();

        // 强制重绘
        update();
    });

    QDialog::showEvent(event);
}

void CutScreen::paintEvent(QPaintEvent *event)
{
    // 确保背景图片有效
    if (backgroundPicture.isNull()) {
        QDialog::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 绘制整个背景（包括原始屏幕和半透明覆盖层）
    painter.drawPixmap(rect(), backgroundPicture, backgroundPicture.rect());

    // 获取选区矩形（逻辑坐标）
    QRect rect = getCapturedRect(m_startPos, m_endPos);

    if (rect.isValid()) {
        // 转换为物理像素坐标
        QRect physicalRect(
            rect.x() * m_pixelRatio,
            rect.y() * m_pixelRatio,
            rect.width() * m_pixelRatio,
            rect.height() * m_pixelRatio
            );

        // 从原图截取选区
        QPixmap fragment = m_screenPicture.copy(physicalRect);
        fragment.setDevicePixelRatio(m_pixelRatio);

        // 绘制选区内容（移除蓝色覆盖层）
        painter.drawPixmap(rect.x(), rect.y(), fragment);

        // 绘制选区边框
        QPen pen(Qt::white, 0);
        painter.setPen(pen);
        painter.drawRect(rect);
    }

    // 添加额外的蓝色覆盖层确保全屏覆盖
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    //painter.fillRect(this->rect(), QColor(173, 216, 230, 50));
}

void CutScreen::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_isMousePressed = true;
        m_startPos = event->pos();
    }
}

void CutScreen::mouseReleaseEvent(QMouseEvent *event)
{
    m_isMousePressed = false;
    m_screenMenu->exec(cursor().pos());
}

void CutScreen::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isMousePressed){
        m_endPos = event->pos();
        update();//产生绘图事件
    }
    QDialog::mouseMoveEvent(event);
}

void CutScreen::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape){
        clearinformation();
        hide();
    }
    else
    {
    QDialog::keyPressEvent(event);
    }
}

void CutScreen::contextMenuEvent(QContextMenuEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
    m_screenMenu->exec(cursor().pos());
}

void CutScreen::clearinformation()
{
    this->m_startPos = this->m_endPos = QPoint();
}

QRect CutScreen::getCapturedRect(QPoint startpos, QPoint endpos)
{
    QRect ret;
    if(startpos.x() < endpos.x())
    {
        ret.setLeft(startpos.x());
    }
    else
    {
        ret.setLeft(endpos.x());
    }

    if(startpos.y() < endpos.y())
    {
        ret.setTop(startpos.y());
    }
    else
    {
        ret.setTop(endpos.y());
    }

    ret.setWidth(qAbs(startpos.x()- endpos.x()));
    ret.setHeight(qAbs(startpos.y() - endpos.y()));

    return ret;
}

void CutScreen::slot_saveCapturedscreen()
{
    QClipboard *clipboard = QApplication::clipboard();
    QRect rect(getCapturedRect(m_startPos, m_endPos));
    clipboard->setPixmap(m_screenPicture.copy(rect));
    clearinformation();
    this->hide() ;
}

void CutScreen::slot_saveFullScreen()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(m_screenPicture);
    clearinformation();
    this->hide() ;
}

void CutScreen::close_Capturedscreen()
{
    //this->deleteLater();
    this->close();
}

//-------------------------------------------------------------------

// #include "cutscreen.h"
// #include <QColorDialog>
// #include <QFontMetrics>

// CutScreen::CutScreen(QWidget *parent) : QDialog(parent)
// {
//     // 关键设置：确保窗口无边框且置顶
//     setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//     setAttribute(Qt::WA_TranslucentBackground);

//     // 启用高分屏缩放支持
//     setAttribute(Qt::WA_AcceptTouchEvents, false);
//     setAttribute(Qt::WA_NoSystemBackground);

//     this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
//     m_screenMenu = new QMenu(this);

//     // 截图操作菜单
//     m_screenMenu->addAction("重新截图", this, &CutScreen::returnToCaptureMode);
//     m_screenMenu->addAction("保存截图", this, &CutScreen::slot_saveCapturedscreen);
//     m_screenMenu->addAction("保存全屏截图", this, &CutScreen::slot_saveFullScreen);
//     m_screenMenu->addAction("复制到剪贴板", this, &CutScreen::slot_copyToClipboard);
//     m_screenMenu->addSeparator();

//     // 新增绘图工具菜单
//     QMenu *drawMenu = new QMenu("绘图工具", this);
//     drawMenu->addAction("矩形工具", this, &CutScreen::setRectangleTool);
//     drawMenu->addAction("箭头工具", this, &CutScreen::setArrowTool);
//     drawMenu->addAction("文字工具", this, &CutScreen::setTextTool);
//     m_screenMenu->addMenu(drawMenu);

//     // 新增颜色选择菜单
//     QMenu *colorMenu = new QMenu("颜色选择", this);
//     colorMenu->addAction("红色", this, &CutScreen::setRedColor);
//     colorMenu->addAction("绿色", this, &CutScreen::setGreenColor);
//     colorMenu->addAction("蓝色", this, &CutScreen::setBlueColor);
//     colorMenu->addAction("其他颜色...", this, &CutScreen::setDrawColor);
//     m_screenMenu->addMenu(colorMenu);

//     // 新增线条粗细菜单
//     QMenu *widthMenu = new QMenu("线条粗细", this);
//     widthMenu->addAction("细线 (1px)", this, [this](){ m_drawWidth = 1; });
//     widthMenu->addAction("中等 (2px)", this, [this](){ m_drawWidth = 2; });
//     widthMenu->addAction("粗线 (3px)", this, [this](){ m_drawWidth = 3; });
//     widthMenu->addAction("加粗 (4px)", this, [this](){ m_drawWidth = 4; });
//     m_screenMenu->addMenu(widthMenu);

//     // 新增撤销/重做菜单
//     m_screenMenu->addSeparator();
//     m_screenMenu->addAction("撤销 (Ctrl+Z)", this, &CutScreen::undo);
//     m_screenMenu->addAction("重做 (Ctrl+Y)", this, &CutScreen::redo);
// }

// CutScreen::~CutScreen()
// {
// }

// void CutScreen::grabCurrentScreen() {
//     // 获取当前屏幕（窗口所在的屏幕）
//     QScreen *screen = nullptr;
//     if (auto win = windowHandle()) {
//         screen = win->screen();
//     }
//     if (!screen) {
//         screen = QGuiApplication::primaryScreen();
//     }

//     // 保存设备像素比
//     m_pixelRatio = screen->devicePixelRatio();

//     // 获取屏幕几何信息（逻辑坐标）
//     QRect screenGeometry = screen->geometry();

//     // 重新抓取当前屏幕图像
//     QPixmap newScreenPicture = screen->grabWindow(
//         0,
//         screenGeometry.x(),
//         screenGeometry.y(),
//         screenGeometry.width(),
//         screenGeometry.height()
//         );
//     newScreenPicture.setDevicePixelRatio(m_pixelRatio);

//     // 更新缓存的屏幕图像
//     m_screenPicture = newScreenPicture;
//     m_originalScreenPicture = newScreenPicture;

//     // 重新创建半透明覆盖层
//     backgroundPicture = QPixmap(screenGeometry.size() * m_pixelRatio);
//     backgroundPicture.setDevicePixelRatio(m_pixelRatio);
//     backgroundPicture.fill(Qt::transparent);

//     QPainter bgPainter(&backgroundPicture);
//     bgPainter.setCompositionMode(QPainter::CompositionMode_Source);
//     bgPainter.drawPixmap(0, 0, m_screenPicture);

//     // 添加蓝色半透明覆盖层
//     bgPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
//     bgPainter.fillRect(backgroundPicture.rect(), QColor(173, 216, 230, 50));
//     bgPainter.end();
// }

// void CutScreen::showEvent(QShowEvent *event)
// {
//     // 使用定时器延迟执行，确保窗口完全显示
//     QTimer::singleShot(50, this, [this]() {
//         // 调用封装的函数抓取屏幕图像
//         grabCurrentScreen();

//         // 初始状态为截图模式
//         m_isCapturing = true;
//         m_isEditing = false;
//         m_currentTool = ToolType::None;
//         m_drawingItems.clear();
//         m_undoStack.clear();
//         m_redoStack.clear();

//         // 强制重绘
//         update();
//     });

//     QDialog::showEvent(event);
// }

// void CutScreen::paintEvent(QPaintEvent *event)
// {
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::SmoothPixmapTransform);

//     // 绘制整个背景（包括原始屏幕和半透明覆盖层）
//     painter.drawPixmap(rect(), backgroundPicture, backgroundPicture.rect());

//     if (m_isCapturing) {
//         // 截图模式下，显示选区
//         QRect rect = getCapturedRect(m_startPos, m_endPos);

//         if (rect.isValid()) {
//             // 转换为物理像素坐标
//             QRect physicalRect(
//                 rect.x() * m_pixelRatio,
//                 rect.y() * m_pixelRatio,
//                 rect.width() * m_pixelRatio,
//                 rect.height() * m_pixelRatio
//                 );

//             // 从原图截取选区
//             QPixmap fragment = m_screenPicture.copy(physicalRect);
//             fragment.setDevicePixelRatio(m_pixelRatio);

//             // 绘制选区内容（移除蓝色覆盖层）
//             painter.drawPixmap(rect.x(), rect.y(), fragment);

//             // 绘制选区边框
//             QPen pen(Qt::white, 0);
//             painter.setPen(pen);
//             painter.drawRect(rect);
//         }

//         // 截图模式：保持全局半透明覆盖
//         painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
//         painter.fillRect(this->rect(), QColor(173, 216, 230, 50));
//     }
//     else if (m_isEditing) {
//         // 编辑模式：仅对截图区域外填充半透明层
//         painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

//         // 1. 填充整个窗口为半透明层
//         painter.fillRect(this->rect(), QColor(173, 216, 230, 50));

//         // 2. 用截图区域的内容覆盖半透明层（清除截图区域内的半透明覆盖）
//         painter.setCompositionMode(QPainter::CompositionMode_Source);
//         painter.drawPixmap(m_captureRect, m_screenPicture, m_screenPicture.rect());

//         // 3. 重新绘制所有图形
//         for (const DrawingItem &item : m_drawingItems) {
//             painter.setPen(QPen(item.color, item.width));

//             switch (item.type) {
//             case ToolType::Rectangle:
//                 painter.drawRect(item.item.toRect());
//                 break;
//             case ToolType::Arrow: {
//                 QLine line = item.item.toLine();
//                 painter.drawLine(line);

//                 // 绘制箭头头部
//                 double angle = std::atan2(-line.dy(), line.dx());
//                 QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * 10,
//                                                       cos(angle + M_PI / 3) * 10);
//                 QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * 10,
//                                                       cos(angle + M_PI - M_PI / 3) * 10);

//                 QPolygonF arrowHead;
//                 arrowHead << line.p2() << arrowP1 << arrowP2;
//                 painter.drawPolygon(arrowHead);
//                 break;
//             }
//             case ToolType::Text: {
//                 painter.setFont(item.font);
//                 painter.drawText(item.item.toRect(), Qt::AlignLeft | Qt::AlignTop, item.text);
//                 break;
//             }
//             default:
//                 break;
//             }
//         }

//         // 4. 绘制当前工具的预览（如果鼠标按下）
//         if (m_isMousePressed && m_currentTool != ToolType::Text) {
//             painter.setPen(QPen(m_drawColor, m_drawWidth));

//             if (m_currentTool == ToolType::Rectangle) {
//                 painter.drawRect(getCapturedRect(m_startPos, m_endPos));
//             }
//             else if (m_currentTool == ToolType::Arrow) {
//                 painter.drawLine(m_startPos, m_endPos);

//                 // 绘制箭头头部预览
//                 QLine line(m_startPos, m_endPos);
//                 double angle = std::atan2(-line.dy(), line.dx());
//                 QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * 10,
//                                                       cos(angle + M_PI / 3) * 10);
//                 QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * 10,
//                                                       cos(angle + M_PI - M_PI / 3) * 10);

//                 QPolygonF arrowHead;
//                 arrowHead << line.p2() << arrowP1 << arrowP2;
//                 painter.drawPolygon(arrowHead);
//             }
//         }
//     }
// }

// void CutScreen::mousePressEvent(QMouseEvent *event)
// {
//     if(event->button() == Qt::LeftButton){
//         m_isMousePressed = true;
//         m_startPos = event->pos();

//         if (m_isCapturing) {
//             // 截图模式下，开始选择区域
//         }
//         else if (m_isEditing) {
//             // 编辑模式下，开始绘图
//             if (m_currentTool == ToolType::Text) {
//                 // 文字工具立即处理
//                 drawTextAtPosition(event->pos());
//                 m_isMousePressed = false; // 文字工具不需要拖拽
//             }
//             else {
//                 // 其他工具开始绘图
//                 m_startPos = event->pos();
//                 m_endPos = event->pos();
//             }
//         }
//     }
// }

// void CutScreen::mouseReleaseEvent(QMouseEvent *event)
// {
//     if (event->button() == Qt::LeftButton) {
//         m_isMousePressed = false;

//         if (m_isCapturing) {
//             // 截图模式下，完成选区
//             m_captureRect = getCapturedRect(m_startPos, m_endPos);
//             if (m_captureRect.isValid() && m_captureRect.width() > 5 && m_captureRect.height() > 5) {
//                 // 进入编辑模式
//                 enterEditMode();
//             }
//         }
//         else if (m_isEditing) {
//             // 编辑模式下，完成绘图操作
//             if (m_currentTool != ToolType::Text) {
//                 QRect rect = getCapturedRect(m_startPos, m_endPos);

//                 switch (m_currentTool) {
//                 case ToolType::Rectangle:
//                     if (rect.isValid() && rect.width() > 5 && rect.height() > 5) {
//                         addDrawingItem(rect, ToolType::Rectangle);
//                     }
//                     break;
//                 case ToolType::Arrow: {
//                     QLine line(m_startPos, m_endPos);
//                     if (qAbs(line.dx()) > 5 || qAbs(line.dy()) > 5) {
//                         addDrawingItem(line, ToolType::Arrow);
//                     }
//                     break;
//                 }
//                 default:
//                     break;
//                 }
//             }
//         }

//         update();
//     }
// }

// void CutScreen::mouseMoveEvent(QMouseEvent *event)
// {
//     if(m_isMousePressed){
//         m_endPos = event->pos();

//         // 在截图模式下移动鼠标，实时更新选区
//         if (m_isCapturing) {
//             update();
//         }
//         // 在编辑模式下移动鼠标，实时更新绘图预览
//         else if (m_isEditing && (m_currentTool == ToolType::Rectangle ||
//                                  m_currentTool == ToolType::Arrow)) {
//             update();
//         }
//     }
//     QDialog::mouseMoveEvent(event);
// }

// void CutScreen::keyPressEvent(QKeyEvent *event)
// {
//     if (event->key() == Qt::Key_Escape) {
//         clearInformation();
//         hide();
//     }
//     // 撤销/重做快捷键
//     else if (event->modifiers() & Qt::ControlModifier) {
//         if (event->key() == Qt::Key_Z) {
//             undo();
//         } else if (event->key() == Qt::Key_Y) {
//             redo();
//         }
//     }
//     else {
//         QDialog::keyPressEvent(event);
//     }
// }

// void CutScreen::contextMenuEvent(QContextMenuEvent *event)
// {
//     this->setCursor(Qt::ArrowCursor);
//     m_screenMenu->exec(cursor().pos());
// }

// void CutScreen::clearInformation()
// {
//     this->m_startPos = this->m_endPos = QPoint();
// }

// QRect CutScreen::getCapturedRect(QPoint startpos, QPoint endpos)
// {
//     QRect ret;
//     if(startpos.x() < endpos.x())
//     {
//         ret.setLeft(startpos.x());
//     }
//     else
//     {
//         ret.setLeft(endpos.x());
//     }

//     if(startpos.y() < endpos.y())
//     {
//         ret.setTop(startpos.y());
//     }
//     else
//     {
//         ret.setTop(endpos.y());
//     }

//     ret.setWidth(qAbs(startpos.x()- endpos.x()));
//     ret.setHeight(qAbs(startpos.y() - endpos.y()));

//     return ret;
// }

// void CutScreen::returnToCaptureMode()
// {
//     this->close();
//     this->deleteLater();
// }

// void CutScreen::enterEditMode()
// {
//     // 进入编辑模式
//     m_isCapturing = false;
//     m_isEditing = true;

//     // 截取选定区域的图像
//     QRect physicalRect(
//         m_captureRect.x() * m_pixelRatio,
//         m_captureRect.y() * m_pixelRatio,
//         m_captureRect.width() * m_pixelRatio,
//         m_captureRect.height() * m_pixelRatio
//         );

//     m_screenPicture = m_originalScreenPicture.copy(physicalRect);
//     m_screenPicture.setDevicePixelRatio(m_pixelRatio);

//     // 保存原始截图用于撤销操作
//     m_undoStack.clear();
//     m_redoStack.clear();
//     m_undoStack.push(m_screenPicture);

//     update();
// }

// void CutScreen::slot_saveCapturedscreen()
// {
//     if (!m_isEditing) return;

//     // 1. 创建与编辑区域尺寸一致的图像（确保包含所有绘制内容）
//     QPixmap savedPixmap(m_screenPicture.size() * m_pixelRatio); // 按设备像素比缩放
//     savedPixmap.setDevicePixelRatio(m_pixelRatio);
//     savedPixmap.fill(Qt::transparent);

//     // 2. 重新绘制所有内容（原始截图 + 绘制的图形）
//     QPainter painter(&savedPixmap);
//     painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿，使图形更清晰

//     // 绘制原始截图（编辑区域的基础图像）
//     painter.drawPixmap(0, 0, m_screenPicture);

//     // 绘制所有通过工具添加的图形（矩形、箭头、文字）
//     for (const DrawingItem &item : m_drawingItems) {
//         painter.setPen(QPen(item.color, item.width)); // 使用图形的原始颜色和线宽

//         switch (item.type) {
//         case ToolType::Rectangle:
//             painter.drawRect(item.item.toRect());
//             break;
//         case ToolType::Arrow: {
//             QLine line = item.item.toLine();
//             painter.drawLine(line);
//             // 绘制箭头头部
//             double angle = std::atan2(-line.dy(), line.dx());
//             QPointF arrowP1 = line.p2() - QPointF(
//                                   sin(angle + M_PI / 3) * 10,
//                                   cos(angle + M_PI / 3) * 10
//                                   );
//             QPointF arrowP2 = line.p2() - QPointF(
//                                   sin(angle + M_PI - M_PI / 3) * 10,
//                                   cos(angle + M_PI - M_PI / 3) * 10
//                                   );
//             QPolygonF arrowHead;
//             arrowHead << line.p2() << arrowP1 << arrowP2;
//             painter.drawPolygon(arrowHead);
//             break;
//         }
//         case ToolType::Text:
//             painter.setFont(item.font); // 使用文字的原始字体
//             painter.drawText(item.item.toRect(), Qt::AlignLeft | Qt::AlignTop, item.text);
//             break;
//         default:
//             break;
//         }
//     }
//     painter.end(); // 结束绘制

//     // 3. 将包含所有内容的图像保存到剪贴板
//     QClipboard *clipboard = QApplication::clipboard();
//     clipboard->setPixmap(savedPixmap);

//     // 清理并隐藏窗口
//     clearInformation();
//     this->hide();
// }

// void CutScreen::slot_saveFullScreen()
// {
//     QClipboard *clipboard = QApplication::clipboard();
//     clipboard->setPixmap(m_originalScreenPicture);
//     clearInformation();
//     this->hide();
// }

// void CutScreen::slot_copyToClipboard()
// {
//     if (!m_isEditing) return;

//     QClipboard *clipboard = QApplication::clipboard();
//     clipboard->setPixmap(m_screenPicture);
// }

// // 绘图工具实现
// void CutScreen::setRectangleTool()
// {
//     if (!m_isEditing) return;
//     m_currentTool = ToolType::Rectangle;
//     setCursor(Qt::CrossCursor);
// }

// void CutScreen::setArrowTool()
// {
//     if (!m_isEditing) return;
//     m_currentTool = ToolType::Arrow;
//     setCursor(Qt::CrossCursor);
// }

// void CutScreen::setTextTool()
// {
//     if (!m_isEditing) return;
//     m_currentTool = ToolType::Text;
//     setCursor(Qt::IBeamCursor);
// }

// void CutScreen::undo()
// {
//     if (!m_isEditing) return;

//     if (m_undoStack.size() > 1) { // 保留至少一个基础状态
//         m_redoStack.push(m_screenPicture);
//         m_undoStack.pop();
//         m_screenPicture = m_undoStack.top();
//         update();
//     }
// }

// void CutScreen::redo()
// {
//     if (!m_isEditing) return;

//     if (!m_redoStack.isEmpty()) {
//         m_undoStack.push(m_screenPicture);
//         m_screenPicture = m_redoStack.pop();
//         update();
//     }
// }

// void CutScreen::setDrawColor()
// {
//     QColor color = QColorDialog::getColor(m_drawColor, this, "选择绘图颜色");
//     if (color.isValid()) {
//         m_drawColor = color;
//     }
// }

// void CutScreen::setRedColor()
// {
//     m_drawColor = Qt::red;
// }

// void CutScreen::setGreenColor()
// {
//     m_drawColor = Qt::green;
// }

// void CutScreen::setBlueColor()
// {
//     m_drawColor = Qt::blue;
// }

// void CutScreen::setDrawWidth()
// {
//     bool ok;
//     int width = QInputDialog::getInt(this, "设置线条粗细", "线条宽度 (1-10):",
//                                      m_drawWidth, 1, 10, 1, &ok);
//     if (ok) {
//         m_drawWidth = width;
//     }
// }

// void CutScreen::drawTextAtPosition(QPoint pos)
// {
//     bool ok;
//     QString text = QInputDialog::getText(this, "输入文本", "文本内容:",
//                                          QLineEdit::Normal, "", &ok);
//     if (ok && !text.isEmpty()) {
//         QFont font = QFontDialog::getFont(&ok, QFont("Arial", 12), this);
//         if (ok) {
//             // 创建文本项
//             DrawingItem textItem;
//             textItem.type = ToolType::Text;
//             textItem.color = m_drawColor;
//             textItem.width = m_drawWidth;
//             textItem.font = font;
//             textItem.text = text;

//             // 计算文本边界
//             QFontMetrics fm(font);
//             QRect textRect = fm.boundingRect(text);
//             textRect.moveTopLeft(pos);
//             textItem.item = textRect;

//             m_drawingItems.append(textItem);

//             // 更新绘图
//             updateDrawing();
//         }
//     }
// }

// void CutScreen::addDrawingItem(const QVariant &item, ToolType type)
// {
//     DrawingItem drawingItem;
//     drawingItem.item = item;
//     drawingItem.type = type;
//     drawingItem.color = m_drawColor;
//     drawingItem.width = m_drawWidth;

//     if (type == ToolType::Text) {
//         drawingItem.font = QFont("Arial", 12);
//     }

//     m_drawingItems.append(drawingItem);

//     // 保存当前状态到撤销栈
//     m_undoStack.push(m_screenPicture);

//     // 更新绘图
//     updateDrawing();
// }

// void CutScreen::updateDrawing()
// {
//     QPixmap temp = m_undoStack.top(); // 获取原始截图
//     QPainter painter(&temp);
//     painter.setRenderHint(QPainter::Antialiasing);

//     for (const DrawingItem &item : m_drawingItems) {
//         painter.setPen(QPen(item.color, item.width));

//         switch (item.type) {
//         case ToolType::Rectangle:
//             painter.drawRect(item.item.toRect());
//             break;
//         case ToolType::Arrow: {
//             QLine line = item.item.toLine();
//             painter.drawLine(line);

//             // 绘制箭头头部
//             double angle = std::atan2(-line.dy(), line.dx());
//             QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * 10,
//                                                   cos(angle + M_PI / 3) * 10);
//             QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * 10,
//                                                   cos(angle + M_PI - M_PI / 3) * 10);

//             QPolygonF arrowHead;
//             arrowHead << line.p2() << arrowP1 << arrowP2;
//             painter.drawPolygon(arrowHead);
//             break;
//         }
//         case ToolType::Text:
//             painter.setFont(item.font);
//             painter.drawText(item.item.toRect(), Qt::AlignLeft | Qt::AlignTop, item.text);
//             break;
//         default:
//             break;
//         }
//     }

//     painter.end();
//     m_screenPicture = temp;
//     update();
// }
