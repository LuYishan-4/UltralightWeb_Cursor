#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "SettingsBackend.hpp"


int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    SettingsBackend backend;

    engine.rootContext()->setContextProperty(
        QStringLiteral("backend"),
        &backend
    );

    engine.load(
        QUrl(QStringLiteral(
            "qrc:/CursorFXSettings/Main.qml"
        ))
    );

    if(engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}