#include <QApplication>
#include "settingswindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SettingsWindow window;
    window.show();
    
    return app.exec();
}