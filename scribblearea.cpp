#include "scribblearea.h"

#include <QChar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>

ScribbleArea::ScribbleArea(QWidget *parent)
    : QWidget(parent)
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
    startTimer(30);
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
    painter.drawPixmap(0, 0, width(), height(), pixmapWater);

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

        // 1. Рисуем 3 фоновые волны течения
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

        // 2. Пускаем бегущие пузырьки воздуха поверх волн
        painter.setPen(QPen(QColor(255, 255, 255, 200), 1.5)); // Белый ободок пузыря
        painter.setBrush(QBrush(QColor(255, 255, 255, 80)));   // Полупрозрачное тело пузыря

        int bubbleCount = 4; // Количество пузырьков на одной линии течения
        for (int k = 0; k < bubbleCount; ++k) {
            // Равномерно распределяем пузырьки по длине течения и заставляем их двигаться
            double progress = (streamAnimationStep / 100.0) + ((double)k / bubbleCount);
            if (progress > 1.0) progress -= 1.0; // Зацикливаем движение

            // Находим координату X пузырька на линии
            double bubbleX = distance * progress;

            // Находим координату Y, используя синус центральной волны (w=1, фазовый сдвиг без смещения w*1.5)
            double centralPhaseShift = streamAnimationStep * 0.15;
            double bubbleY = waveAmplitude * std::sin((bubbleX / waveLength) - centralPhaseShift);

            // Пузырьки будут слегка пульсировать или отличаться по размеру для живости
            double bubbleRadius = cellSize * (0.04 + 0.02 * (k % 2));

            // Рисуем круглый пузырек воздуха
            painter.drawEllipse(QPointF(bubbleX, bubbleY), bubbleRadius, bubbleRadius);
        }

        painter.restore();
    };

    // Оставляем сочный водно-голубой/неоновый оттенок
    QColor neonWaterColor(0, 191, 255, 150);

    // Отрисовываем три водных потока
    drawWaveStream(5, 1, 5, 3, neonWaterColor);
    drawWaveStream(2, 3, 2, 1, neonWaterColor);
    drawWaveStream(6, 4, 5, 6, neonWaterColor);

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
                    painter.setBrush(QBrush(QColor(139, 69, 19, 180))); // Коричневый цвет грязи

                    // Рисуем 3 случайных пятна внутри прямоугольника кувшинки
                    // Используем координаты ячейки cellRect
                    painter.drawEllipse(cellRect.x() + cellSize * 0.2, cellRect.y() + cellSize * 0.3, cellSize * 0.25, cellSize * 0.15);
                    painter.drawEllipse(cellRect.x() + cellSize * 0.5, cellRect.y() + cellSize * 0.5, cellSize * 0.3, cellSize * 0.2);
                    painter.drawEllipse(cellRect.x() + cellSize * 0.3, cellRect.y() + cellSize * 0.6, cellSize * 0.2, cellSize * 0.15);
                    painter.restore();
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

  // Вспомогательная лямбда-функция для отрисовки лягушки с эффектом воды
    auto drawSingleFrog = [&](int id, const QPointF& currentPos, const QPixmap& pixmap) {
        if (player == id && teleportPhase > 0) {
            // Вычисляем, сколько пикселей от исходной картинки нужно ОТРЕЗАТЬ снизу
            int srcHeight = pixmap.height();
            int croppedSrcHeight = srcHeight * (1.0f - frogSubmersion);

            // Вычисляем пропорциональную высоту лягушки на экране
            int currentFrogHeight = frogSize * (1.0f - frogSubmersion);

            if (currentFrogHeight > 0 && croppedSrcHeight > 0) {
                // Вырезаем верхнюю (надводную) часть текстуры лягушки.
                // Метод copy(X, Y, Ширина, Высота) берет кусок от нуля до croppedSrcHeight. Лапки остаются снизу за бортом!
                QPixmap visiblePart = pixmap.copy(0, 0, pixmap.width(), croppedSrcHeight);

                // Рисуем этот кусочек на экране
                painter.drawPixmap(currentPos.x(), currentPos.y(), frogSize, currentFrogHeight, visiblePart);
            }
        } else {
            // Обычное отображение на кувшинке в полный рост
            painter.drawPixmap(currentPos.x(), currentPos.y(), frogSize, frogSize, pixmap);
        }
    };

    // Отрисовываем игроков
    drawSingleFrog(0, p1CurrentPos, pixmapFrog1);
    drawSingleFrog(1, p2CurrentPos, pixmapFrog2);
    if (pCount > 2) {
        drawSingleFrog(2, p3CurrentPos, pixmapFrog3);
    }


    // painter.drawPixmap(p1CurrentPos.x(), p1CurrentPos.y(), frogSize, frogSize, pixmapFrog1);
    // painter.drawPixmap(p2CurrentPos.x(), p2CurrentPos.y(), frogSize, frogSize, pixmapFrog2);

    // if (pCount > 2) {
    //     painter.drawPixmap(p3CurrentPos.x(), p3CurrentPos.y(), frogSize, frogSize, pixmapFrog3);
    // }

    // auto drawFrog = [&](int tileX, int tileY, const QPixmap &frog, float dx, float dy) {
    //     int x = offsetX + tileX * cellSize + dx * cellSize;
    //     int y = offsetY + tileY * cellSize + dy * cellSize;
    //     painter.drawPixmap(x, y, frogSize, frogSize, frog);
    // };

    // drawFrog(p1TileX, p1TileY, pixmapFrog1, 0.1f, 0.1f);
    // drawFrog(p2TileX, p2TileY, pixmapFrog2, 0.5f, 0.1f);
    // if (pCount > 2) {
    //     drawFrog(p3TileX, p3TileY, pixmapFrog3, 0.3f, 0.5f);
    // }

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

    // --- АНИМАЦИЯ РОЯ МОШЕК НАД ОШТРАФОВАННЫМИ ЛЯГУШКАМИ ---
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::black)); // Мошки — просто маленькие чёрные точки

    // Проверяем всех трех возможных игроков
    for (int id = 0; id < pCount; ++id) {
        // Если игрок сейчас находится под эффектом пропуска хода
        if (skipTurn[id]) {
            // Находим текущие экранные координаты этой лягушки
            QPointF frogPos = (id == 0) ? p1CurrentPos : ((id == 1) ? p2CurrentPos : p3CurrentPos);

            // Центр роя будет находиться чуть выше головы лягушки
            QPointF swarmCenter(frogPos.x() + frogSize / 2.0, frogPos.y() - cellSize * 0.1);

            int flyCount = 5; // Количество мошек в рою
            for (int f = 0; f < flyCount; ++f) {
                // У каждой мошки своя фаза и своя траектория, чтобы они летали хаотично
                double timeFactor = streamAnimationStep * 0.3 + (f * 2.5);

                // Радиус полета мошки вокруг центра роя (от 2 до 8 пикселей)
                double radiusX = (cellSize * 0.08) + std::sin(timeFactor * 0.7) * 5;
                double radiusY = (cellSize * 0.05) + std::cos(timeFactor * 1.1) * 3;

                // Вычисляем смещение мошки по круговой параболе синуса/косинуса
                double flyX = swarmCenter.x() + std::cos(timeFactor) * radiusX;
                double flyY = swarmCenter.y() + std::sin(timeFactor) * radiusY;

                // Размер мошки (маленькая точка в 1.5 - 2.5 пикселя)
                double flySize = 1.5 + (f % 2);

                // Рисуем мошку
                painter.drawEllipse(QPointF(flyX, flyY), flySize, flySize);
            }
        }
    }
    painter.restore();

}

void ScribbleArea::timerEvent(QTimerEvent *event)
{
    // 1. Постоянно крутим фазу анимации течения волн
    streamAnimationStep = (streamAnimationStep + 1) % 100;
    if (!isRolling && !isMovingAnimation && teleportPhase == 0) {
        update();
    }

    // Расчитываем целевые экранные координаты для каждой лягушки
    QPointF p1Target(offsetX + p1TileX * cellSize + 0.1f * cellSize, offsetY + p1TileY * cellSize + 0.1f * cellSize);
    QPointF p2Target(offsetX + p2TileX * cellSize + 0.5f * cellSize, offsetY + p2TileY * cellSize + 0.1f * cellSize);
    QPointF p3Target(offsetX + p3TileX * cellSize + 0.3f * cellSize, offsetY + p3TileY * cellSize + 0.5f * cellSize);

    if (p1CurrentPos.isNull()) {
        p1CurrentPos = p1Target; p2CurrentPos = p2Target; p3CurrentPos = p3Target;
    }

    // --- КРИТИЧЕСКИЙ ФИКС: АВТОНОМНОЕ УПРАВЛЕНИЕ ТЕЛЕПОРТАЦИЕЙ ---
    if (teleportPhase > 0) {
        int currentTileX = (player == 0) ? p1TileX : ((player == 1) ? p2TileX : p3TileX);
        int currentTileY = (player == 0) ? p1TileY : ((player == 1) ? p2TileY : p3TileY);

        // Фаза 1: Медленное погружение на старте портала
        if (teleportPhase == 1) {
            frogSubmersion += 0.04f;
            if (frogSubmersion >= 0.6f) {
                frogSubmersion = 0.6f;
                teleportPhase = 2; // Переключаемся на заплыв

                int nextX = currentTileX; int nextY = currentTileY;
                int targetBackDir = -1;

                // Определяем точку выхода и жестко задаем направление НАЗАД (откуда пришла)
                if ((currentTileX == 5 && currentTileY == 1) || (currentTileX == 5 && currentTileY == 3)) {
                    nextX = 5; nextY = 3;
                    targetBackDir = 1; // Блокирует ЛЕВО. Лягушка гарантированно пойдет на ПРАВО.
                }
                else if ((currentTileX == 2 && currentTileY == 3) || (currentTileX == 5 && currentTileY == 3)) {
                    nextX = 2; nextY = 1;
                    targetBackDir = 2; // Блокирует ПРАВО. Лягушка гарантированно пойдет на ЛЕВО.
                }
                else if ((currentTileX == 6 && currentTileY == 4) || (currentTileX == 5 && currentTileY == 3)) {
                    nextX = 5; nextY = 6;
                    targetBackDir = 3; // Блокирует НИЗ. Лягушка гарантированно пойдет наВВЕРХ.
                }

                // Применяем координаты и правильный BackDir для текущего игрока
                if (player == 0) { p1TileX = nextX; p1TileY = nextY; p1BackDir = targetBackDir; }
                else if (player == 1) { p2TileX = nextX; p2TileY = nextY; p2BackDir = targetBackDir; }
                else if (player == 2) { p3TileX = nextX; p3TileY = nextY; p3BackDir = targetBackDir; }
            }
            update();
            return;
        }

        // Фаза 2: Заплыв с торчащей головой (Движение LERP)
        if (teleportPhase == 2) {
            // Скорость самого заплыва по воде можно тоже слегка снизить для реалистичности (изменили коэффициент LERP с 0.2 на 0.1)
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

        // Фаза 3: Медленное выныривание на новой кувшинке
        if (teleportPhase == 3) {
            frogSubmersion -= 0.04f; // ИСПРАВЛЕНО: Выныривает так же медленно, как и погружалась
            if (frogSubmersion <= 0.0f) {
                frogSubmersion = 0.0f;
                teleportPhase = 0; // Анимация полностью завершена

                // Передаем ход следующему игроку, так как заплыв окончен
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

    // --- ДАЛЬШЕ ИДЕТ ВАШ СТАНДАРТНЫЙ ИГРОВОЙ ЦИКЛ ---
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
        // Обычное плавное перемещение шагами
        p1CurrentPos += (p1Target - p1CurrentPos) * 0.3;
        p2CurrentPos += (p2Target - p2CurrentPos) * 0.3;
        p3CurrentPos += (p3Target - p3CurrentPos) * 0.3;

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

                if (currentCell == '2') {
                    skipTurn[player] = true;
                }

                // Если наступили на фиолетовую кувшинку — НЕ меняем игрока, а запускаем фазу 1 телепорта!
                if (currentCell == '3') {
                    teleportPhase = 1;
                    frogSubmersion = 0.0f;
                    update();
                    return; // Уходим на круг анимации погружения
                }

                // Обычная передача хода, если спец-эффектов нет
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
                // Логика выбора следующей плитки (поиск пути)
                bool moved = false;
                auto canMoveTo = [&](int x, int y) {
                    if (x < 0 || x > 7 || y < 0 || y > 7) return false;
                    QChar c = grid[x][y];
                    return (c == '1' || c == '2' || c == '3');
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