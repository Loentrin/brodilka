#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ScribbleArea;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow *ui;
    void keyPressEvent(QKeyEvent* event);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    ScribbleArea *scribbleArea;
};
#endif
