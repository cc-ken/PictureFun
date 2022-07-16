#include "imageview.h"
#include "utils/logger.h"

#include <QStyleOption>
#include <QPainter>
#include <QPalette>
#include <QMimeData>

ImageView::ImageView(QWidget *parent) : QLabel(parent), scale_(1.0f), zoom_(1.0f)
{
    QPalette palette;
    palette.setColor(QPalette::Normal, QPalette::Window, QColor(0, 0, 0));
    setAutoFillBackground(true);
    setPalette(palette);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    setAcceptDrops(true);
    setMouseTracking(true);
}

int ImageView::loadImageFile(QString strFileName)
{
    std::string filename = strFileName.toStdString();
    cv::Mat img = cv::imread(filename, cv::IMREAD_UNCHANGED);
    
    setMatImage(img, false);
    
    return 0;
}

void ImageView::setMatImage(const cv::Mat &mat, bool nocopy)
{
    if (mat.empty()) {
        return;
    }
    
    if (!nocopy) {
        mat.copyTo(imgOrigin_);
    } else {
        imgOrigin_ = mat;
    }

    int nChannels = mat.channels();
    if (nChannels == 1) {
        cv::cvtColor(mat, imgDisplay_, cv::COLOR_GRAY2RGB);
    }
    else if (nChannels == 4) {
        cv::cvtColor(mat, imgDisplay_, cv::COLOR_BGRA2RGB);
    }
    else {
        cv::cvtColor(mat, imgDisplay_, cv::COLOR_BGR2RGB);
    }
    
    const uint8_t *qImageBuffer = (const uint8_t*)imgDisplay_.data;
    img_ = QImage(qImageBuffer, imgDisplay_.cols, imgDisplay_.rows, imgDisplay_.step, QImage::Format_RGB888);
    
    setPixmap(QPixmap::fromImage(img_));
    fit();
}

bool ImageView::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QWidget::event(event);
}

bool ImageView::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        return pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }
    else if (QGesture *pan = event->gesture(Qt::PanGesture)) {
        return panTriggered(static_cast<QPanGesture *>(pinch));
    }
    return true;
}

bool ImageView::panTriggered(QPanGesture *event)
{
    //LOG_INFO("pan");
    return false;
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    int delta = event->pixelDelta().y();
    if (delta != 0) {
        float factor = 1 + (0.02 * delta / fabs(delta));
        //LOG_INFO("wheel zoom=" << factor << ", delta=" << delta);
        zoom(factor);
    }
}

void ImageView::fit()
{
    zoom_ = 1.f;
    pan_ = cv::Point();
    
    update();
}

bool ImageView::zoom(float factor)
{
    if (factor < 1 && zoom_ * scale_ * img_.width() < 100) {
        return false;
    }
    zoom_ *= factor;
    
    update();
}

bool ImageView::pinchTriggered(QPinchGesture *event)
{
    return zoom(event->scaleFactor());
}

void ImageView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (img_.isNull()) {
        return QWidget::paintEvent(event);
    }
    
    scale_ = qMin((float)width() / img_.width(), (float)height() / img_.height());
    int w = img_.width() * scale_;
    int h = img_.height() * scale_;
    
    viewSize_ = cv::Size(w, h);

    int nOrgX = width() / 2 + pan_.x;
    int nOrgY = height() / 2 + pan_.y;
    viewOrigin_.setX(nOrgX);
    viewOrigin_.setY(nOrgY);
    painter.translate(nOrgX, nOrgY);
    painter.scale(zoom_, zoom_);

    QRect picRect(-w / 2, -h / 2, w, h);
    painter.drawImage(picRect, img_);
    
    if (selecting_)
        drawSelectBox();
}

QRect ImageView::selectRect()
{
    QPoint lt(qMin(dragStartPos_.x(), dragCurrPos_.x()), qMin(dragStartPos_.y(), dragCurrPos_.y()));
    QPoint br(qMax(dragStartPos_.x(), dragCurrPos_.x()), qMax(dragStartPos_.y(), dragCurrPos_.y()));
    
    return QRect(lt, br);
}

cv::Rect ImageView::roi() {
    auto sel = selectRect();
    cv::Rect result(view2mat(sel.topLeft()), view2mat(sel.bottomRight()));
    
    cv::imshow("test", imgOrigin_(result));
    
    return result;
}

void ImageView::drawSelectBox() {
    QPainter painter(this);
    painter.setPen(QPen(Qt::blue, 2, Qt::DotLine));
    painter.setBrush(QBrush(Qt::transparent));
    QRect roi = selectRect();
    painter.drawRect(roi);
}

cv::Point ImageView::view2mat(QPoint p)
{
    p = p - viewOrigin_;
    p = p / zoom_;
    
    double x = p.x() / scale_;
    double y = p.y() / scale_;

    cv::Point2d delta(img_.width() / 2, img_.height() / 2);

    x = x + delta.x;
    y = y + delta.y;

    if (x >= 0 && y >= 0 && x < imgDisplay_.cols && y < imgDisplay_.rows) {
        return cv::Point(x, y);
    }
    return cv::Point();
}

QSize ImageView::sizeHint() const
{
    return QLabel::sizeHint();
}

QSize ImageView::minimumSizeHint() const
{
    return QLabel::minimumSizeHint();
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    dragCurrPos_ = dragStartPos_ = event->pos();
    pressed_ = true;
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    if (!pressed_) {
        if (!imgDisplay_.empty()) {
            cv::Point pt = view2mat(event->pos());
            cv::Vec3b color = imgDisplay_.at<cv::Vec3b>(pt);
            emit selectPixel(pt.x, pt.y, color[0], color[1], color[2]);
        }
        return QWidget::mouseMoveEvent(event);
    }

    QPoint pos = event->pos();
    if (selecting_) {
        setCursor(Qt::CrossCursor);
        dragCurrPos_ = pos;
    }
    else {
        setCursor(Qt::SizeAllCursor);
        pan_.x += (pos.x() - dragStartPos_.x());
        pan_.y += (pos.y() - dragStartPos_.y());
        dragStartPos_ = pos;
    }
    update();
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    pressed_ = false;
    setCursor(Qt::ArrowCursor);
    update();
}

void ImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
    
}

void ImageView::dragEnterEvent(QDragEnterEvent *event)
{
    if(!event->mimeData()->urls()[0].toLocalFile().right(3).compare("jpg",Qt::CaseInsensitive)
        ||!event->mimeData()->urls()[0].toLocalFile().right(3).compare("png",Qt::CaseInsensitive)
        ||!event->mimeData()->urls()[0].toLocalFile().right(3).compare("bmp",Qt::CaseInsensitive)
        ||!event->mimeData()->urls()[0].toLocalFile().right(4).compare("jpeg",Qt::CaseInsensitive))
    {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImageView::dropEvent(QDropEvent *event)
{
    loadImageFile(event->mimeData()->urls()[0].toLocalFile());
}
