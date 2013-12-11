#include <QtGui/QApplication>
#include "ccControlCenterGUI.h"


using namespace CCF;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CCF::ControlCenterGUI window;
    
    return a.exec();
}