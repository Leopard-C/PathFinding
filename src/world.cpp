#include "world.h"
#include <QPainter>
#include <QResizeEvent>
#include <QMessageBox>
#include <thread>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "utils/utils.h"


World::World(QWidget* parent/* = nullptr*/)
    : QLabel(parent)
{
    this->setStyleSheet("border:1px solid gray;");
    this->setMinimumSize(50, 50);
    this->setAlignment(Qt::AlignCenter);
    this->setMouseTracking(true);

    resetColors();

    // for multithread(not GUI thread)
    connect(this, &World::sigDrawSrc,  this, &World::drawSrc);
    connect(this, &World::sigDrawDest, this, &World::drawDest);
    connect(this, &World::sigDrawPath, this, &World::drawPath);
}

/*   reset colors
***/
void World::resetColors() {
    bgColor       = DEFAULT_BG_COLOR;
    lineColor     = DEFAULT_LINE_COLOR;
    pathColor     = DEFAULT_PATH_COLOR;
    obstacleColor = DEFAULT_OBSTACLE_COLOR;
    srcColor      = DEFAULT_SRC_COLOR;
    destColor     = DEFAULT_DEST_COLOR;
    visitedColor  = DEFAULT_VISITED_COLOR;
}


/*********************************************************
 *
 *          World Size
 *
*********************************************************/

bool World::setWorldSize(int worldWidth, int worldHeight) {
    if (findingPath) {
        return false;
    }
    if (!checkWorldSize(worldWidth, worldHeight)) {
        this->setText(errMsg);
        emit sigLogError(errMsg);
        return false;
    }

    clearAll();
    this->worldWidth = worldWidth;
    this->worldHeight = worldHeight;
    grids = util::New2D<char>(this->worldWidth, this->worldHeight, 0);
    drawGrids();

    emit sigLogInfo(QString("Set world size: [%0, %1]").arg(worldWidth).arg(worldHeight));
    return true;
}

bool World::checkWorldSize(int worldWidth, int worldHeight) {
    if (worldWidth < WORLD_WIDTH_MIN) {
        errMsg = QString("World width should be %0 at least").arg(WORLD_WIDTH_MIN);
        return false;
    }
    if (worldHeight < WORLD_HEIGHT_MIN) {
        errMsg = QString("World height should be %0 at least").arg(WORLD_HEIGHT_MIN);
        return false;
    }

    double scaleWidget = double(widgetPixelWidth) / widgetPixelHeight;
    double scaleWorld  = double(worldWidth)  / worldHeight;

    if (scaleWidget > scaleWorld) {
        worldPixelWidth  = widgetPixelHeight * scaleWorld;
        worldPixelHeight = widgetPixelHeight;

        if (worldHeight > worldPixelHeight / 6) {
            errMsg = "World height is too big";
            return false;
        }
        if (worldWidth > worldPixelWidth / 6) {
            errMsg = "World width is too big";
            return false;
        }

        deltaX = worldPixelWidth  / worldWidth;
        deltaY = worldPixelHeight / worldHeight;
    }
    else {
        worldPixelWidth  = widgetPixelWidth;
        worldPixelHeight = widgetPixelWidth / scaleWorld;

        if (worldWidth > worldPixelWidth / 6) {
            errMsg = "World width is too big";
            return false;
        }
        if (worldHeight > worldPixelHeight / 6) {
            errMsg = "World height is too big";
            return false;
        }

        deltaX = worldPixelWidth  / worldWidth;
        deltaY = worldPixelHeight / worldHeight;
    }

    startX = (widgetPixelWidth - deltaX * worldWidth) / 2.0;
    startY = (widgetPixelHeight - deltaY * worldHeight) / 2.0;

    worldPixelWidth = deltaX * worldWidth;
    worldPixelHeight = deltaY * worldHeight;

    return true;
}


/***********************************************************
 *
 *                     World Coordinate
 *
***********************************************************/

QPoint World::getWorldXY(int pixelX, int pixelY) const {
    if (pixelX < startX || pixelY < startY) {
        return { -1, - 1 };
    }
    if (pixelX >= startX + worldPixelWidth || pixelY >= startY + worldPixelHeight) {
        return { -1, -1 };
    }
    int worldX = (pixelX - startX) / deltaX;
    int worldY = (pixelY - startY) / deltaY;
    return { worldX, worldY };
}

QPoint World::getWorldXY(const QPoint &pixel) const {
    return getWorldXY(pixel.x(), pixel.y());
}

bool World::checkWorldXY(int worldX, int worldY) const {
    return (worldX >= 0 && worldX < worldWidth && worldY >= 0 && worldY < worldHeight);
}

bool World::checkWorldXY(const QPoint &worldXY) const {
    return checkWorldXY(worldXY.x(), worldXY.y());
}


/**************************************************************
 *
 *                   Fill in a Grid with color
 *
**************************************************************/

void World::fillGrid(QPainter &painter, int worldX, int worldY, const QColor &color) {
    int left = startX + worldX * deltaX + margin;
    int right = startX + (worldX + 1) * deltaX - margin;
    int top = startY + worldY * deltaY + margin;
    int bottom = startY  + (worldY + 1) * deltaY - margin;
    QRect rect(QPoint(left, top), QPoint(right, bottom));
    painter.fillRect(rect, color);
}

void World::fillGrid(QPainter &painter, const QPoint &worldXY, const QColor &color) {
    fillGrid(painter, worldXY.x(), worldXY.y(), color);
}

void World::fillGrid(int worldX, int worldY, const QColor& color) {
    QPainter painter(&pixmap);
    fillGrid(painter, worldX, worldY, color);
    this->setPixmap(pixmap);
}

void World::fillGrid(const QPoint& worldXY, const QColor& color) {
    return fillGrid(worldXY.x(), worldXY.y(), color);
}


/***********************************************************
 *
 *                     Draw
 *
***********************************************************/

void World::drawGrids() {
    if (worldWidth < WORLD_WIDTH_MIN || worldHeight < WORLD_HEIGHT_MIN) {
        return;
    }

    pixmap = QPixmap(this->size());
    pixmap.fill(bgColor);
    QPainter painter(&pixmap);
    QPen pen;
    pen.setColor(lineColor);
    pen.setWidth(1);
    painter.setPen(pen);

    for (int i = 0; i <= worldWidth; ++i) {
        int x = startX + i * deltaX;
        painter.drawLine(x, startY, x, startY + worldPixelHeight);
    }

    for (int i = 0; i <= worldHeight; ++i) {
        int y = startY + i * deltaY;
        painter.drawLine(startX, y, startX + worldPixelWidth, y);
    }

    this->setPixmap(pixmap);
}

void World::drawVisited(int worldX, int worldY) {
    fillGrid(worldX, worldY, visitedColor);
}

void World::drawVisiteds() {
    fillGrids(visitedPath, visitedColor);
}

void World::drawObstacle(int worldX, int worldY) {
    fillGrid(worldX, worldY, obstacleColor);
}

void World::drawObstacles() {
    if (grids) {
        QPainter painter(&pixmap);
        for (int y = 0; y < worldHeight; ++y) {
            for (int x = 0; x < worldWidth; ++x) {
                if (grids[y][x]) {
                    fillGrid(painter, x, y, obstacleColor);
                }
            }
        }
        this->setPixmap(pixmap);
    }
}

void World::drawSrc() {
    if (src.x() != -1 && src.y() != -1) {
        fillGrid(src, srcColor);
    }
}

void World::drawDest() {
    if (dest.x() != -1 && dest.y() != -1) {
        fillGrid(dest, destColor);
    }
}

void World::drawPath() {
    fillGrids(path, pathColor);
}


/*******************************************************
 *
 *     refresh, clear, reset
 *
*******************************************************/

void World::refreshAll() {
    drawGrids();
    drawObstacles();
    drawVisiteds();
    drawPath();
    drawSrc();
    drawDest();
}

void World::clearAll() {
    if (grids) {
        util::Delete2D(grids, this->worldWidth, this->worldHeight);
    }
    std::list<Coordinate>().swap(path);
    std::vector<Coordinate>().swap(visitedPath);
    src = { -1, -1 };
    dest = { -1, -1 };
}

void World::resetAll() {
    clearAll();
    grids = util::New2D<char>(this->worldWidth, this->worldHeight, 0);
    drawGrids();
    emit sigLogInfo("Reset OK.");
}

void World::clearPath() {
    std::list<Coordinate>().swap(path);
    std::vector<Coordinate>().swap(visitedPath);
    drawGrids();
    drawObstacles();
    drawSrc();
    drawDest();
}


/*********************************************************
 *
 *                Find  Path
 *
*********************************************************/

void World::findPath() {
    if (findingPath) {
        return;
    }
    findingPath = true;

    bool flag = false;
    do {
        if (!grids) {
            errMsg = "Please set world size first.";
            break;
        }
        if (!findPathBase) {
            errMsg = "Please set find path method first.";
            break;
        }
        if (src.x() == -1 || src.y() == -1) {
            errMsg = "Please set source!";
            break;
        }
        if (dest.x() == -1 || dest.y() == -1) {
            errMsg = "Please set destination!";
            break;
        }

        flag = true;
    } while (false);

    if (!flag) {
        emit sigFindPathStopped();
        emit sigLogError(errMsg);
        findingPath = false;
        return;
    }

    clearPath();
    emit sigLogInfo("Finding path...");

    std::thread t([&]{
        findPathBase->setBoundary(0, worldWidth-1, 0, worldHeight-1);
        findPathBase->setShowVisitedPath(showVisitedPath);
        findPathBase->setShowVisitedPathInterval(showVisitedPathInterval);
        findPathBase->enableDiagonalMove(enableDiagonalMove);
        findPathBase->clearObstacles();
        for (int y = 0; y < worldHeight; ++y) {
            for (int x = 0; x < worldWidth; ++x) {
                if (grids[y][x] == 1) {
                    findPathBase->addObstacle({ x, y });
                }
            }
        }

        Coordinate src_(src.x(), src.y());
        Coordinate dest_(dest.x(), dest.y());
        path = findPathBase->findPath(src_, dest_);

        if (findingPath) {
            if (path.back() == dest_) {
                emit sigLogInfo(QString("Found path! (%0 steps)").arg(path.size()));
                emit sigDrawPath();
                emit sigDrawSrc();
                emit sigDrawDest();
            }
            else {
                emit sigLogError("Not found path!");
            }
        }
        else {
            emit sigLogError("Force stopped.");
        }

        emit sigFindPathStopped();
        findingPath = false;
    });
    t.detach();
}

void World::stopFindPath() {
    if (findPathBase) {
        findPathBase->stop();
        findingPath = false;
        emit sigLogInfo("Stopping...");
    }
}

void World::visited(int x, int y) {
    if ((src.x() == x && src.y() == y) || (dest.x() == x && dest.y() == y)) {
        return;
    }
    this->fillGrid(x, y, visitedColor);
    visitedPath.emplace_back(x, y);
    update();
}


/*******************************************************
 *
 *      Source & Destination
 *
*******************************************************/

void World::startSetSource() {
    if (findingPath) {
        return;
    }
    mode = MODE_SET_SOURCE;
    emit sigLogInfo("Please click to set source...");
}

void World::startSetDestination() {
    if (findingPath) {
        return;
    }
    mode = MODE_SET_DESTINATION;
    emit sigLogInfo("Please click to set destination...");
}


/*******************************************************
 *
 *      Obstacles
 *
*******************************************************/

void World::setObstacle(int worldX, int worldY, bool isObstacle) {
    if (checkWorldXY(worldX, worldY) && grids) {
        grids[worldY][worldX] = isObstacle;
    }
}

void World::setObstacle(const QPoint& worldXY, bool isObstacle) {
    setObstacle(worldXY.x(), worldXY.y(), isObstacle);
}

bool World::isObstacle(int worldX, int worldY) const {
    return (grids && grids[worldY][worldX] == 1);
}

bool World::isObstacle(const QPoint& worldXY) const {
    return isObstacle(worldXY.x(), worldXY.y());
}

void World::setRandomObstacles(double ratio) {
    if (!grids) {
        emit sigLogError("Please set world size first.");
        return;
    }
    if (ratio < 0.001 || ratio > 0.999) {
        emit sigLogError(QString("Invalid ratio: %0 [0.001, 0.999]").arg(ratio));
        return;
    }

    clearAll();
    grids = util::New2D<char>(this->worldWidth, this->worldHeight, 0);

    int gridsCount = worldWidth * worldHeight;
    int obstaclesCount = gridsCount * ratio;
    std::vector<int> rands;
    rands.reserve(gridsCount);
    for (int i = 0; i < obstaclesCount; ++i) {
        rands.push_back(1);
    }
    for (int i = obstaclesCount; i < gridsCount; ++i) {
        rands.push_back(0);
    }
    std::random_shuffle(rands.begin(), rands.end());

    for (int y = 0; y < worldHeight; ++y) {
        for (int x = 0; x < worldWidth; ++x) {
            int idx = y * worldHeight + x;
            grids[y][x] = rands[idx];
        }
    }

    drawGrids();
    drawObstacles();
    emit sigLogInfo("Set random obstacles OK!");
}


/*************************************************************
 *
 *       Get Save Data
 *
*************************************************************/

bool World::saveImage(const QString& filename) const {
    if (!grids) {
        emit sigLogError("Please set world size first.");
        return false;
    }
    return pixmap.save(filename, "png");
}

bool World::saveData(const QString& filename, bool bObstacle, bool bSrc,
                           bool bDest, bool bPath, bool bVisited) const
{
    if (!grids) {
        emit sigLogError("Please set world size first.");
        return false;
    }

    FILE* fp = fopen(filename.toStdString().c_str(), "w+b");
    if (!fp) {
        emit sigLogError("Open file failed.");
        return false;
    }

    fwrite(&worldWidth,  sizeof(int), 1, fp);
    fwrite(&worldHeight, sizeof(int), 1, fp);
    fwrite(&bSrc , sizeof(bool), 1, fp);
    if (bSrc) {
        int x = src.x();
        int y = src.y();
        fwrite(&x, sizeof(int), 1, fp);
        fwrite(&y, sizeof(int), 1, fp);
    }

    fwrite(&bDest, sizeof(bool), 1, fp);
    if (bDest) {
        int x = dest.x();
        int y = dest.y();
        fwrite(&x, sizeof(int), 1, fp);
        fwrite(&y, sizeof(int), 1, fp);
    }

    fwrite(&bPath, sizeof(bool), 1, fp);
    if (bPath) {
        int size = path.size();
        fwrite(&size, sizeof(int), 1, fp);
        for (auto& coord : path) {
            fwrite(&coord.x, sizeof(int), 1, fp);
            fwrite(&coord.y, sizeof(int), 1, fp);
        }
    }

    fwrite(&bVisited, sizeof(bool), 1, fp);
    if (bVisited) {
        int size = visitedPath.size();
        fwrite(&size, sizeof(int), 1, fp);
        for (auto& coord : visitedPath) {
            fwrite(&coord.x, sizeof(int), 1, fp);
            fwrite(&coord.y, sizeof(int), 1, fp);
        }
    }

    fwrite(&bObstacle, sizeof(bool), 1, fp);
    if (bObstacle) {
        int size = 0;
        for (int y = 0; y < worldHeight; ++y) {
            for (int x = 0; x < worldWidth; ++x) {
                if (grids[y][x] == 1) {
                    size++;
                }
            }
        }
        fwrite(&size, sizeof(int), 1, fp);
        for (int y = 0; y < worldHeight; ++y) {
            for (int x = 0; x < worldWidth; ++x) {
                if (grids[y][x] == 1) {
                    fwrite(&x, sizeof(char), 1, fp);
                    fwrite(&y, sizeof(char), 1, fp);
                }
            }
        }
    }

    fclose(fp);
    return true;
}

bool World::loadData(const QString &filename) {
    FILE* fp = fopen(filename.toStdString().c_str(), "rb");
    if (!fp) {
        emit sigLogError("Open file failed.");
        return false;
    }

    int flag = -1;
    do {
        if (feof(fp)) { break; }
        fread(&worldWidth, sizeof(int), 1, fp);
        if (feof(fp)) { break; }
        fread(&worldHeight, sizeof(int), 1, fp);

        if (!checkWorldSize(worldWidth, worldHeight)) {
            this->setText(errMsg);
            emit sigLogError(errMsg);
            flag = 1;
            break;
        }

        clearAll();
        grids = util::New2D<char>(this->worldWidth, this->worldHeight, 0);
        emit sigUpdateWorldSize(worldWidth, worldHeight);

        bool bObstacle, bSrc, bDest,  bPath,  bVisited;

        if (feof(fp)) { break; }
        fread(&bSrc , sizeof(bool), 1, fp);
        if (bSrc) {
            int x = -1, y = -1;
            if (feof(fp)) { break; }
            fread(&x, sizeof(int), 1, fp);
            if (feof(fp)) { break; }
            fread(&y, sizeof(int), 1, fp);
            src = { x, y };
        }

        if (feof(fp)) { break; }
        fread(&bDest, sizeof(bool), 1, fp);
        if (bDest) {
            int x = -1, y = -1;
            if (feof(fp)) { break; }
            fread(&x, sizeof(int), 1, fp);
            if (feof(fp)) { break; }
            fread(&y, sizeof(int), 1, fp);
            dest = { x, y };
        }

        if (feof(fp)) { break; }
        fread(&bPath, sizeof(bool), 1, fp);
        if (bPath) {
            int size = 0;
            if (feof(fp)) { break; }
            fread(&size, sizeof(int), 1, fp);
            int x, y;
            for (int i = 0; i < size; ++i) {
                if (feof(fp)) { break; }
                fread(&x, sizeof(int), 1, fp);
                fread(&y, sizeof(int), 1, fp);
                path.emplace_back(x, y);
            }
        }

        if (feof(fp)) { break; }
        fread(&bVisited, sizeof(bool), 1, fp);
        if (bVisited) {
            int size = 0;
            if (feof(fp)) { break; }
            fread(&size, sizeof(int), 1, fp);
            int x, y;
            for (int i = 0; i < size; ++i) {
                if (feof(fp)) { break; }
                fread(&x, sizeof(int), 1, fp);
                if (feof(fp)) { break; }
                fread(&y, sizeof(int), 1, fp);
                visitedPath.emplace_back(x, y);
            }
        }

        if (feof(fp)) { break; }
        fread(&bObstacle, sizeof(bool), 1, fp);
        if (bObstacle) {
            int size = 0;
            if (feof(fp)) { break; }
            fread(&size, sizeof(int), 1, fp);
            int x, y;
            for (int i = 0; i < size; ++i) {
                if (feof(fp)) { break; }
                fread(&x, sizeof(char), 1, fp);
                if (feof(fp)) { break; }
                fread(&y, sizeof(char), 1, fp);
                grids[y][x] = 1;
            }
        }

        flag = 0;
    } while (false);

    if (flag == -1) {
        emit sigLogError("File data is broken.");
    }

    fclose(fp);
    return true;
}


/*************************************************************
 *
 *       Override Event
 *
*************************************************************/

void World::resizeEvent(QResizeEvent *event) {
    widgetPixelWidth = event->size().width();
    widgetPixelHeight = event->size().height();

    if (!grids) { return; }

    if (checkWorldSize(worldWidth, worldHeight)) {
        refreshAll();
    }
    else {
        //clearAll();
        this->setText(errMsg);
    }
}

void World::mousePressEvent(QMouseEvent *ev) {
    if (!grids || findingPath) { return; }

    clearPath();

    if (ev->button() == Qt::LeftButton) {
        // press shift to draw a line. (TESTing)
        if (ev->modifiers() == Qt::ShiftModifier) {
            if (startPos.x() == -1 || startPos.y() == -1) {
                return;
            }
            endPos = ev->pos();
            QPoint start = getWorldXY(startPos);
            QPoint end = getWorldXY(endPos);
            if (!checkWorldXY(start) || !checkWorldXY(end)) {
                return;
            }
            int width = abs(end.x() - start.x());
            int height = abs(end.y() - start.y());
            if (width < height) {
                if (start.y() < end.y()) {
                    for (int y = start.y(); y <= end.y(); ++y) {
                        drawObstacle(start.x(), y);
                        setObstacle(start.x(), y, true);
                    }
                }
                else {
                    for (int y = end.y(); y <= start.y(); ++y) {
                        drawObstacle(start.x(), y);
                        setObstacle(start.x(), y, true);
                    }
                }
            }
            else {
                if (start.x() < end.x()) {
                    for (int x = start.x(); x <= end.x(); ++x) {
                        drawObstacle(x, start.y());
                        setObstacle(x, start.y(), true);
                    }
                }
                else {
                    for (int x = end.x(); x <= start.x(); ++x) {
                        drawObstacle(x, start.y());
                        setObstacle(x, start.y(), true);
                    }
                }
            }
            startPos = { -1, -1 };
            return;
        }
        else {
            startPos = ev->pos();
        }

        QPoint worldXY = getWorldXY(ev->pos());
        if (!checkWorldXY(worldXY)) {
            return;
        }

        switch (mode) {
        case MODE_SET_OBSTACLE:
            if (worldXY == src) {
                src = { -1, -1 };
            }
            else if (worldXY == dest) {
                dest = { -1, -1 };
            }
            if (!isObstacle(worldXY)) {
                fillGrid(worldXY, obstacleColor);
                setObstacle(worldXY, true);
            }
            break;
        case MODE_SET_SOURCE:
            if (isObstacle(worldXY)) {
                setObstacle(worldXY, false);
            }
            else if (worldXY == dest) {
                dest = { -1, -1 };
            }
            if (src.x() != -1 && src.y() != -1) {
                fillGrid(src, bgColor);
            }
            src = worldXY;
            fillGrid(src, srcColor);
            emit sigLogInfo(QString("Set Source: (%0, %1)").arg(src.x()).arg(src.y()));
            break;
        case MODE_SET_DESTINATION:
            if (isObstacle(worldXY)) {
                setObstacle(worldXY, false);
            }
            else if (worldXY == src) {
                src = { -1, -1 };
            }
            if (dest.x() != -1 && dest.y() != -1) {
                fillGrid(dest, bgColor);
            }
            dest = worldXY;
            fillGrid(dest, destColor);
            emit sigLogInfo(QString("Set Destination: (%0, %1)").arg(dest.x()).arg(dest.y()));
            break;
        }
    }
    else if (ev->button() == Qt::RightButton) {
        QPoint worldXY = getWorldXY(ev->pos());
        if (!checkWorldXY(worldXY)) {
            return;
        }

        if (ev->modifiers() == Qt::ShiftModifier) {
            return;
        }
        if (isObstacle(worldXY)) {
            setObstacle(worldXY, false);
            fillGrid(worldXY, bgColor);
        }
        else if (worldXY == src) {
            src = { -1, -1 };
            fillGrid(worldXY, bgColor);
        }
        else if (worldXY == dest) {
            dest = { -1, -1 };
            fillGrid(worldXY, bgColor);
        }
    }

    currMouseBtn = ev->button();
}

void World::mouseMoveEvent(QMouseEvent *ev) {
    if (!grids || findingPath || mode != MODE_SET_OBSTACLE) {
        return;
    }

    if (ev->modifiers() == Qt::ShiftModifier) {
        return;
    }

    QPoint worldXY = getWorldXY(ev->pos());
    if (!checkWorldXY(worldXY)) {
        return;
    }

    if (currMouseBtn == Qt::LeftButton) {
        if (worldXY == src) {
            src = { -1, - 1 };
        }
        else if (worldXY == dest) {
            dest = { -1, - 1 };
        }
        else if (!isObstacle(worldXY)) {
            fillGrid(worldXY, obstacleColor);
            setObstacle(worldXY, true);
        }
    }
    else if (currMouseBtn == Qt::RightButton) {
        if (worldXY == src) {
            src = { -1, - 1 };
        }
        else if (worldXY == dest) {
            dest = { -1, - 1 };
        }
        else if (isObstacle(worldXY)) {
            fillGrid(worldXY, bgColor);
            setObstacle(worldXY, false);
        }
    }
}

void World::mouseReleaseEvent(QMouseEvent*) {
    mode = MODE_SET_OBSTACLE;
    currMouseBtn = Qt::NoButton;
}
