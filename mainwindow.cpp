#include "mainwindow.h"
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
    : QMainWindow(parent), scribbleArea(new ScribbleArea(this))
{
    setCentralWidget(scribbleArea);
    setWindowState(Qt::WindowMaximized);
    setWindowTitle(":3");
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