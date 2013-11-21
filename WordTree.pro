#-------------------------------------------------
#
# Project created by QtCreator 2011-04-01T20:50:25
#
#-------------------------------------------------

#QT       += core gui
QT       += widgets

TARGET = WordTree
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    WTBackend.cpp \
    WTTreeVisualizer.cpp \
    WTVisualizedTree.cpp \
    SearchHistory.cpp \
    WTWordTreeView.cpp

HEADERS  += MainWindow.h \
    WTBackend.h \
    WTTreeVisualizer.h \
    WTVisualizedTree.h \
    SearchHistory.h \
    WTWordTreeView.h

FORMS    += MainWindow.ui \
    SearchHistory.ui

OTHER_FILES += \
    Doxyfile \
    Document.txt
