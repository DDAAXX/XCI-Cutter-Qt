#include <QtGlobal>

#ifdef Q_OS_LINUX
    #include <QApplication>
#endif

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>

#include "worker.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef Q_OS_LINUX
    QApplication  app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif
    qmlRegisterType<Worker>("comp.xcicutter", 1, 0, "Worker");
    qmlRegisterType<XCIFile>();

    QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Worker wrk(engine.rootObjects().at(0));

    engine.rootContext()->setContextProperty("worker", &wrk);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
