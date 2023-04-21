#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "huffman.hpp"

#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QString>

#include <QFileDialog>
#include <QTextBrowser>

#include <QTextStream>
#include <cstdio>
#include <ctime>

void MainWindow::timer(bool mode){
    static clock_t t1=0,t2=0;
    double total =0.0,sec=0.0;
    int min=0;

    if(mode){
        t1 = clock();
    }
    else{
        t2 = clock();
        total = ((double)(t2-t1))/CLOCKS_PER_SEC;
        min = (int)total/60;
        sec = total - min*60.0;
        ui->textBrowser->textCursor().insertText("Time: ");
        ui->textBrowser->textCursor().insertText(QString::number(min) +" m " + QString::number(sec) + "s\n" );
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
}


QString uncharToQstring (unsigned char * id, int len){
    QString temp, msg;
    int j = 0;
    while (j<len){
        temp = QString("%1") .arg ((int)id[j], 2, 16, QLatin1Char( 'O') ) ;
        msg. append (temp);
        j++;
    }
    return msg;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Huffman Coding Utility");
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);

    QString *file_path = new QString;
    connect(ui->pushButton_ofile,&QPushButton::clicked,this,[=](){
        openFile(file_path);
    });
    connect(ui->pushButton_Compress,&QPushButton::clicked,this,[=](){
        on_pushButton_Compress_clicked(file_path);
    });
    connect(ui->pushButton_Decompress,&QPushButton::clicked,this,[=](){
        on_pushButton_Decompress_clicked(file_path);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(QString* path){
    *path = QFileDialog::getOpenFileName(this);
    if(path->length()==0){
        ui->textBrowser->setText("... Path:\n" + *path + "\n...[Not a file] \n");
    }
    else {
        ui->textBrowser->setText("... Path:\n" + *path + "\n...[You selected] \n");
    }
}


void MainWindow::on_pushButton_Compress_clicked(QString* path)
{
    QTextStream cin(stdin,  QIODevice::ReadOnly);
    QTextStream cout(stdout,  QIODevice::WriteOnly);
    QTextStream cerr(stderr,  QIODevice::WriteOnly);

    QString file_name;
    QFileInfo file_info;
    file_info = QFileInfo(*path);
    file_name = file_info.fileName();
    qint64 file_size = file_info.size();
    QString sfile_size = QString::number(file_size);

    ui->textBrowser->append("...\n... file_size:"+ sfile_size + "\n");

    QFile mFile(*path);
    if(!mFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for reading";
        return;
    }
    int mfileHandle = mFile.handle();

    // 壓縮 步驟 1：將 原始檔案的「大小」「內容」存入 dcInput.CharArr 中
    timer(true);
    ui->progressBar->setValue(5);
    ui->textBrowser->textCursor().insertText("Step1: Reading File...\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    FILE *dptr = fdopen(mfileHandle, "rb");

    CharList clist;
    MinHeap mhp;
    Hufftree tree;
    Buffer dcInput;

    unsigned char buff = 0;

    dcInput.createCharArr(file_size);

    for(int i=0;i<file_size;++i){
            fread(&buff,1,1,dptr);
            dcInput.charArr[i] = buff;
    }
    ui->textBrowser->textCursor().insertText("\n");

    fclose(dptr);
    mFile.close();

    // 壓縮 步驟 2：建立 Huffman Tree + 生成 編碼表，並根據編碼表 壓縮資料 存入 buffer
    ui->progressBar->setValue(35);
    ui->textBrowser->textCursor().insertText("Step2: Building Huffman Tree & Encoding...\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    // Phase 1：建立 Huffman Tree
    clist.buildFreqTable(dcInput);
    // clist.printFtable();
    // cout << clist <<endl;
    mhp.buildHeap(clist);

    // cout << mhp <<endl;
    mhp.buildHufftree(tree);
    // cout << mhp <<endl;

    // Phase 2：Encode 壓縮
    tree.genCode(clist);
    // clist.printFtable();
    // cout << dcInput.charArr <<endl;

    tree.encode(clist,dcInput);
    // dcInput.printBuffer();

//    // 壓縮 步驟 3：將 Header資料 & 壓縮碼 寫入 檔案中
    ui->progressBar->setValue(75);
    ui->textBrowser->textCursor().insertText("Step3: Save Encoded File...\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    QString opath = *path + ".bin";
    QFile oFile(opath);
    oFile.open(QFile::ReadOnly | QFile::Text);
    oFile.close();
    if(!oFile.open(QFile::ReadWrite | QFile::Text)){
        cout << "could not open file for writing";
        return;
    }
    int ofileHandle = oFile.handle();
    FILE *eptr = fdopen(ofileHandle, "wb+");

//    // 寫入 Freq table size[2] & Freq table [(1+freq_size)*table_size]
//    rewind(eptr);
    clist.writeFtable(eptr);

//    // 寫入 bsize[4](原檔案壓縮後 不含標頭 的 bit 數) & csize(原始檔案 bytes 數)[4]
    dcInput.writeSize(eptr);

//    // 寫入 壓縮後的資料(buffer)
    dcInput.writeEncodeBuff(eptr);

    fclose(eptr);
    oFile.close();

    int noheaderSize = (dcInput.bsize/8 + ((dcInput.bsize%8)?1:0));
    int totalSize = 11 + ((1+clist.freq_size) * clist.table_size) + noheaderSize;
    double ratio = (((double)dcInput.csize-totalSize)/dcInput.csize) * 100.0;

    ui->textBrowser->textCursor().insertText("Original File size:\t" + QString::number(dcInput.csize) +" bytes\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    ui->textBrowser->textCursor().insertText("Encoded file Size (without header):\t");
    ui->textBrowser->textCursor().insertText(QString::number(noheaderSize) +" bytes\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    ui->textBrowser->textCursor().insertText("Encoded file Size (with header):\t");
    ui->textBrowser->textCursor().insertText(QString::number(totalSize) +" bytes\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    ui->textBrowser->textCursor().insertText("Compression ratio: \t");
    ui->textBrowser->textCursor().insertText(QString::number(ratio) +" %\n");
    ui->textBrowser->moveCursor(QTextCursor::End);


    timer(false);
    ui->progressBar->setValue(100);
    ui->textBrowser->textCursor().insertText("All done!!");
    ui->textBrowser->moveCursor(QTextCursor::End);

    delete[] clist.ftable;
    delete[] dcInput.buffer;
    delete[] dcInput.charArr;
    delete[] mhp.heap;
}

void MainWindow::on_pushButton_Decompress_clicked(QString* path)
{
    QTextStream cin(stdin,  QIODevice::ReadOnly);
    QTextStream cout(stdout,  QIODevice::WriteOnly);
    QTextStream cerr(stderr,  QIODevice::WriteOnly);

    QString file_name;
    QFileInfo file_info;
    file_info = QFileInfo(*path);
    file_name = file_info.fileName();
    qint64 file_size = file_info.size();
    QString sfile_size = QString::number(file_size);

    ui->textBrowser->append("...\n... file_size:"+ sfile_size + "\n");

    QFile mFile(*path);
    if(!mFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for reading";
        return;
    }
    int mfileHandle = mFile.handle();

    // 解壓 步驟 1：將 Header資料 & 壓縮碼 讀出 並存入 Charlist & Buffer 中
    timer(true);
    ui->progressBar->setValue(5);
    ui->textBrowser->textCursor().insertText("Step1: Reading File...\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    FILE *eptr = fdopen(mfileHandle, "rb");

    CharList clist;
    MinHeap mhp;
    Hufftree tree;
    Buffer ecInput;


    // 從 FILE 讀進 Freq table size[2] & Freq table [(1+freq_size)*table_size]
//    rewind(eptr);
    clist.readFtable(eptr);
    // clist.printFtable();

    // 從 FILE 讀進 bsize[4](原檔案壓縮後 不含標頭 的 bit 數) & csize(原始檔案 bytes 數)[4]
    ecInput.readSize(eptr);

    // 配置 buffer & charArr 的空間
    ecInput.createBuffer(ecInput.bsize);
    ecInput.createCharArr(ecInput.csize);

    // 從 FILE 讀進 壓縮碼 存入 buffer
    ecInput.readEncodeBuff(eptr);
    // ecInput.printBuffer();

    fclose(eptr);
    mFile.close();

    // 解壓 步驟 2：透過 頻率表(ftable) 建立 Huffman Tree + 走訪 樹 以解碼 並存入 ecInput.CharArr
    ui->progressBar->setValue(35);
    ui->textBrowser->textCursor().insertText("Step2: Building Huffman Tree & Decoding...\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    // Phase 1：建立 Huffman Tree
    mhp.buildHeap(clist);

    // cout << mhp <<endl;
    mhp.buildHufftree(tree);
    // cout << mhp <<endl;

    // Phase 2：Decode 解碼
    tree.decode(ecInput);

    // 解壓 步驟 3：將 解碼的資料 寫入 檔案
    ui->progressBar->setValue(75);
    ui->textBrowser->textCursor().insertText("Step3: Save Decoded File...\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    QString opath = (*path).section(".",0,1);
    QFile oFile(opath);
    oFile.open(QFile::ReadOnly | QFile::Text);
    oFile.close();
    if(!oFile.open(QFile::ReadWrite | QFile::Text)){
        cout << "could not open file for writing";
        return;
    }
    int ofileHandle = oFile.handle();
    FILE *dptr = fdopen(ofileHandle, "wb+");

    ecInput.writeDecodeChar(dptr);

    fclose(dptr);
    oFile.close();

    int noheaderSize = (ecInput.bsize/8 + ((ecInput.bsize%8)?1:0));
    int totalSize = 11 + ((1+clist.freq_size) * clist.table_size) + noheaderSize;
    double ratio = (((double)ecInput.csize-totalSize)/ecInput.csize) * 100.0;

    ui->textBrowser->textCursor().insertText("Original File size:\t" + QString::number(ecInput.csize) +" bytes\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    ui->textBrowser->textCursor().insertText("Encoded file Size (without header):\t");
    ui->textBrowser->textCursor().insertText(QString::number(noheaderSize) +" bytes\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    ui->textBrowser->textCursor().insertText("Encoded file Size (with header):\t");
    ui->textBrowser->textCursor().insertText(QString::number(totalSize) +" bytes\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    ui->textBrowser->textCursor().insertText("Compression ratio: \t");
    ui->textBrowser->textCursor().insertText(QString::number(ratio) +" %\n");
    ui->textBrowser->moveCursor(QTextCursor::End);

    timer(false);
    ui->progressBar->setValue(100);
    ui->textBrowser->textCursor().insertText("All done!!");
    ui->textBrowser->moveCursor(QTextCursor::End);

    delete[] clist.ftable;
    delete[] ecInput.buffer;
    delete[] ecInput.charArr;
    delete[] mhp.heap;
}

