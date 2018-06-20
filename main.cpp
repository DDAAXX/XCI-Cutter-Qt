#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "worker.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qmlRegisterType<Worker>("comp.xcicutter", 1, 0, "Worker");
    qmlRegisterType<XCIFile>();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Worker wrk(engine.rootObjects().at(0));

    engine.rootContext()->setContextProperty("worker", &wrk);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
