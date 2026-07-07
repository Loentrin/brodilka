#include "scribblearea.h"

#include <QMouseEvent>
#include <QPainter>
#include <QMessageBox>
#include <QPixmap>
#include <QChar>

ScribbleArea::ScribbleArea(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    player = 0;
    canRoll = 1;
    isRolling = 0;
    diceValue = 1;
    // p1Pos.setX(10+50);
    // p1Pos.setY(7*80-20+50);
    // p2Pos.setX(30+50);
    // p2Pos.setY(7*80+20+50);
    // p3Pos.setX(-10+50);
    // p3Pos.setY(7*80+30+50);
    p1TileX = 0;
    p1TileY = 7;
    p1BackDir = -1;
    p2TileX = 0;
    p2TileY = 7;
    p2BackDir = -1;
    p3TileX = 0;
    p3TileY = 7;
    p3BackDir = -1;
    /*
      0
  3      1
      2
     */
}

void ScribbleArea::mousePressEvent(QMouseEvent *event){
    if(!canRoll) return;
    QPoint dicePos(900, 350);
    QSize diceSize(150, 150);
    if(diceRect.contains(event->pos())){
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
    Q_UNUSED(event);
    calculateLayout();


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black));

    QPixmap pixmapFrog1(":img/img/frog1.png");
    QPixmap pixmapFrog2(":img/img/frog2.png");
    QPixmap pixmapFrog3(":img/img/frog3.png");
    QPixmap pixmapLilypad(":img/img/lilypad.png");
    QPixmap pixmapCrown(":img/img/crown.png");
    QPixmap pixmapWater(":img/img/water.png");
    painter.drawPixmap(0,0,width(),height(), pixmapWater);

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            QChar cell = grid[i][j];
            if(cell != '0'){
                QRect cellRect(offsetX + i * cellSize, offsetY + j * cellSize, cellSize, cellSize);
                painter.drawPixmap(cellRect, pixmapLilypad);


                QColor borderColor;
                bool hasBorder = false;

                if (cell == '2') {
                    borderColor = Qt::red;
                    hasBorder = true;
                } else if (cell == '3') {
                    borderColor = QColor(120, 0, 120);
                    hasBorder = true;
                }

                if (hasBorder) {
                    painter.setPen(QPen(borderColor, 4));
                    painter.setBrush(Qt::NoBrush);
                    painter.drawEllipse(cellRect.adjusted(2, 2, -2, -2));
                }

                if (i == 7 && j == 0) {
                    int crownSize = cellSize * 0.5;
                    painter.drawPixmap(offsetX + 7 * cellSize + (cellSize - crownSize) / 2, offsetY * cellSize - crownSize * 0.3, crownSize, crownSize, pixmapCrown);
                }
            }
        }
    }

    int frogSize = cellSize * 0.5;

    auto drawFrog = [&](int tileX, int tileY, const QPixmap &frog, float dx, float dy) {
        int x = offsetX + tileX * cellSize + dx * cellSize;
        int y = offsetY + tileY * cellSize + dy * cellSize;
        painter.drawPixmap(x, y, frogSize, frogSize, frog);
    };

    drawFrog(p1TileX, p1TileY, pixmapFrog1, 0.1f, 0.1f);
    drawFrog(p2TileX, p2TileY, pixmapFrog2, 0.5f, 0.1f);
    if (pCount > 2) {
        drawFrog(p3TileX, p3TileY, pixmapFrog3, 0.3f, 0.5f);
    }

    painter.fillRect(infoRect, QColor(0, 0, 0, 150));

    painter.setFont(QFont("Comic Sans MS", 20));
    painter.setPen(QPen(Qt::white));

    QString turnText;

    if (player == 0) turnText = "Зелёный\n игрок!";
    else if (player == 1) turnText = "Синий\n игрок!";
    else turnText = "Оранжевый\n игрок!";

    painter.drawText(infoRect.adjusted(10, 20, -10, -10), Qt::AlignTop | Qt::AlignHCenter, turnText);

    painter.setPen(QPen(Qt::black));

    if (player == 0) painter.setBrush(Qt::green);
    else if (player == 1) painter.setBrush(Qt::blue);
    else painter.setBrush(QColor("orange"));
    painter.drawRect(diceRect);

    //painter.setBrush(QColor(Qt::gray));
    //painter.drawRect(QRect(750,640,200,100));

    painter.setBrush(QColor(Qt::white));

    int dotSize = diceRect.width() / 10;

    QPoint center = diceRect.center();

    auto drawDot = [&](int col, int row) {
        int x = diceRect.x() + diceRect.width() * (col + 1) / 4;
        int y = diceRect.y() + diceRect.height() * (row + 1) / 4;
        painter.drawEllipse(QPoint(x, y), dotSize, dotSize);
    };

    switch (diceValue) {
    case 1:
        painter.drawEllipse(center, dotSize, dotSize);
        break;
    case 2:
        drawDot(0, 0);
        drawDot(2, 2);
        break;
    case 3:
        drawDot(0, 0);
        drawDot(1, 1);
        drawDot(2, 2);
        break;
    case 4:
        drawDot(0, 0);
        drawDot(2, 0);
        drawDot(0, 2);
        drawDot(2, 2);
        break;
    case 5:
        drawDot(0, 0);
        drawDot(2, 0);
        drawDot(1, 1);
        drawDot(0, 2);
        drawDot(2, 2);
        break;
    case 6:
        drawDot(0, 0);
        drawDot(2, 0);
        drawDot(0, 1);
        drawDot(2, 1);
        drawDot(0, 2);
        drawDot(2, 2);
        break;
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
            player++;
            if(player >= pCount){
                player = 0;
            }
            killTimer(event->timerId());
            repaint();
            return;
        }
        moved = false;
        if(player == 0){
            if(p1TileX != 0 && !moved && p1BackDir != 3){
                if(grid[p1TileX-1][p1TileY] == '1'){
                    p1TileX--;
                    p1BackDir = 1;
                    moved = true;
                }
            }
            if(p1TileX != 7 && !moved && p1BackDir != 1){
                if(grid[p1TileX+1][p1TileY] == '1'){
                    p1TileX++;
                    p1BackDir = 3;
                    moved = true;
                }
            }
            if(p1TileY != 0 && !moved && p1BackDir != 0){
                if(grid[p1TileX][p1TileY-1] == '1'){
                    p1TileY--;
                    p1BackDir = 2;
                    moved = true;
                }
            }
            if(p1TileY != 7 && !moved && p1BackDir != 2){
                if(grid[p1TileX][p1TileY+1] == '1'){
                    p1TileY++;
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
                    p2BackDir = 1;
                    moved = true;
                }
            }
            if(p2TileX != 7 && !moved && p2BackDir != 1){
                if(grid[p2TileX+1][p2TileY] == '1'){
                    p2TileX++;
                    p2BackDir = 3;
                    moved = true;
                }
            }
            if(p2TileY != 0 && !moved && p2BackDir != 0){
                if(grid[p2TileX][p2TileY-1] == '1'){
                    p2TileY--;
                    p2BackDir = 2;
                    moved = true;
                }
            }
            if(p2TileY != 7 && !moved && p2BackDir != 2){
                if(grid[p2TileX][p2TileY+1] == '1'){
                    p2TileY++;
                    p2BackDir = 0;
                    moved = true;
                }
            }
        }
        moved = false;
        if(player == 2){
            if(p3TileX != 0 && !moved && p3BackDir != 3){
                if(grid[p3TileX-1][p3TileY] == '1'){
                    p3TileX--;
                    p3BackDir = 1;
                    moved = true;
                }
            }
            if(p3TileX != 7 && !moved && p3BackDir != 1){
                if(grid[p3TileX+1][p3TileY] == '1'){
                    p3TileX++;
                    p3BackDir = 3;
                    moved = true;
                }
            }
            if(p3TileY != 0 && !moved && p3BackDir != 0){
                if(grid[p3TileX][p3TileY-1] == '1'){
                    p3TileY--;
                    p3BackDir = 2;
                    moved = true;
                }
            }
            if(p3TileY != 7 && !moved && p3BackDir != 2){
                if(grid[p3TileX][p3TileY+1] == '1'){
                    p3TileY++;
                    p3BackDir = 0;
                    moved = true;
                }
            }
        }

    }

    repaint();
}

void ScribbleArea::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    calculateLayout();
}

void ScribbleArea::calculateLayout()
{
    int w = width();
    int h = height();

    int infoWidth = qBound(150.0, w * 0.2, 300.0);
    infoRect = QRect(w - infoWidth, 0, infoWidth, h);

    int fieldWidth = w - infoWidth;
    cellSize = qMin(fieldWidth, h) / 8;

    offsetX = (fieldWidth - cellSize * 8) / 2;
    offsetY = (h - cellSize * 8) / 2;
    fieldRect = QRect(offsetX, offsetY, cellSize * 8, cellSize * 8);

    int diceSize = qMin(infoWidth * 0.6, h * 0.25);
    diceSize = qBound(50, diceSize, 180);
    int diceX = infoRect.x() + (infoWidth - diceSize) / 2;
    int diceY = infoRect.y() + (h - diceSize) / 2;
    diceRect = QRect(diceX, diceY, diceSize, diceSize);
}