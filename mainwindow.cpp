#include "mainwindow.h"
#include "qpainter.h"
#include "scribblearea.h"

#include <QApplication>
#include <QColorDialog>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QVBoxLayout>

//! [0]
class MenuWidget : public QWidget {
public:
    MenuWidget(QWidget *parent = nullptr) : QWidget(parent) {}
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        QPixmap water(":img/img/water.png");
        painter.drawPixmap(rect(), water);
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scribbleArea(new ScribbleArea(this))
{
    setWindowTitle(":3");
    resize(1024, 768);
    setMinimumSize(300,300);

    isGameActive = false;

    menuWidget = new MenuWidget(this);

    btn2Players = new QPushButton("Игра для 2 игроков", menuWidget);
    btn3Players = new QPushButton("Игра для 3 игроков", menuWidget);
    btnQuit = new QPushButton("Выход", menuWidget);

    QFont titleFont("Comic Sans MS", 36, QFont::Bold);

    btn2Players->setFont(titleFont);
    btn2Players->setStyleSheet(" background-color: red");
    btn3Players->setFont(titleFont);
    btn3Players->setStyleSheet(" background-color: orange");
    btnQuit->setFont(titleFont);



    name = new QLabel("Гонка лягушек :3", menuWidget);
    name->setFont(titleFont);
    name->setStyleSheet("background-color: rgba(0,255,0,150); border-radius: 15px; padding: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(menuWidget);

    layout->addWidget(name, 0, Qt::AlignCenter);
    layout->addWidget(btn2Players, 0, Qt::AlignCenter);
    layout->addWidget(btn3Players, 0, Qt::AlignCenter);
    layout->addWidget(btnQuit, 0, Qt::AlignCenter);

    menuWidget->setLayout(layout);

    setCentralWidget(menuWidget);

    connect(btn2Players, &QPushButton::clicked, this, [this]() { startGame(2); });
    connect(btn3Players, &QPushButton::clicked, this, [this]() { startGame(3); });
    connect(btnQuit, &QPushButton::clicked, this, &MainWindow::quitApp);

    scribbleArea->hide();
}

void MainWindow::startGame(int players){
    scribbleArea->pCount = players;
    setCentralWidget(scribbleArea);
    scribbleArea->show();
    isGameActive = true;

}

void MainWindow::quitApp()
{
    close();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    if(isGameActive == false) {
        updateFontSizes();
    }

}

void MainWindow::updateFontSizes()
{
    QFont titleFont("Comic Sans MS", qMin(width(), height()) / 15, QFont::Bold);

     name->setFont(titleFont);
    btn2Players->setFont(titleFont);
    btn3Players->setFont(titleFont);
    btnQuit->setFont(titleFont);
}