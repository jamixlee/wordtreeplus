//#include <QtGui/QApplication>
#include <QApplication>
#include "MainWindow.h"
#include "SearchHistory.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    SearchHistory rSearchHistoryWindow;

    w.SetSearchHistoryWindow(&rSearchHistoryWindow);

    w.show();

    return a.exec();
}
