#pragma once

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <QShortcut>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QEvent>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QPanGesture>
#include <QWheelEvent>
#include <QDragEnterEvent>

class ImageView : public QLabel
{
Q_OBJECT
public:
    ImageView(QWidget *parent = nullptr);
    ~ImageView() = default;
    
public:
    void setMatImage(const cv::Mat& mat, bool nocopy = false);
    int LoadImageFile(QString strFileName);
    cv::Mat getMat() const {
        return imgOrigin_;
    }
    float GetCurImgScale() const {
        return scale_ * zoom_;
    }
    bool zoom(float factor);
    void fit();
    void setSelecting(bool selecting) {
        selecting_ = selecting;
    }
    //QPoint mat2view(cv::Point2d pt);
    cv::Point view2mat(QPoint pt);

protected:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

    cv::Mat imgOrigin_;
    cv::Mat imgDisplay_;
    QImage img_;
    cv::Size viewSize_;
    QPoint viewOrigin_;
    float scale_;
    float zoom_;
    cv::Point pan_;
    cv::Rect selRect_;
    QPoint dragStartPos_;
    bool pressed_ = false;
    bool selecting_ = false;
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    
    //mouse events
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    // drag & drop feature
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    // Mobile pan/zoom handling
    bool event(QEvent *event) override;
    virtual bool gestureEvent(QGestureEvent *event);
    virtual bool pinchTriggered(QPinchGesture *event);
    virtual bool panTriggered(QPanGesture *event);
    
public:
signals:
    void selectPixel(int x, int y, int r, int g, int b);
};
