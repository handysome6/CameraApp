#include "guimeasure.h"

#include <QApplication>
#include <string>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::string filename = "/home/andyls/Project1/Project1/resources/sbs_39.jpg";
    const char* model_path = "/home/andyls/Project1/Project1/resources/camera_model.json";
    GuiMeasure w(filename, model_path);
    w.show();
    return a.exec();
}
