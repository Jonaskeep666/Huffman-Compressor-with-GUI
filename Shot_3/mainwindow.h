#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void timer(bool mode);

private slots:
    void openFile(QString* path);

    void on_pushButton_Compress_clicked(QString* path);

    void on_pushButton_Decompress_clicked(QString* path);

private:
    Ui::MainWindow *ui;
    QAction *openAction;
};
#endif // MAINWINDOW_H
