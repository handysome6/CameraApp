#include "mainwidget.h"
#include "constants.h"
#include <QApplication>
#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[])
{
    int hflag = 0;
    // int flip_flag = 0;
    int c;


    while ((c = getopt (argc, argv, "fh:")) != -1)
        switch (c)
        {
            case('h'):
                hflag = 1;
                break;
            case('f'):
                // flip_flag = 1;
                break;
            default:
                exit(1);
        }

    if (hflag)
    {
        std::cout << "Showing help message. " << std::endl 
            << "This is a camera app for taking photo and measuring length." << std::endl
            << "\t -h: print help message" << std::endl
            << "\t -f: flip camera image " << std::endl;
        exit(0);
    }



    QApplication a(argc, argv);
    MainWidget w;
    w.show();

    return a.exec();
}
