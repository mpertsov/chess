#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include "boardstate.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    BoardState boardState;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("boardState", &boardState);
    qmlRegisterType<Figure>("com.mpertsov.chess", 1, 0, "Figure");
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    try {
        return app.exec();
    } catch(const std::bad_alloc &) {
        return 0;
    }
}
