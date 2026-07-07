#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
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
    //void keyPressEvent(QKeyEvent* event);

private slots:
    void startGame(int players);
    void quitApp();

private:
    ScribbleArea *scribbleArea;
    QWidget *menuWidget;
    QPushButton *btn2Players;
    QPushButton *btn3Players;
    QPushButton *btnQuit;
    QLabel *name;
};
#endif
