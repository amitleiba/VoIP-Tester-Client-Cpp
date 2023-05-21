#include "GuiHandler.hpp"

//int main(int argc, char *argv[])
//{
//    GuiHandler gui(argc, argv);
//    gui.runGui();
//}

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    qRegisterMetaType<std::string>();
    QSharedPointer<MainWindow> mainWindow = QSharedPointer<MainWindow>::create();
    GuiHandler gui(mainWindow);
    gui.runGui();
    return a.exec();
}
