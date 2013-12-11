#include "ccRobotView.h"


namespace CCF
{
    static const QBrush CC_ROBOT_VIEW_BRUSH     = QBrush(Qt::red);
    static const QPen   CC_ROBOT_VIEW_PEN       = QPen(Qt::blue,2);
    static int size = 10;


    void RobotView::SetRobotSize(int _size)
    {
        size = _size;
    }

    RobotView::RobotView(const Displayable* _object)
        :View(_object)
    {
        SetBrush(CC_ROBOT_VIEW_BRUSH);
        SetPen(CC_ROBOT_VIEW_PEN);
    }

    void RobotView::Draw(QPainter& painter)
    {
        View::Draw(painter);
        const Robot* r = robot();
        painter.drawEllipse(View::convertCoordinateToDisplayerSize(r->GetPosition()),View::convertDistanceToDisplayerDistance(size)/2,View::convertDistanceToDisplayerDistance(size)/2);
        QPoint direction;
        direction.setX(r->GetPosition().x()+50*cos(r->GetOrientation()));
        direction.setY(r->GetPosition().y()+50*sin(r->GetOrientation()));
        painter.drawLine(View::convertCoordinateToDisplayerSize(r->GetPosition()),View::convertCoordinateToDisplayerSize(direction));
        painter.drawEllipse(View::convertCoordinateToDisplayerSize(direction),5,5);
    }

    const Robot* RobotView::robot() const
    {
        return (const Robot*)object;
    }
} // namespace CCF