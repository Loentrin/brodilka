#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class ScribbleArea;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void resizeEvent(QResizeEvent *event) override;
public:
    MainWindow(QWidget *parent = nullptr);

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

    void updateFontSizes();
    bool isGameActive;
};
#endif
