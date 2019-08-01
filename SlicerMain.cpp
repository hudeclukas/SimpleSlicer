#include <QApplication>

#include "SlicerGUI.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    SlicerGUI guiMain;
    guiMain.show();

    return app.exec();
}
