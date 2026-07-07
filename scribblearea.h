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
    int pCount;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent* event) override;
    //void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    int player;
    int canRoll, isRolling;
    int diceValue;
    int diceTimeRemaining, movesRemaining;
    QPoint p1Pos, p2Pos, p3Pos;
    int p1TileX, p1TileY, p2TileX, p2TileY, p3TileX, p3TileY;
    int p1BackDir, p2BackDir, p3BackDir;

    void calculateLayout();

    QRect fieldRect;
    QRect infoRect;
    QRect diceRect;
    int cellSize;
    int offsetX, offsetY;

    QVector<QString> grid = {
    "11210001",
    "10010001",
    "13031011",
    "01001010",
    "02012010",
    "03030031",
    "01013001",
    "11001121"
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
