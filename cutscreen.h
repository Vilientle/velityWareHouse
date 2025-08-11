#ifndef CUTSCREEN_H
#define CUTSCREEN_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QRect>
#include <QApplication>
#include <QGuiApplication>
#include <QPainter>
#include <QMouseEvent>
#include <string>
#include <math.h>
#include <QClipboard>
#include <QScreen>
#include <QMenu>
#include <QWindow>
#include <QTimer>

class CutScreen : public QDialog
{
    Q_OBJECT

private:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    void clearinformation();

public:
    CutScreen(QWidget* parent = nullptr);
    ~CutScreen();

    QRect getCapturedRect(QPoint startpos, QPoint endpos);
    void slot_saveCapturedscreen();
    void slot_saveFullScreen();
    void close_Capturedscreen();

private:
    QPixmap m_screenPicture;
    QPixmap backgroundPicture;
    bool m_isMousePressed = false;
    QPoint m_startPos, m_endPos;
    QMenu *m_screenMenu = nullptr;
    QRect m_screenGeometry; // 存储屏幕几何信息
    QPoint m_windowOffset; // 新增：窗口边框偏移量
    qreal m_pixelRatio = 1.0; // 添加设备像素比成员

};

#endif // CUTSCREEN_H

//-------------------------------------------------------
// #ifndef CUTSCREEN_H
// #define CUTSCREEN_H

// #include <QObject>
// #include <QWidget>
// #include <QDialog>
// #include <QRect>
// #include <QApplication>
// #include <QGuiApplication>
// #include <QPainter>
// #include <QMouseEvent>
// #include <string>
// #include <math.h>
// #include <QClipboard>
// #include <QScreen>
// #include <QMenu>
// #include <QWindow>
// #include <QTimer>
// #include <QColor>
// #include <QStack>
// #include <QInputDialog>
// #include <QFontDialog>
// #include <QVariant>
// #include <QLineF>
// #include <QPolygonF>
// #include <QColorDialog>
// #include <QLabel>
// #include <QPainterPath>

// enum class ToolType {
//     None, Rectangle, Arrow, Text
// };

// class CutScreen : public QDialog
// {
//     Q_OBJECT

// private:
//     void showEvent(QShowEvent *event);
//     void paintEvent(QPaintEvent *event);
//     void mousePressEvent(QMouseEvent *event);
//     void mouseReleaseEvent(QMouseEvent *event);
//     void mouseMoveEvent(QMouseEvent *event);
//     void keyPressEvent(QKeyEvent *event);
//     void contextMenuEvent(QContextMenuEvent *event);

//     void clearInformation();

//     void grabCurrentScreen();

// public:
//     CutScreen(QWidget* parent = nullptr);
//     ~CutScreen();

//     QRect getCapturedRect(QPoint startpos, QPoint endpos);
//     void returnToCaptureMode();
//     void slot_saveCapturedscreen();
//     void slot_saveFullScreen();
//     void slot_copyToClipboard();

// private slots:
//     void setRectangleTool();
//     void setArrowTool();
//     void setTextTool();
//     void undo();
//     void redo();
//     void setDrawColor();
//     void setDrawWidth();
//     void setRedColor();
//     void setGreenColor();
//     void setBlueColor();

// private:
//     void addDrawingItem(const QVariant &item, ToolType type);
//     void updateDrawing();
//     void enterEditMode();
//     void drawTextAtPosition(QPoint pos);

//     QPixmap m_screenPicture;
//     QPixmap m_originalScreenPicture; // 原始截图，用于撤销操作
//     QPixmap backgroundPicture;
//     bool m_isMousePressed = false;
//     QPoint m_startPos, m_endPos;
//     QMenu *m_screenMenu = nullptr;
//     QRect m_screenGeometry;
//     QPoint m_windowOffset;
//     qreal m_pixelRatio = 1.0;

//     // 截图状态
//     bool m_isCapturing = true; // 初始为截图模式
//     bool m_isEditing = false;  // 编辑模式标志
//     ToolType m_currentTool = ToolType::None;
//     QColor m_drawColor = Qt::red;
//     int m_drawWidth = 2;
//     QStack<QPixmap> m_undoStack;
//     QStack<QPixmap> m_redoStack;

//     // 截图区域
//     QRect m_captureRect;

//     // 绘制项存储
//     struct DrawingItem {
//         QVariant item;
//         ToolType type;
//         QColor color;
//         int width;
//         QFont font; // 文字工具专用
//         QString text; // 文字工具专用
//     };
//     QList<DrawingItem> m_drawingItems;
// };

// #endif // CUTSCREEN_H
