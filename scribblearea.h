#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QVector>

class ScribbleArea : public QWidget
{
    Q_OBJECT

public:
    ScribbleArea(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent* event) override;
    //void keyPressEvent(QKeyEvent* event) override;
private:
    int player;
    int canRoll, isRolling;
    int diceValue;
    int diceTimeRemaining, movesRemaining;
    QPoint p1Pos, p2Pos;
    int p1TileX, p1TileY, p2TileX, p2TileY;
    int p1BackDir, p2BackDir;
    QVector<QString> grid = {
    "11110001",
    "10010001",
    "11011011",
    "01001010",
    "01011010",
    "01010011",
    "01011001",
    "11001111"
    };
    /*
    "11100001",
    "10111111",
    "10000000",
    "11101110",
    "00111011",
    "00000001",
    "00011101",
    "11110111"
*/
};

#endif
