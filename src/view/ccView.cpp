#include "ccView.h"

namespace CCF
{
    const ControlCenterView* View::controlCenterView = 0;

    View::View(const Displayable* _object)
        :object(_object)
    {}

    View::~View()
    {}

    void View::DefineControlCenterView(const ControlCenterView *_ControlCenterView)
    {
        controlCenterView = _ControlCenterView;
    }

    QPoint View::convertCoordinateToDisplayerSize(const QPoint& point)
    {
        if(!controlCenterView) return point;

        return controlCenterView->ConvertCoordinateToDisplayerSize(point);
    }

    int View::convertDistanceToDisplayerDistance(int distance)
    {
        if(!controlCenterView) return distance;

        return controlCenterView->ConvertDistanceToDisplayerDistance(distance);
    }

    void View::Draw(QPainter& painter)
    {
        painter.setBrush(brush);
        painter.setPen(pen);
    }

    void View::SetBrush(const QBrush& _brush)
    {
        brush = _brush;
    }


    void View::SetPen(const QPen& _pen)
    {
        pen = _pen;
    }

} // namespace CCF