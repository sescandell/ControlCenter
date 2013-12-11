#include "ccStatisticsDisplayer.h"

#include <QtGui/QSizePolicy>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPolygon>
#include <QtGui/QColor>
#include <QtGui/QFont>

#include <QtCore/QRegExp>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QPoint>

namespace CCF
{
    static const int    CC_SD_BORDER_ROUND_COIN_SIZE    = 5;
    static const QBrush CC_SD_BORDER_BRUSH              = Qt::NoBrush;
    static const QPen   CC_SD_BORDER_PEN                = QPen(QColor(200,200,200));

    static const QBrush CC_SD_GRID_BRUSH             = QBrush(QColor(0,0,0));
    static const QPen   CC_SD_GRID_PEN               = Qt::NoPen;
    static const int    CC_SD_GRID_LEFT              = 25;
    static const int    CC_SD_GRID_MAX_POINTS        = 100;
    static const int    CC_SD_GRID_PADDING_TEXT_LEFT = 3;

    static const QBrush CC_SD_AXES_BRUSH     = Qt::NoBrush;
    static const QPen   CC_SD_AXES_PEN       = QPen(QColor(70,250,70));
    static const int    CC_SD_AXES_FONT_SIZE = 8;

    static const QPen   CC_SD_LINES_PEN     = QPen(QColor(40,120,40));
    static const int    CC_SD_LINES_COUNT   = 3;

    static const int    CC_SD_PADDING_LEFT  = 5;
    static const int    CC_SD_PADDING_TOP   = 15;
    static const int    CC_SD_PADDING_BOTTOM= 5;

    static const int    CC_SD_MARGIN_BOTTOM = 5;
    static const int    CC_SD_MARGIN_RIGHT  = 5;

    static const int    CC_SD_TEXT_HEIGHT   = 10;
    static const QPen   CC_SD_TEXT_PEN      = QPen(QColor(0,0,0));

    static const QPen   CC_SD_RSSI_PEN  = QPen(QColor(255,0,0));
    static const int    CC_SD_RSSI_MAX  = -160;

    static const QPen   CC_SD_LQI_PEN   = QPen(QColor(0,255,0));
    static const int    CC_SD_LQI_MAX   = 160;

    static const QPen   CC_SD_DELAY_PEN = QPen(QColor(0,0,255));
    static const int    CC_SD_DELAY_MAX = 16;

    StatisticsDisplayer::StatisticsDisplayer(const Robot* _robot, QWidget* parent)
        :QWidget(parent), robot(_robot), displayDelay(true), displayRSSI(true), displayLQI(true)
    {
        setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum));
        setAutoFillBackground(true);
        setMinimumWidth(50);
        setMinimumHeight(200);

        show();
    }

    QString StatisticsDisplayer::formatPortName(const QString& name) const
    {
        QRegExp rx("(\\d+)");
        QString formatedName(name);
        if(formatedName.contains(rx)) {
            int portNum = rx.cap(1).toInt();
            formatedName = "COM " + QString::number(portNum);
        }
        return formatedName;
    }

    int StatisticsDisplayer::GetIdentifier() const
    {
        return robot->GetIdentifier();
    }

    void StatisticsDisplayer::paintEvent(QPaintEvent* ev)
    {
        QPainter p(this);
        // Draw a border to make the widget visible
        p.setPen(CC_SD_BORDER_PEN);
        p.setBrush(CC_SD_BORDER_BRUSH);
        p.drawRoundRect(0,0,width()-1,height()-1,CC_SD_BORDER_ROUND_COIN_SIZE,CC_SD_BORDER_ROUND_COIN_SIZE);

        // Draw the robot name and communication interface
        p.setPen(CC_SD_TEXT_PEN);
        p.drawText(CC_SD_PADDING_LEFT,CC_SD_PADDING_TOP,robot->GetName() + QString(" - ") + formatPortName(robot->GetInterfaceName()));

        ////// Draw the Grid
        drawGrid(p);

        ////// Draw curves
        drawCurves(p);

        ev->accept();
    }

    void StatisticsDisplayer::drawGrid(QPainter& p) const
    {
        //// Background
        p.setPen(CC_SD_GRID_PEN);
        p.setBrush(CC_SD_GRID_BRUSH);
        p.drawRect(CC_SD_PADDING_LEFT,CC_SD_PADDING_TOP+CC_SD_TEXT_HEIGHT,width()-CC_SD_PADDING_LEFT-CC_SD_MARGIN_RIGHT,height()-CC_SD_PADDING_TOP-CC_SD_TEXT_HEIGHT-CC_SD_PADDING_BOTTOM);
        //// Axes
        p.setPen(CC_SD_AXES_PEN);
        p.setBrush(CC_SD_GRID_BRUSH);
        // X
        p.drawLine(CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM,width()-CC_SD_MARGIN_RIGHT-1,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM);
        // Y
        p.drawLine(CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM,CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT,CC_SD_PADDING_TOP+CC_SD_TEXT_HEIGHT);
        //// Lines
        const int gridHeight = height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-CC_SD_PADDING_TOP-CC_SD_TEXT_HEIGHT;
        p.setPen(CC_SD_LINES_PEN);
        p.drawLine(CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT, height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)  , width()-CC_SD_MARGIN_RIGHT-1, height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1));
        p.drawLine(CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT, height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*2, width()-CC_SD_MARGIN_RIGHT-1, height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*2);
        p.drawLine(CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT, height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*3, width()-CC_SD_MARGIN_RIGHT-1, height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*3);
        //// Text
        QFont font = p.font();
        font.setPointSize(CC_SD_AXES_FONT_SIZE);
        p.setFont(font);
        // RSSI
        if(displayRSSI)
        {
            p.setPen(CC_SD_RSSI_PEN);
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)-CC_SD_TEXT_HEIGHT,   QString::number(CC_SD_RSSI_MAX/(CC_SD_LINES_COUNT+1)));
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*2-CC_SD_TEXT_HEIGHT, QString::number(CC_SD_RSSI_MAX/(CC_SD_LINES_COUNT+1)*2));
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*3-CC_SD_TEXT_HEIGHT, QString::number(CC_SD_RSSI_MAX/(CC_SD_LINES_COUNT+1)*3));
        }
        // LQI
        if(displayLQI)
        {
            p.setPen(CC_SD_LQI_PEN);
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1),   QString::number(CC_SD_LQI_MAX/(CC_SD_LINES_COUNT+1)));
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*2, QString::number(CC_SD_LQI_MAX/(CC_SD_LINES_COUNT+1)*2));
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*3, QString::number(CC_SD_LQI_MAX/(CC_SD_LINES_COUNT+1)*3));
        }
        // DELAY
        if(displayDelay)
        {
            p.setPen(CC_SD_DELAY_PEN);
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)+CC_SD_TEXT_HEIGHT,   QString::number(CC_SD_DELAY_MAX/(CC_SD_LINES_COUNT+1)));
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*2+CC_SD_TEXT_HEIGHT, QString::number(CC_SD_DELAY_MAX/(CC_SD_LINES_COUNT+1)*2));
            p.drawText(CC_SD_PADDING_LEFT+CC_SD_GRID_PADDING_TEXT_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-gridHeight/(CC_SD_LINES_COUNT+1)*3+CC_SD_TEXT_HEIGHT, QString::number(CC_SD_DELAY_MAX/(CC_SD_LINES_COUNT+1)*3));
        }
    }

    void StatisticsDisplayer::drawCurves(QPainter& p) const
    {
        const int gridHeight = height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-CC_SD_PADDING_TOP-CC_SD_TEXT_HEIGHT;
        const int gridWidth  = width()-CC_SD_MARGIN_RIGHT-CC_SD_PADDING_LEFT-CC_SD_GRID_LEFT;

        // Retrieving statistics from the robot
        QList<Robot::StatisticsInformation > robotCommunicatorStatistics = robot->GetStatistics();
        float pointStep = (float)gridWidth / (float)CC_SD_GRID_MAX_POINTS;
        if(pointStep==0) pointStep=1;

        // To easily draw curves, we put values in a QPolygon object
        QPolygon rssiPoints;
        QPolygon lqiPoints;
        QPolygon delayPoints;
        int sumRSSI  = 0;
        int sumLQI   = 0;
        int sumDelay = 0;
        
        int index = 0;
        foreach(Robot::StatisticsInformation statistics, robotCommunicatorStatistics)
        {
            rssiPoints.append(QPoint(index*pointStep+CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-statistics.RSSI*gridHeight/CC_SD_RSSI_MAX));
            sumRSSI+=statistics.RSSI;
            lqiPoints.append(QPoint(index*pointStep+CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-statistics.LQI*gridHeight/CC_SD_LQI_MAX));
            sumLQI+=statistics.LQI;
            delayPoints.append(QPoint(index*pointStep+CC_SD_PADDING_LEFT+CC_SD_GRID_LEFT,height()-CC_SD_PADDING_BOTTOM-CC_SD_MARGIN_BOTTOM-statistics.Delay*gridHeight/CC_SD_DELAY_MAX));
            sumDelay+=statistics.Delay;
            index++;
        }
        
        // Draw polygons
        if(displayRSSI)
        {
            p.setPen(CC_SD_RSSI_PEN);
            p.drawPolyline(rssiPoints);
            // Draw mean RSSI
        }
        if(displayLQI)
        {
            p.setPen(CC_SD_LQI_PEN);
            p.drawPolyline(lqiPoints);
            // Draw mean LQI
        }
        if(displayDelay)
        {
            p.setPen(CC_SD_DELAY_PEN);
            p.drawPolyline(delayPoints);
            // Draw mean delay
        }
    }
}