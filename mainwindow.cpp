#include "mainwindow.h"
#include "qpainter.h"
#include "ui_mainwindow.h"
#include "scribblearea.h"

#include <QApplication>
#include <QColorDialog>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QCloseEvent>

//! [0]
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scribbleArea(new ScribbleArea(this))
{
    //setCentralWidget(scribbleArea);
    scribbleArea->hide();
    scribbleArea->pCount = 2;
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(":3");
    ui->setupUi(this);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    /*switch (event->key()) {
    case Qt::Key_S:
        scribbleArea->pauseBall();
        break;
    case Qt::Key_B:
        scribbleArea->unpauseBall();
        break;
    case Qt::Key_Equal:
        scribbleArea->speedUpBall();
        break;
    case Qt::Key_Minus:
        scribbleArea->slowDownBall();
        break;
    default:
        QWidget::keyPressEvent(event);
    }*/
}

void QWidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPixmap pixmapWater(":img/img/water.png");
    painter.drawPixmap(0,0,740,740, pixmapWater);
}
void MainWindow::on_pushButton_clicked()
{
    setCentralWidget(scribbleArea);
    scribbleArea->show();
}


void MainWindow::on_pushButton_3_clicked()
{
    close();
}


void MainWindow::on_pushButton_2_clicked()
{
    scribbleArea->pCount = 3;
    setCentralWidget(scribbleArea);
    scribbleArea->show();
}

