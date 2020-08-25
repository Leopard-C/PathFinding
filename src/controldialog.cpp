#include "controldialog.h"
#include <QDebug>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QFrame>
#include <QGroupBox>
#include <QMessageBox>
#include "extend/controlpanel_extend_astar.h"
#include "findpath/astar.h"
#include "findpath/bstar.h"
#include "findpath/bfs.h"
#include "findpath/dfs.h"

#define GROUPBOX_QSS \
        "QGroupBox{border-width:1px;border-style:solid;"\
        "border-radius:5px;border-color:gray;margin-top:1ex;}"\
        "QGroupBox::title{subcontrol-origin:margin;subcontrol-position:top left;"\
        "left:10px;margin-left:3px;padding:0 1px;}"


ControlDialog::ControlDialog(World* world, QWidget* parent)
    : QDialog(parent), world(world)
{
    this->setWindowTitle("Control Panel");

    //  1. Find Path Method
    //
    comboxFindPathList = new QComboBox(this);
    comboxFindPathList->addItem("A Star");
    comboxFindPathList->addItem("B Star");
    comboxFindPathList->addItem("BFS");
    comboxFindPathList->addItem("DFS");
    onFindPathMethodChanged("A Star");
    controlPanelExtend = new ControlPanelExtendAStar(this, world, nullptr);
    connect(comboxFindPathList, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(onFindPathMethodChanged(QString)));
    connect(world, &World::sigFindPathStopped, this, [this]{
        comboxFindPathList->setEnabled(true);
    });

    // 2. World Size
    //
    QSize worldSize = world->getWorldSize();
    editWorldWidth = new QLineEdit(QString::number(worldSize.width()), this);
    editWorldHeight = new QLineEdit(QString::number(worldSize.height()), this);
    editWorldWidth->setFixedWidth(60);
    editWorldHeight->setFixedWidth(60);
    editWorldWidth->setAlignment(Qt::AlignCenter);
    editWorldHeight->setAlignment(Qt::AlignCenter);
    btnSetWorldSize = new QPushButton("Set", this);
    btnSetWorldSize->setFixedSize(40, 55);
    connect(btnSetWorldSize, &QPushButton::clicked, this, &ControlDialog::onSetWorldSize);
    connect(world, &World::sigUpdateWorldSize, this, [&](int w, int h){
        editWorldWidth->setText(QString::number(w));
        editWorldHeight->setText(QString::number(h));
    });

    QGridLayout* worldSizeLayout = new QGridLayout();
    QLabel* label1 = new QLabel("Width:", this);
    label1->setFixedWidth(60);
    QLabel* label2 = new QLabel("Height:", this);
    label2->setFixedWidth(60);
    worldSizeLayout->addWidget(label1, 0, 0);
    worldSizeLayout->addWidget(editWorldWidth, 0, 1);
    worldSizeLayout->addWidget(label2, 1, 0);
    worldSizeLayout->addWidget(editWorldHeight, 1, 1);
    worldSizeLayout->addWidget(btnSetWorldSize, 0, 2, 2, 1);
    worldSizeLayout->setSizeConstraint(QLayout::SetFixedSize);

    QGroupBox* worldSizeGroup = new QGroupBox("World Size", this);
    worldSizeGroup->setLayout(worldSizeLayout);
    worldSizeGroup->setStyleSheet(GROUPBOX_QSS);

    // 3. Colors
    //
    QGridLayout* colorLayout = new QGridLayout();
    labelBgColor = new ColorLabel(60, 25, world->getBgColor(), this);
    labelLineColor = new ColorLabel(60, 25, world->getLineColor(), this);
    labelPathColor = new ColorLabel(60, 25, world->getPathColor(), this);
    labelObstacleColor = new ColorLabel(60, 25, world->getObstacleColor(), this);
    labelSrcColor = new ColorLabel(60, 25, world->getSrcColor(), this);
    labelDestColor = new ColorLabel(60, 25, world->getDestColor(), this);
    labelVisitedColor = new ColorLabel(60, 25, world->getVisitedColor(), this);
    btnResetColors = new QPushButton(this);
    btnResetColors->setIcon(QIcon("icon/reset.png"));
    btnResetColors->setFixedWidth(60);
    colorLayout->addWidget(labelBgColor, 0, 0);
    colorLayout->addWidget(new QLabel("Background", this), 0, 1);
    colorLayout->addWidget(labelLineColor, 0, 2);
    colorLayout->addWidget(new QLabel("Line", this), 0, 3);
    colorLayout->addWidget(labelSrcColor, 1, 0);
    colorLayout->addWidget(new QLabel("Source", this), 1, 1);
    colorLayout->addWidget(labelPathColor, 1, 2);
    colorLayout->addWidget(new QLabel("Path", this), 1, 3);
    colorLayout->addWidget(labelDestColor, 2, 0);
    colorLayout->addWidget(new QLabel("Destination", this), 2, 1);
    colorLayout->addWidget(labelObstacleColor, 2, 2);
    colorLayout->addWidget(new QLabel("Obstacle", this), 2, 3);
    colorLayout->addWidget(labelVisitedColor, 3, 0);
    colorLayout->addWidget(new QLabel("Visited", this), 3, 1);
    colorLayout->addWidget(btnResetColors, 3, 2);
    colorLayout->addWidget(new QLabel("Reset", this), 3, 3);
    connect(labelBgColor, &ColorLabel::colorChanged, world, &World::setBgColor);
    connect(labelLineColor, &ColorLabel::colorChanged, world, &World::setLineColor);
    connect(labelPathColor, &ColorLabel::colorChanged, world, &World::setPathColor);
    connect(labelObstacleColor, &ColorLabel::colorChanged, world, &World::setObstacleColor);
    connect(labelSrcColor, &ColorLabel::colorChanged, world, &World::setSrcColor);
    connect(labelDestColor, &ColorLabel::colorChanged, world, &World::setDestColor);
    connect(labelVisitedColor, &ColorLabel::colorChanged, world, &World::setVisitedColor);
    connect(btnResetColors, &QPushButton::clicked, this, [this]{
        this->world->resetColors();
        this->world->refreshAll();
        this->labelBgColor->setColor(this->world->getBgColor());
        this->labelLineColor->setColor(this->world->getLineColor());
        this->labelPathColor->setColor(this->world->getPathColor());
        this->labelObstacleColor->setColor(this->world->getObstacleColor());
        this->labelSrcColor->setColor(this->world->getSrcColor());
        this->labelDestColor->setColor(this->world->getDestColor());
        this->labelVisitedColor->setColor(this->world->getVisitedColor());
    });

    QGroupBox* colorGroup = new QGroupBox("Color", this);
    colorGroup->setStyleSheet(GROUPBOX_QSS);
    colorGroup->setLayout(colorLayout);

    // 4. Visited Path
    //
    checkBoxEnableDiagonalMove = new QCheckBox("Diagonal Move", this);
    checkBoxEnableDiagonalMove->setChecked(false);
    checkBoxShowVisited = new QCheckBox("Show Visited Path", this);
    checkBoxShowVisited->setChecked(world->isShowVisitedPath());
    sliderVisitedInterval = new QSlider(Qt::Orientation::Horizontal, this);
    sliderVisitedInterval->setRange(1, 100);
    sliderVisitedInterval->setValue(10);
    editVisitedInterval = new QLineEdit(this);
    editVisitedInterval->setFixedWidth(60);
    editVisitedInterval->setText(QString::number(world->getShowVisitedPathInterval()));
    btnSetVisitedInterval = new QPushButton("Set", this);
    QLabel* label0 = new QLabel("Intervals(ms):");
    label0->setFixedWidth(100);
    connect(checkBoxEnableDiagonalMove, &QCheckBox::stateChanged, world, &World::setEnableDiagonalMove);
    connect(checkBoxShowVisited, &QCheckBox::stateChanged, this, [&](int state){
        this->findPathBase->setShowVisitedPath(state == Qt::Checked);
        this->world->setShowVisitedPath(state == Qt::Checked);
    });
    connect(btnSetVisitedInterval, &QPushButton::clicked, this, [&]{
        QString intervalStr = editVisitedInterval->text();
        if (intervalStr.size() > 0) {
            int interval = intervalStr.toInt();
            if (interval > 1) {
                this->world->setShowVisitedPathInterval(interval);
                this->findPathBase->setShowVisitedPathInterval(interval);
                logInfo(QString("Set interval to %0").arg(interval));
            }
            else {
                logError("Interval should be 2 at least");
            }
        }
    });
    connect(sliderVisitedInterval, &QSlider::valueChanged, this, [&](int value){
        this->world->setShowVisitedPathInterval(value);
        this->findPathBase->setShowVisitedPathInterval(value);
        this->editVisitedInterval->setText(QString::number(value));
        logInfo(QString("Set interval to %0").arg(value));
    });

    QGridLayout* btnLayout1_5 = new QGridLayout();
    btnLayout1_5->addWidget(checkBoxEnableDiagonalMove, 0, 0, 1, 3);
    btnLayout1_5->addWidget(checkBoxShowVisited, 1, 0, 1, 3);
    btnLayout1_5->addWidget(label0, 2, 0);
    btnLayout1_5->addWidget(editVisitedInterval, 2, 1);
    btnLayout1_5->addWidget(btnSetVisitedInterval, 2, 2);
    btnLayout1_5->addWidget(sliderVisitedInterval, 3, 0, 1, 3);
    btnLayout1_5->setSizeConstraint(QLayout::SetFixedSize);

    QGroupBox* btnGroupBox1_5 = new QGroupBox("Settings", this);
    btnGroupBox1_5->setStyleSheet(GROUPBOX_QSS);
    btnGroupBox1_5->setLayout(btnLayout1_5);

    // 5. Control Buttons
    //    + Source and Destination
    //    + start / stop find path
    //    + clear / reset
    //    + generate random obstacles
    //    * extended control panel
    //
    QGridLayout* btnLayout2 = new QGridLayout();
    btnSetSource = new QPushButton("Set Source", this);
    btnSetDestination = new QPushButton("Set Destination", this);
    btnFindPath = new QPushButton("Find Path", this);
    btnStopFindPath = new QPushButton("Stop", this);
    btnClearPath = new QPushButton("Clear Path", this);
    btnResetAll = new QPushButton("Reset", this);
    btnRandomObstacles = new QPushButton("Random Obstacles", this);
    btnExtend = new QPushButton("· · ·", this);
    btnLayout2->addWidget(btnSetSource, 0, 0);
    btnLayout2->addWidget(btnSetDestination, 0, 1);
    btnLayout2->addWidget(btnFindPath, 1, 0);
    btnLayout2->addWidget(btnStopFindPath, 1, 1);
    btnLayout2->addWidget(btnClearPath, 2, 0);
    btnLayout2->addWidget(btnResetAll, 2, 1);
    btnLayout2->addWidget(btnRandomObstacles, 3, 0);
    btnLayout2->addWidget(btnExtend, 3, 1);
    connect(btnSetSource, &QPushButton::clicked, world, &World::startSetSource);
    connect(btnSetDestination, &QPushButton::clicked, world, &World::startSetDestination);
    connect(btnResetAll, &QPushButton::clicked, world, &World::resetAll);
    connect(btnClearPath, &QPushButton::clicked, world, &World::clearPath);
    connect(btnFindPath, &QPushButton::clicked, this, [this]{
        this->comboxFindPathList->setDisabled(true);
        this->world->findPath();
    });
    connect(btnStopFindPath, &QPushButton::clicked, world, &World::stopFindPath);
    connect(btnRandomObstacles, &QPushButton::clicked, world, [&]{
        QDialog* dialog = new QDialog(nullptr);
        QLabel* label = new QLabel("Ratio(0.001-0.999):", dialog);
        QLineEdit* edit = new QLineEdit("0.3", dialog);
        QHBoxLayout* hlayout = new QHBoxLayout();
        QPushButton* btnOK = new QPushButton("OK", dialog);
        connect(btnOK, &QPushButton::clicked, dialog, [this,edit,dialog]{
            QString ratioStr = edit->text();
            if (ratioStr.size() > 0) {
                this->world->setRandomObstacles(ratioStr.toDouble());
                dialog->close();
            }
        });
        hlayout->addWidget(label);
        hlayout->addWidget(edit);
        QVBoxLayout* vlayout = new QVBoxLayout(dialog);
        vlayout->addLayout(hlayout);
        vlayout->addWidget(btnOK);
        dialog->setModal(true);
        dialog->show();
    });
    connect(btnExtend, &QPushButton::clicked, this, [&]{
        if (controlPanelExtend) {
            controlPanelExtend->show();
        }
        else {
            logError("No extended control panel.");
        }
    });

    QGroupBox* btnGroupBox2 = new QGroupBox("Control", this);
    btnGroupBox2->setLayout(btnLayout2);
    btnGroupBox2->setStyleSheet(GROUPBOX_QSS);

    // 6. log message box
    //
    QVBoxLayout* logLayout = new QVBoxLayout();
    logTextEdit = new QTextEdit(this);
    logLayout->addWidget(logTextEdit);
    connect(world, &World::sigLogInfo, this, &ControlDialog::logInfo);
    connect(world, &World::sigLogError, this, &ControlDialog::logError);
    QGroupBox* logGroupBox = new QGroupBox("Log Message", this);
    logGroupBox->setLayout(logLayout);
    logGroupBox->setStyleSheet(GROUPBOX_QSS);

    QVBoxLayout* controlWidgetLayout = new QVBoxLayout(this);
    controlWidgetLayout->addWidget(comboxFindPathList);
    controlWidgetLayout->addWidget(worldSizeGroup);
    controlWidgetLayout->addWidget(colorGroup);
    controlWidgetLayout->addWidget(btnGroupBox1_5);
    controlWidgetLayout->addWidget(btnGroupBox2);
    controlWidgetLayout->addWidget(logGroupBox);
}


void ControlDialog::onSetWorldSize()
{
    QString strWorldWidth = editWorldWidth->text();
    QString strWorldHeight = editWorldHeight->text();
    if (strWorldWidth.isNull() || strWorldWidth.isEmpty() ||
        strWorldHeight.isNull() || strWorldHeight.isEmpty())
    {
        logError("Please input world size.");
        return;
    }

    int worldWidth = strWorldWidth.toInt();
    int worldHeight = strWorldHeight.toInt();

    world->setWorldSize(worldWidth, worldHeight);
}

/*  log message
***/
void ControlDialog::logMessage(QString msg, bool error/* = false*/) {
    if (error) {
        logTextEdit->setTextColor(Qt::red);
    }
    else {
        logTextEdit->setTextColor(Qt::black);
    }
    if (logTextEdit->toPlainText().length() == 0) {
        logId = 0;
    }
    logTextEdit->append(QString("[%0]%1").arg(logId).arg(msg));
    logId++;
}


// SLOT
void ControlDialog::logInfo(QString msg) {
    logMessage(msg, false);
}

// SLOT
void ControlDialog::logError(QString msg) {
    logMessage(msg, true);
}

// SLOT
void ControlDialog::onFindPathMethodChanged(QString methodName) {
    FindPathBase* findPathBaseNew = nullptr;
    if (methodName == "A Star") {
        findPathBaseNew = new AStar();
        findPathBaseNew->copyFrom(this->findPathBase);
        if (controlPanelExtend) {
            delete controlPanelExtend;
        }
        controlPanelExtend = new ControlPanelExtendAStar(this, world, nullptr);
    }
    else if (methodName == "B Star") {
        findPathBaseNew = new BStar();
        if (controlPanelExtend) {
            delete controlPanelExtend;
            controlPanelExtend = nullptr;
        }
    }
    else if (methodName == "BFS") {
        findPathBaseNew = new BFS();
        if (controlPanelExtend) {
            delete controlPanelExtend;
            controlPanelExtend = nullptr;
        }
        //controlPanelExtend = new ControlPanelExtendAStar(world, nullptr);
    }
    else if (methodName == "DFS") {
        findPathBaseNew = new DFS();
        if (controlPanelExtend) {
            delete controlPanelExtend;
            controlPanelExtend = nullptr;
        }
        //controlPanelExtend = new ControlPanelExtendAStar(world, nullptr);
    }
    else {
        return;
    }

    if (findPathBase) {
        findPathBaseNew->copyFrom(findPathBase);
        delete findPathBase;
    }

    findPathBase = findPathBaseNew;
    world->findPathBase = findPathBase;

    connect(findPathBase, &FindPathBase::sigVisited, world, &World::visited);
    connect(findPathBase, &FindPathBase::sigTimeUsed, this, [&](long us){
        if (us < 1e3) {
            logInfo(QString("Time used: %0us").arg(us));
        }
        else if (us < 1e6) {
            logInfo(QString("Time used: %0ms").arg(us / 1e3));
        }
        else {
            logInfo(QString("Time used: %0s").arg(us / 1e6));
        }
    });
}
