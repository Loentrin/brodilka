#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QVector>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class ScribbleArea : public QWidget
{
    Q_OBJECT

public:
    ScribbleArea(QWidget *parent = nullptr);
    int pCount;
    void resetGame();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    int player;
    int canRoll, isRolling;
    int diceValue;
    int diceTimeRemaining, movesRemaining;
    QPointF p1CurrentPos, p2CurrentPos, p3CurrentPos;
    bool isMovingAnimation = false;
    bool skipTurn[3] = {false, false, false};
    int streamAnimationStep = 0;
    float frogSubmersion = 0.0f;
    int teleportPhase = 0;
    int p1TileX, p1TileY, p2TileX, p2TileY, p3TileX, p3TileY;
    int p1BackDir, p2BackDir, p3BackDir;

    void calculateLayout();

    QRect fieldRect;
    QRect infoRect;
    QRect diceRect;
    int cellSize;
    int offsetX, offsetY;

    bool gameOver = false;
    int winner = -1;

    QWidget *winMenuWidget;
    QLabel *winLabel;
    QPushButton *restartButton;
    QPushButton *quitButton;

    void showWinMenu();
    void updateWinMenuSize();

    QVector<QString> grid = {
        "11210001",
        "10010001",
        "14031011",
        "01001010",
        "02012010",
        "03040041",
        "01013001",
        "51001121"};
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
