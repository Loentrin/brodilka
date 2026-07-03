#include "scribblearea.h"

#include <QMouseEvent>
#include <QPainter>
#include <QMessageBox>
#include <QPixmap>

ScribbleArea::ScribbleArea(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    player = 0;
    canRoll = 1;
    isRolling = 0;
    diceValue = 1;
    p1Pos.setX(10+50);
    p1Pos.setY(7*80-20+50);
    p2Pos.setX(30+50);
    p2Pos.setY(7*80+20+50);
    p1TileX = 0;
    p1TileY = 7;
    p1BackDir = -1;
    p2TileX = 0;
    p2TileY = 7;
    p2BackDir = -1;
    /*
      0
  3      1
      2
     */
}

void ScribbleArea::mousePressEvent(QMouseEvent *event){
    if(!canRoll) return;
    if(event->pos().x() > 800 && event->pos().y() > 150 && event->pos().x() < 800+280 && event->pos().y() < 150+80){
        canRoll = 0;
        isRolling = 1;
        diceTimeRemaining = 7;
        startTimer(200);
    }
}

//! [12] //! [13]
void ScribbleArea::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::black));
    QPixmap pixmapFrog1(":img/img/frog1.png");
    QPixmap pixmapFrog2(":img/img/frog2.png");
    QPixmap pixmapLilypad(":img/img/lilypad.png");
    QPixmap pixmapWater(":img/img/water.png");
    painter.drawPixmap(0,0,740,740, pixmapWater);
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(grid[i][j] == '1'){
                painter.translate(i*80+50+40,j*80+50+40);
                painter.rotate((i+j)*90);
                painter.drawPixmap(-40,-40,80,80, pixmapLilypad);
                painter.rotate(-(i+j)*90);
                painter.translate(-(i*80+50+40),-(j*80+50+40));
            }
        }
    }

    QSize frogSize(50,50);
    painter.drawPixmap(QRect(p1Pos, frogSize), pixmapFrog1);
    painter.drawPixmap(QRect(p2Pos, frogSize), pixmapFrog2);

    QPoint dicePos(900, 350);
    QSize diceSize(150, 150);
    QSize dotSize(10,10);
    painter.setFont(QFont("Helvetica", 30));
    painter.setPen(QPen(Qt::white));
    painter.drawText(QPoint(800, 100), "Игрок " + QString::number(player+1) + " ходит!");
    painter.setPen(QPen(Qt::black));

    painter.setBrush(QColor(Qt::red));
    painter.drawRect(QRect(800, 150, 280, 80));
    painter.drawText(QPoint(810, 200), "Бросить кубик");

    if(player == 0){
        painter.setBrush(QColor(Qt::green));
    }
    else{
        painter.setBrush(QColor(Qt::blue));
    }
    painter.drawRect(QRect(dicePos, diceSize));
    painter.setBrush(QColor(Qt::white));
    if(diceValue == 1){
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/2-dotSize.width(), dicePos.y()+diceSize.height()/2-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
    }
    if(diceValue == 2){
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
    }
    if(diceValue == 3){
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/2-dotSize.width(), dicePos.y()+diceSize.height()/2-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
    }
    if(diceValue == 4){
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
    }
    if(diceValue == 5){
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/2-dotSize.width(), dicePos.y()+diceSize.height()/2-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
    }
    if(diceValue == 6){
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()*3/4-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/4-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/2-dotSize.width(), dicePos.y()+diceSize.height()/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
        painter.drawEllipse(QRect(dicePos.x()+diceSize.width()/2-dotSize.width(), dicePos.y()+diceSize.height()*3/4-dotSize.height(), dotSize.width()*2, dotSize.height()*2));
    }
}


void ScribbleArea::timerEvent(QTimerEvent* event)
{
    bool moved;
    if(isRolling){
        diceTimeRemaining--;
        diceValue = rand()%5+1;
        if(diceTimeRemaining < 1){
            isRolling = 0;
            movesRemaining = diceValue;
            startTimer(400);
            killTimer(event->timerId());
        }
    }
    else{
        movesRemaining--;
        if(movesRemaining < 0){
            canRoll = 1;
            player = 1-player;
            killTimer(event->timerId());
            repaint();
            return;
        }
        moved = false;
        if(player == 0){
            if(p1TileX != 0 && !moved && p1BackDir != 3){
                if(grid[p1TileX-1][p1TileY] == '1'){
                    p1TileX--;
                    p1Pos -= QPoint(80,0);
                    p1BackDir = 1;
                    moved = true;
                }
            }
            if(p1TileX != 7 && !moved && p1BackDir != 1){
                if(grid[p1TileX+1][p1TileY] == '1'){
                    p1TileX++;
                    p1Pos += QPoint(80,0);
                    p1BackDir = 3;
                    moved = true;
                }
            }
            if(p1TileY != 0 && !moved && p1BackDir != 0){
                if(grid[p1TileX][p1TileY-1] == '1'){
                    p1TileY--;
                    p1Pos -= QPoint(0,80);
                    p1BackDir = 2;
                    moved = true;
                }
            }
            if(p1TileY != 7 && !moved && p1BackDir != 2){
                if(grid[p1TileX][p1TileY+1] == '1'){
                    p1TileY++;
                    p1Pos += QPoint(0,80);
                    p1BackDir = 0;
                    moved = true;
                }
            }
        }
        moved = false;
        if(player == 1){
            if(p2TileX != 0 && !moved && p2BackDir != 3){
                if(grid[p2TileX-1][p2TileY] == '1'){
                    p2TileX--;
                    p2Pos -= QPoint(80,0);
                    p2BackDir = 1;
                    moved = true;
                }
            }
            if(p2TileX != 7 && !moved && p2BackDir != 1){
                if(grid[p2TileX+1][p2TileY] == '1'){
                    p2TileX++;
                    p2Pos += QPoint(80,0);
                    p2BackDir = 3;
                    moved = true;
                }
            }
            if(p2TileY != 0 && !moved && p2BackDir != 0){
                if(grid[p2TileX][p2TileY-1] == '1'){
                    p2TileY--;
                    p2Pos -= QPoint(0,80);
                    p2BackDir = 2;
                    moved = true;
                }
            }
            if(p2TileY != 7 && !moved && p2BackDir != 2){
                if(grid[p2TileX][p2TileY+1] == '1'){
                    p2TileY++;
                    p2Pos += QPoint(0,80);
                    p2BackDir = 0;
                    moved = true;
                }
            }
        }

    }

    repaint();
}