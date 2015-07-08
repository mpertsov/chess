TEMPLATE = app

QT += qml quick widgets

SOURCES += src/main.cpp \
    src/boardstate.cpp \
    src/figure.cpp

RESOURCES += qml.qrc

DESTDIR = ./build

HEADERS += \
    src/boardstate.h \
    src/figurecolor.h \
    src/figure.h \
    src/figuretype.h \
    src/statetype.h \
    src/figureturn.h
