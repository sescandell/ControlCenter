#include "ccControlCenterDisplayer.h"
#include <QtGui/QSizePolicy>

namespace CCF
{
    ControlCenterDisplayer::ControlCenterDisplayer(QWidget* parent)
        :QWidget(parent), tool(false)
    {
        setMinimumHeight(10);
        setMinimumWidth(10);
        setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    }

    void ControlCenterDisplayer::EnableTool()
    {
        tool = true;
    }

    void ControlCenterDisplayer::DisableTool()
    {
        tool = false;
    }

    void ControlCenterDisplayer::mouseDoubleClickEvent(QMouseEvent* ev)
    {
        if(tool)
            emit doubleClicked(ev->pos());
    }

    void ControlCenterDisplayer::mouseMoveEvent(QMouseEvent* ev)
    {
        if(tool)
            emit mouseMoved(ev->pos());
    }

    void ControlCenterDisplayer::mousePressEvent(QMouseEvent* ev)
    {
        if(tool)
            emit clicked(ev->pos());
    }

    void ControlCenterDisplayer::resizeEvent(QResizeEvent* ev)
    {
        QWidget::resizeEvent(ev);
        emit resized(size());
    }

    void ControlCenterDisplayer::paintEvent(QPaintEvent */*ev*/)
    {
        if(isVisible())
        {
            QPainter painter(this);
            emit painting(painter);
        }
    }

} // namspace CCF