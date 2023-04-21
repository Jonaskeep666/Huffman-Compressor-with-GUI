#include "mainwindow.h"
//#include <iostream>
//#include <cstdlib>
//#include <ctime>
//#include "huffman.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

//    CharList clist;
//    MinHeap mhp;
//    Hufftree tree;
//    Buffer dcInput;
//    Buffer ecInput;

//    qDebug() << "All done! ";

    w.show();
    return a.exec();
}
