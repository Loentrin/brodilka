#include "scribblearea.h"

#include <QChar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QApplication>

// начальные значения и меню победы
ScribbleArea::ScribbleArea(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    player = 0;
    canRoll = 1;
    isRolling = 0;
    diceValue = 1;
    p1TileX = 0;
    p1TileY = 7;
    p1BackDir = -1;
    p2TileX = 0;
    p2TileY = 7;
    p2BackDir = -1;
    p3TileX = 0;
    p3TileY = 7;
    p3BackDir = -1;
    startTimer(30);


    winMenuWidget = new QWidget(this);
    winMenuWidget->hide();
    winMenuWidget->setStyleSheet("background-color: rgba(0, 0, 0, 180); border-radius: 20px;");

    QVBoxLayout *winLayout = new QVBoxLayout(winMenuWidget);
    winLayout->setSpacing(20);

    winLabel = new QLabel("ПОБЕДА!", winMenuWidget);
    winLabel->setFont(QFont("Comic Sans MS", 20, QFont::Bold));
    winLabel->setStyleSheet("color: gold; background: transparent;");
    winLabel->setAlignment(Qt::AlignCenter);

    restartButton = new QPushButton("Играть снова", winMenuWidget);
    restartButton->setFont(QFont("Comic Sans MS", 24, QFont::Bold));
    restartButton->setStyleSheet("background-color: green; color: white; border-radius: 15px; padding: 15px;");

    quitButton = new QPushButton("Выйти", winMenuWidget);
    quitButton->setFont(QFont("Comic Sans MS", 24, QFont::Bold));
    quitButton->setStyleSheet("background-color: red; color: white; border-radius: 15px; padding: 15px;");

    winLayout->addWidget(winLabel);
    winLayout->addWidget(restartButton);
    winLayout->addWidget(quitButton);

    connect(restartButton, &QPushButton::clicked, this, &ScribbleArea::resetGame);
    connect(quitButton, &QPushButton::clicked, this, [this]() { QApplication::quit();
    });
    /*
      0
  3      1
      2
     */
}

void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
    if (!canRoll)
        return;
    QPoint dicePos(900, 350);
    QSize diceSize(150, 150);
    if (diceRect.contains(event->pos())) {
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
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black));

    QPixmap pixmapFrog1(":img/img/frog1.png");
    QPixmap pixmapFrog2(":img/img/frog2.png");
    QPixmap pixmapFrog3(":img/img/frog3.png");
    QPixmap pixmapLilypad(":img/img/lilypad.png");
    QPixmap pixmapCrown(":img/img/crown.png");
    QPixmap pixmapWater(":img/img/water.png");
    painter.drawPixmap(0, 0, width(), height(), pixmapWater);

    // течение
    auto drawWaveStream = [&](int fromX, int fromY, int toX, int toY, QColor waveColor) {
        QPointF startPt(offsetX + fromX * cellSize + cellSize / 2.0, offsetY + fromY * cellSize + cellSize / 2.0);
        QPointF endPt(offsetX + toX * cellSize + cellSize / 2.0, offsetY + toY * cellSize + cellSize / 2.0);

        QPointF direction = endPt - startPt;
        double distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        double angle = std::atan2(direction.y(), direction.x());

        painter.save();

        painter.translate(startPt);
        painter.rotate(angle * 180.0 / M_PI);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::NoBrush);

        double waveLength = cellSize * 0.8;
        double waveAmplitude = cellSize * 0.08;

        // 1. волны
        for (int w = 0; w < 3; ++w) {
            QPainterPath wavePath;
            wavePath.moveTo(0, 0);

            double phaseShift = (streamAnimationStep * 0.15) + (w * 1.5);
            float thickness = (w == 1) ? 3.0f : 1.5f;

            QColor currentWaveColor = waveColor;
            if (w != 1) currentWaveColor.setAlpha(waveColor.alpha() * 0.6);

            painter.setPen(QPen(currentWaveColor, thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            for (double x = 0; x <= distance; x += 5.0) {
                double y = waveAmplitude * std::sin((x / waveLength) - phaseShift);
                wavePath.lineTo(x, y);
            }
            painter.drawPath(wavePath);
        }

        // 2. пузырьки (Г-
        painter.setPen(QPen(QColor(255, 255, 255, 200), 1.5));
        painter.setBrush(QBrush(QColor(255, 255, 255, 80)));

        int bubbleCount = 4;
        for (int k = 0; k < bubbleCount; ++k) {
            double progress = (streamAnimationStep / 100.0) + ((double)k / bubbleCount);
            if (progress > 1.0) progress -= 1.0;
            double bubbleX = distance * progress;
            double centralPhaseShift = streamAnimationStep * 0.15;
            double bubbleY = waveAmplitude * std::sin((bubbleX / waveLength) - centralPhaseShift);
            double bubbleRadius = cellSize * (0.04 + 0.02 * (k % 2));
            painter.drawEllipse(QPointF(bubbleX, bubbleY), bubbleRadius, bubbleRadius);
        }

        painter.restore();
    };


    QColor neonWaterColor(0, 191, 255, 150);

    drawWaveStream(5, 1, 5, 3, neonWaterColor);
    drawWaveStream(2, 3, 2, 1, neonWaterColor);
    drawWaveStream(6, 4, 5, 6, neonWaterColor);

    // кувшинки
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            QChar cell = grid[i][j];
            if (cell != '0') {
                QRect cellRect(offsetX + i * cellSize, offsetY + j * cellSize, cellSize, cellSize);
                painter.drawPixmap(cellRect, pixmapLilypad);

                QColor borderColor;
                bool hasBorder = false;

                if (cell == '2') {
                    borderColor = Qt::red;
                    hasBorder = true;
                    painter.save();
                    painter.setPen(Qt::NoPen);
                    painter.restore();
                } else if (cell == '3' || cell == '4') {
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
                    painter.drawPixmap(offsetX + 7 * cellSize + (cellSize - crownSize) / 2, offsetY - crownSize * 0.3, crownSize, crownSize, pixmapCrown);

                }
            }
        }
    }

    int frogSize = cellSize * 0.5;

  // погружение
    auto drawSingleFrog = [&](int id, const QPointF& currentPos, const QPixmap& pixmap) {
        if (player == id && teleportPhase > 0) {
            int srcHeight = pixmap.height();
            int croppedSrcHeight = srcHeight * (1.0f - frogSubmersion);
            int currentFrogHeight = frogSize * (1.0f - frogSubmersion);

            if (currentFrogHeight > 0 && croppedSrcHeight > 0) {
                  QPixmap visiblePart = pixmap.copy(0, 0, pixmap.width(), croppedSrcHeight);
                painter.drawPixmap(currentPos.x(), currentPos.y(), frogSize, currentFrogHeight, visiblePart);
            }
        } else {
            painter.drawPixmap(currentPos.x(), currentPos.y(), frogSize, frogSize, pixmap);
        }
    };
    // лягушки
    drawSingleFrog(0, p1CurrentPos, pixmapFrog1);
    drawSingleFrog(1, p2CurrentPos, pixmapFrog2);
    if (pCount > 2) {
        drawSingleFrog(2, p3CurrentPos, pixmapFrog3);
    }
    painter.fillRect(infoRect, QColor(0, 0, 0, 150));
    painter.setFont(QFont("Comic Sans MS", 20));
    painter.setPen(QPen(Qt::white));

    QString turnText;

    if (player == 0)
        turnText = "Зелёный\n игрок!";
    else if (player == 1)
        turnText = "Синий\n игрок!";
    else
        turnText = "Оранжевый\n игрок!";

    painter.drawText(infoRect.adjusted(10, 20, -10, -10), Qt::AlignTop | Qt::AlignHCenter, turnText);

    painter.setPen(QPen(Qt::black));

    if (player == 0)
        painter.setBrush(Qt::green);
    else if (player == 1)
        painter.setBrush(Qt::blue);
    else
        painter.setBrush(QColor("orange"));
    painter.drawRect(diceRect);
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

    // Мошки
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::black));


    for (int id = 0; id < pCount; ++id) {
        if (skipTurn[id]) {
            QPointF frogPos = (id == 0) ? p1CurrentPos : ((id == 1) ? p2CurrentPos : p3CurrentPos);
            QPointF swarmCenter(frogPos.x() + frogSize / 2.0, frogPos.y() - cellSize * 0.1);

            int flyCount = 5;
            for (int f = 0; f < flyCount; ++f) {
                double timeFactor = streamAnimationStep * 0.3 + (f * 2.5);
                double radiusX = (cellSize * 0.08) + std::sin(timeFactor * 0.7) * 5;
                double radiusY = (cellSize * 0.05) + std::cos(timeFactor * 1.1) * 3;

                double flyX = swarmCenter.x() + std::cos(timeFactor) * radiusX;
                double flyY = swarmCenter.y() + std::sin(timeFactor) * radiusY;

                double flySize = 1.5 + (f % 2);

                painter.drawEllipse(QPointF(flyX, flyY), flySize, flySize);
            }
        }
    }
    painter.restore();

}

void ScribbleArea::timerEvent(QTimerEvent *event)
{
    streamAnimationStep = (streamAnimationStep + 1) % 100;
    if (!isRolling && !isMovingAnimation && teleportPhase == 0) {
        update();
    }

    QPointF p1Target(offsetX + p1TileX * cellSize + 0.1f * cellSize, offsetY + p1TileY * cellSize + 0.1f * cellSize);
    QPointF p2Target(offsetX + p2TileX * cellSize + 0.5f * cellSize, offsetY + p2TileY * cellSize + 0.1f * cellSize);
    QPointF p3Target(offsetX + p3TileX * cellSize + 0.3f * cellSize, offsetY + p3TileY * cellSize + 0.5f * cellSize);

    // телепорт
    if (teleportPhase > 0) {
        int currentTileX = (player == 0) ? p1TileX : ((player == 1) ? p2TileX : p3TileX);
        int currentTileY = (player == 0) ? p1TileY : ((player == 1) ? p2TileY : p3TileY);
        if (teleportPhase == 1) {
            frogSubmersion += 0.04f;
            if (frogSubmersion >= 0.6f) {
                frogSubmersion = 0.6f;
                teleportPhase = 2;

                int nextX = currentTileX; int nextY = currentTileY;
                int targetBackDir = -1;

                if ((currentTileX == 5 && currentTileY == 1) ) {
                    nextX = 5; nextY = 3;
                    targetBackDir = 1;
                }
                else if ((currentTileX == 2 && currentTileY == 3)) {
                    nextX = 2; nextY = 1;
                    targetBackDir = 2;
                }
                else if ((currentTileX == 6 && currentTileY == 4)) {
                    nextX = 5; nextY = 6;
                    targetBackDir = 3;
                }
                if (player == 0) { p1TileX = nextX; p1TileY = nextY; p1BackDir = targetBackDir; }
                else if (player == 1) { p2TileX = nextX; p2TileY = nextY; p2BackDir = targetBackDir; }
                else if (player == 2) { p3TileX = nextX; p3TileY = nextY; p3BackDir = targetBackDir; }
            }
            update();
            return;
        }

        if (teleportPhase == 2) {
            p1CurrentPos += (p1Target - p1CurrentPos) * 0.1;
            p2CurrentPos += (p2Target - p2CurrentPos) * 0.1;
            p3CurrentPos += (p3Target - p3CurrentPos) * 0.1;

            QPointF current = (player == 0) ? p1CurrentPos : ((player == 1) ? p2CurrentPos : p3CurrentPos);
            QPointF target  = (player == 0) ? p1Target     : ((player == 1) ? p2Target     : p3Target);

            if (QPointF(target - current).manhattanLength() < 4.0) {
                teleportPhase = 3;
            }
            update();
            return;
        }

        if (teleportPhase == 3) {
            frogSubmersion -= 0.04f;
            if (frogSubmersion <= 0.0f) {
                frogSubmersion = 0.0f;
                teleportPhase = 0;

                canRoll = 1;
                do {
                    player++;
                    if (player >= pCount) player = 0;
                    if (skipTurn[player]) skipTurn[player] = false;
                    else break;
                } while (true);

                killTimer(event->timerId());
            }
            update();
            return;
        }
    }

    // обычный ход
    if (isRolling) {
        diceTimeRemaining--;
        diceValue = rand() % 6 + 1;
        if (diceTimeRemaining < 1) {
            isRolling = 0;
            movesRemaining = diceValue;
            isMovingAnimation = true;
            startTimer(30);
            killTimer(event->timerId());
        }
    }
    else if (isMovingAnimation) {
        p1CurrentPos += (p1Target - p1CurrentPos) * 0.6;
        p2CurrentPos += (p2Target - p2CurrentPos) * 0.6;
        p3CurrentPos += (p3Target - p3CurrentPos) * 0.6;

        QPointF current = (player == 0) ? p1CurrentPos : ((player == 1) ? p2CurrentPos : p3CurrentPos);
        QPointF target  = (player == 0) ? p1Target     : ((player == 1) ? p2Target     : p3Target);

        if (QPointF(target - current).manhattanLength() < 2.0) {
            p1CurrentPos = p1Target; p2CurrentPos = p2Target; p3CurrentPos = p3Target;
            movesRemaining--;

            if (movesRemaining < 0) {
                isMovingAnimation = false;

                int currentTileX = (player == 0) ? p1TileX : ((player == 1) ? p2TileX : p3TileX);
                int currentTileY = (player == 0) ? p1TileY : ((player == 1) ? p2TileY : p3TileY);
                QChar currentCell = grid[currentTileX][currentTileY];

                if (currentCell == '5') {
                    winner = player;
                    showWinMenu();
                    killTimer(event->timerId());
                    update();
                    return;
                }

                if (currentCell == '2') {
                    skipTurn[player] = true;
                }
                if (currentCell == '3') {
                    teleportPhase = 1;
                    frogSubmersion = 0.0;
                    update();
                    return;
                }
                canRoll = 1;
                do {
                    player++;
                    if (player >= pCount) player = 0;
                    if (skipTurn[player]) skipTurn[player] = false;
                    else break;
                } while (true);

                killTimer(event->timerId());
                update();
                return;
            }
            else {
                bool moved = false;
                auto canMoveTo = [&](int x, int y) {
                    if (x < 0 || x > 7 || y < 0 || y > 7) return false;
                    QChar c = grid[x][y];
                    return (c == '1' || c == '2' || c == '3' || c == '4' || c =='5');
                };

                if (player == 0) {
                    if (p1TileX != 0 && !moved && p1BackDir != 3 && canMoveTo(p1TileX - 1, p1TileY)) { p1TileX--; p1BackDir = 1; moved = true; }
                    if (p1TileX != 7 && !moved && p1BackDir != 1 && canMoveTo(p1TileX + 1, p1TileY)) { p1TileX++; p1BackDir = 3; moved = true; }
                    if (p1TileY != 0 && !moved && p1BackDir != 0 && canMoveTo(p1TileX, p1TileY - 1)) { p1TileY--; p1BackDir = 2; moved = true; }
                    if (p1TileY != 7 && !moved && p1BackDir != 2 && canMoveTo(p1TileX, p1TileY + 1)) { p1TileY++; p1BackDir = 0; moved = true; }
                }
                else if (player == 1) {
                    if (p2TileX != 0 && !moved && p2BackDir != 3 && canMoveTo(p2TileX - 1, p2TileY)) { p2TileX--; p2BackDir = 1; moved = true; }
                    if (p2TileX != 7 && !moved && p2BackDir != 1 && canMoveTo(p2TileX + 1, p2TileY)) { p2TileX++; p2BackDir = 3; moved = true; }
                    if (p2TileY != 0 && !moved && p2BackDir != 0 && canMoveTo(p2TileX, p2TileY - 1)) { p2TileY--; p2BackDir = 2; moved = true; }
                    if (p2TileY != 7 && !moved && p2BackDir != 2 && canMoveTo(p2TileX, p2TileY + 1)) { p2TileY++; p2BackDir = 0; moved = true; }
                }
                else if (player == 2) {
                    if (p3TileX != 0 && !moved && p3BackDir != 3 && canMoveTo(p3TileX - 1, p3TileY)) { p3TileX--; p3BackDir = 1; moved = true; }
                    if (p3TileX != 7 && !moved && p3BackDir != 1 && canMoveTo(p3TileX + 1, p3TileY)) { p3TileX++; p3BackDir = 3; moved = true; }
                    if (p3TileY != 0 && !moved && p3BackDir != 0 && canMoveTo(p3TileX, p3TileY - 1)) { p3TileY--; p3BackDir = 2; moved = true; }
                    if (p3TileY != 7 && !moved && p3BackDir != 2 && canMoveTo(p3TileX, p3TileY + 1)) { p3TileY++; p3BackDir = 0; moved = true; }
                }
            }
        }
    }

    update();
}


void ScribbleArea::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    calculateLayout();
    p1CurrentPos = QPointF(offsetX + p1TileX * cellSize + 0.1f * cellSize, offsetY + p1TileY * cellSize + 0.1f * cellSize);
    p2CurrentPos = QPointF(offsetX + p2TileX * cellSize + 0.5f * cellSize, offsetY + p2TileY * cellSize + 0.1f * cellSize);
    p3CurrentPos = QPointF(offsetX + p3TileX * cellSize + 0.3f * cellSize, offsetY + p3TileY * cellSize + 0.5f * cellSize);
    QWidget::resizeEvent(event);
    if(gameOver){
        updateWinMenuSize();
    }
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

void ScribbleArea::resetGame()
{
    player = 0;
    canRoll = 1;
    isRolling = 0;
    diceValue = 1;
    gameOver = false;
    winner = -1;
    isMovingAnimation = false;
    teleportPhase = 0;
    frogSubmersion = 0.0;
    streamAnimationStep = 0;

    p1TileX = 0; p1TileY = 7; p1BackDir = -1;
    p2TileX = 0; p2TileY = 7; p2BackDir = -1;
    p3TileX = 0; p3TileY = 7; p3BackDir = -1;

    p1CurrentPos = QPointF(offsetX + p1TileX * cellSize + 0.1f * cellSize, offsetY + p1TileY * cellSize + 0.1f * cellSize);
    p2CurrentPos = QPointF(offsetX + p2TileX * cellSize + 0.5f * cellSize, offsetY + p2TileY * cellSize + 0.1f * cellSize);
    p3CurrentPos = QPointF(offsetX + p3TileX * cellSize + 0.3f * cellSize, offsetY + p3TileY * cellSize + 0.5f * cellSize);

    for (int i = 0; i < 3; i++) {
        skipTurn[i] = false;
    }

    winMenuWidget->hide();

    update();
}
void ScribbleArea::showWinMenu()
{
    gameOver = true;

    QString winnerText;
    if (winner == 0) winnerText = "Зелёный игрок";
    else if (winner == 1) winnerText = "Синий игрок";
    else if (winner == 2) winnerText = "Оранжевый игрок";

    winLabel->setText(QString("%1\nПОБЕДИЛ!").arg(winnerText));

    updateWinMenuSize();
    winMenuWidget->show();
    winMenuWidget->raise();
}

void ScribbleArea::updateWinMenuSize()
{

    int menuWidth = qMin(width() / 2, 600);
    int menuHeight = qMin(height() / 2, 400);

    menuWidth = qBound(300, menuWidth, 800);
    menuHeight = qBound(200, menuHeight, 500);

    winMenuWidget->resize(menuWidth, menuHeight);
    winMenuWidget->move((width() - menuWidth) / 2, (height() - menuHeight) / 2);

    int fontSize = qMin(menuWidth, menuHeight) / 12;
    fontSize = qBound(20, fontSize, 60);

    QFont titleFont("Comic Sans MS", fontSize, QFont::Bold);
    winLabel->setFont(titleFont);

    int buttonFontSize = qMin(menuWidth, menuHeight) / 18;
    buttonFontSize = qBound(14, buttonFontSize, 40);

    QFont buttonFont("Comic Sans MS", buttonFontSize, QFont::Bold);
    restartButton->setFont(buttonFont);
    quitButton->setFont(buttonFont);

    int padding = qMin(menuWidth, menuHeight) / 15;
    padding = qBound(10, padding, 60);

    QLayout *layout = winMenuWidget->layout();
    if (layout) {
        layout->setContentsMargins(padding, padding, padding, padding);
        layout->setSpacing(padding / 2);
    }
}