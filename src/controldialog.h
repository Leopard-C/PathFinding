#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTextEdit>
#include <QComboBox>
#include <QSlider>
#include "colorlabel.h"
#include "world.h"
#include "extend/controlpanelextend.h"


class ControlDialog : public QDialog
{
    Q_OBJECT
public:
    ControlDialog(World* world, QWidget* parent = nullptr);

signals:
    void sigClosed();
    void sigVisited(int x, int y);

public slots:
    void onSetWorldSize();
    void onFindPathMethodChanged(QString);
    void logInfo(QString msg);
    void logError(QString msg);

public:
    void logMessage(QString msg, bool error = false);

protected:
    void closeEvent(QCloseEvent *) override {
        emit sigClosed();
    }

private:
    World* world;

    QLineEdit* editWorldWidth;
    QLineEdit* editWorldHeight;
    QPushButton* btnSetWorldSize;
    QPushButton* btnSaveAsImage;
    QPushButton* btnSaveAsData;

    ColorLabel* labelBgColor;
    ColorLabel* labelLineColor;
    ColorLabel* labelPathColor;
    ColorLabel* labelObstacleColor;
    ColorLabel* labelSrcColor;
    ColorLabel* labelDestColor;
    ColorLabel* labelVisitedColor;
    QPushButton* btnResetColors;

    QCheckBox* checkBoxEnableDiagonalMove;
    QCheckBox* checkBoxShowVisited;
    QSlider* sliderVisitedInterval;
    QLineEdit* editVisitedInterval;
    QPushButton* btnSetVisitedInterval;

    QComboBox* comboxFindPathList;

    QPushButton* btnSetSource;
    QPushButton* btnSetDestination;
    QPushButton* btnFindPath;
    QPushButton* btnStopFindPath;
    QPushButton* btnResetAll;
    QPushButton* btnClearPath;
    QPushButton* btnRandomObstacles;
    QPushButton* btnExtend;

    QTextEdit* logTextEdit;
    int logId = 0;

    FindPathBase* findPathBase = nullptr;
    ControlPanelExtend* controlPanelExtend = nullptr;
};

#endif // CONTROLDIALOG_H
