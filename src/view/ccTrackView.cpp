#include "ccTrackView.h"

#include <QtCore/QPoint>
#include <QtGui/QPixmap>
#include <QtGui/QPen>

namespace CCF
{
    static const QPen   CC_TRACK_VIEW_PEN   = QPen(Qt::blue,2);

    TrackView::TrackView(const Displayable* _object, bool _drawFlags)
        :View(_object), drawFlags(_drawFlags)
    {
        SetPen(CC_TRACK_VIEW_PEN);
    }

    void TrackView::Draw(QPainter& painter)
    {
        static QPixmap startFlagPixmap = QPixmap(":/icons/ressources/flag_blue.png");
        static QPixmap endFlagPixmap = QPixmap(":/icons/ressources/flag_red.png");

        View::Draw(painter);
        Track t(track()->GetName());
        // First we convert all coordinates to the displayer size
        for(int i=0; i< track()->size(); i++)
        {
            t.append(View::convertCoordinateToDisplayerSize(track()->at(i)));
        }
        // We draw the track
        painter.drawPolyline(t);
        // We add two smart flags :D
        if(drawFlags && t.size()>=2)
        {
            painter.drawPixmap(t.first().x()-startFlagPixmap.size().width()/2,t.first().y()-startFlagPixmap.size().height()/2,startFlagPixmap);
            painter.drawPixmap(t.last().x()-endFlagPixmap.size().width()/2,t.last().y()-endFlagPixmap.size().height()/2,endFlagPixmap);
        }
    }

    const Track* TrackView::track() const
    {
        return (const Track*)object;
    }
} // namespace CCF