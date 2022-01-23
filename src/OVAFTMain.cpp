#include <QApplication>
#include "OVAFTMainWindow.h"

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    OVAFTMainWindow mainWindow;
    mainWindow.show();
    return QApplication::exec();
}
