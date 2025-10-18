#include <QApplication>
#include "settingswindow.h"
#include <QDebug>

int main(int argc, char *argv[]) {
    qDebug()<< "App is starting";
    QApplication app(argc, argv);
    SettingsWindow window;
    window.show();
    // Сохраняем настройки при закрытии приложения
    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        qDebug() << "App closing, ensuring settings are saved...";
    });
    return app.exec();
}
