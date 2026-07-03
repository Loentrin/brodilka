#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QPushButton>

class ScribbleArea;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent* event);
private:
    ScribbleArea *scribbleArea;
};
#endif
