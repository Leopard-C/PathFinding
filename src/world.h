#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QLabel>
#include <vector>
#include <QPainter>
#include "findpath/findpath_base.h"

class World : public QLabel
{
    Q_OBJECT
public:
    enum MouseClickMode {
        MODE_SET_OBSTACLE    = 0,
        MODE_SET_SOURCE      = 1,
        MODE_SET_DESTINATION = 2,
    };

public:
    World(QWidget* parent = nullptr);

    QSize getWorldSize() const { return { worldWidth, worldHeight }; }
    QSize getWorldPixelSize() const { return { worldPixelWidth, worldPixelHeight}; }

    bool isShowVisitedPath() const { return showVisitedPath; }
    int  getShowVisitedPathInterval() const { return showVisitedPathInterval; }

    bool isFindingPath() const { return findingPath; }

    const QColor& getBgColor() const { return bgColor; }
    const QColor& getPathColor() const { return pathColor; }
    const QColor& getLineColor() const { return lineColor; }
    const QColor& getObstacleColor() const { return obstacleColor; }
    const QColor& getSrcColor() const { return srcColor; }
    const QColor& getDestColor() const { return destColor; }
    const QColor& getVisitedColor() const { return visitedColor; }

    bool saveImage(const QString& filename) const;
    bool saveData(const QString& filename, bool bObstacle, bool bSrc, bool bDest, bool bPath, bool bVisited) const;
    bool loadData(const QString& filename);

signals:
    void sigLogInfo(QString msg) const;
    void sigLogError(QString msg) const;
    void sigDrawPath();
    void sigDrawSrc();
    void sigDrawDest();
    void sigUpdateWorldSize(int w, int h);
    void sigFindPathStopped();

public slots:
    void setMargin(int m) { margin = m; }
    bool setWorldSize(int worldWidth, int worldHeight);

    /*   colors
    ***/
    void setBgColor(const QColor& c) { bgColor = c; refreshAll(); }
    void setPathColor(const QColor& c) { pathColor = c; refreshAll(); }
    void setLineColor(const QColor& c) { lineColor = c; refreshAll(); }
    void setObstacleColor(const QColor& c) { obstacleColor = c; refreshAll(); }
    void setSrcColor(const QColor& c) { srcColor = c; refreshAll(); }
    void setDestColor(const QColor& c) { destColor = c; refreshAll(); }
    void setVisitedColor(const QColor& c) { visitedColor = c; refreshAll(); }
    void resetColors();

    /*   start set source & destination
    ***/
    void startSetSource();
    void startSetDestination();

    /*   set random obstacles
    ***/
    void setRandomObstacles(double ratio);

    /*   Diagonal Move
    ***/
    void setEnableDiagonalMove(bool enable) { enableDiagonalMove = enable; }

    /*   visited path
    ***/
    void visited(int x, int y);
    void setShowVisitedPath(bool show) { showVisitedPath = show; }
    void setShowVisitedPathInterval(int interval) { showVisitedPathInterval = interval; }

    /*    start/stop find path
    ***/
    void findPath();
    void stopFindPath();

    /*   clear, reset, refresh
    ***/
    void clearAll();
    void resetAll();
    void clearPath();
    void refreshAll();

    /*   Draw
    ***/
    void drawGrids();
    void drawObstacle(int worldX, int worldY);
    void drawObstacles();
    void drawVisited(int worldX, int worldY);
    void drawVisiteds();
    void drawSrc();
    void drawDest();
    void drawPath();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

private:
    QPoint getWorldXY(int pixelX, int pixelY) const;
    QPoint getWorldXY(const QPoint& pixel) const;

    void fillGrid(QPainter& painter, int worldX, int worldY, const QColor& color);
    void fillGrid(QPainter& painter, const QPoint& worldXY, const QColor& color);
    void fillGrid(int worldX, int worldY, const QColor& color);
    void fillGrid(const QPoint& worldXY, const QColor& color);

    template<typename Containter>
    void fillGrids(const Containter& c, const QColor& color) {
        QPainter painter(&pixmap);
        for (auto& coord : c) {
            fillGrid(painter, coord.x, coord.y, color);
        }
        this->setPixmap(pixmap);
    }

    /*   Check param is valid
    ***/
    bool checkWorldSize(int worldWidth, int worldHeight);
    bool checkWorldXY(int worldX, int worldY) const;
    bool checkWorldXY(const QPoint& worldXY) const;

    void setObstacle(int worldX, int worldY, bool isObstacle);
    void setObstacle(const QPoint& worldXY, bool isObstacle);

    bool isObstacle(int worldX, int worldY) const;
    bool isObstacle(const QPoint& worldXY) const;


public:
    FindPathBase* findPathBase = nullptr;

private:
    enum {
        DEFAULT_BG_COLOR       = qRgb(255, 255, 255),
        DEFAULT_LINE_COLOR     = qRgb(150, 150, 150),
        DEFAULT_PATH_COLOR     = qRgb( 90, 255,  30),
        DEFAULT_OBSTACLE_COLOR = qRgb( 66,  66,  66),
        DEFAULT_SRC_COLOR      = qRgb(130, 230, 230),
        DEFAULT_DEST_COLOR     = qRgb(255,  50,  50),
        DEFAULT_VISITED_COLOR  = qRgb(255, 150,   0)
    };

    enum {
        WORLD_WIDTH_MIN  = 3,
        WORLD_HEIGHT_MIN = 3
    };

private:
    QPixmap pixmap;
    QString errMsg;

    bool findingPath = false;

    int worldWidth = 0;
    int worldHeight = 0;
    int worldPixelWidth = 0;
    int worldPixelHeight = 0;

    /*  size of this widget. update in 'resize(event)'
    ***/
    int widgetPixelWidth;
    int widgetPixelHeight;

    /*  use for drawing grids
    ***/
    int startX, startY;
    int deltaX, deltaY;
    int margin = 1;

    char** grids = nullptr;
    QPoint src  = { -1, - 1 };
    QPoint dest = { -1, - 1 };
    std::list<Coordinate> path;
    std::vector<Coordinate> visitedPath;

    /*  settings for finding path
    ***/
    bool showVisitedPath = true;
    int showVisitedPathInterval = 10;
    bool enableDiagonalMove = false;

    QColor bgColor;
    QColor lineColor;
    QColor pathColor;
    QColor obstacleColor;
    QColor srcColor;
    QColor destColor;
    QColor visitedColor;

    /*  mouse state
    ***/
    QPoint startPos = { -1, -1 };
    QPoint endPos;
    Qt::MouseButton currMouseBtn = Qt::NoButton;
    MouseClickMode mode = MODE_SET_OBSTACLE;
};

#endif // WORLD_H
