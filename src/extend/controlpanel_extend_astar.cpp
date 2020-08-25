#include "controlpanel_extend_astar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include "../findpath/astar.h"

#define GROUPBOX_QSS \
        "QGroupBox{border-width:1px;border-style:solid;"\
        "border-radius:5px;border-color:gray;margin-top:1ex;}"\
        "QGroupBox::title{subcontrol-origin:margin;subcontrol-position:top left;"\
        "left:10px;margin-left:3px;padding:0 1px;}"


ControlPanelExtendAStar::ControlPanelExtendAStar(ControlDialog* mainControlDialog, World* world, QWidget* parent)
    : ControlPanelExtend(mainControlDialog, world, parent)
{
    setupLayout();
}

void ControlPanelExtendAStar::setupLayout() {
    // Enable diagonal move
    // Heruistic function
    //
    QGridLayout* btnLayout1 = new QGridLayout();

    groupButton = new QButtonGroup(this);
    radioBtnManhattan = new QRadioButton("Manhattan", this);
    radioBtnEuclidean = new QRadioButton("Euclidean", this);
    groupButton->addButton(radioBtnManhattan, 0);
    groupButton->addButton(radioBtnEuclidean, 1);
    radioBtnManhattan->setChecked(true);
    btnLayout1->addWidget(radioBtnManhattan, 1, 0);
    btnLayout1->addWidget(radioBtnEuclidean, 1, 1);
    connect(groupButton, SIGNAL(buttonClicked(int)), this, SLOT(onHeuristicTypeChanged(int)));

    QGroupBox* btnGroupBox1 = new QGroupBox("Settings", this);
    btnGroupBox1->setStyleSheet(GROUPBOX_QSS);
    btnGroupBox1->setLayout(btnLayout1);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(btnGroupBox1);
}

// SLOT
//
void ControlPanelExtendAStar::onHeuristicTypeChanged(int id) {
    AStar* astar = static_cast<AStar*>(world->findPathBase);
    if (astar) {
        if (id == 0) {
            astar->setManhattanHeuristicFunction();
            emit sigLogInfo("Set heuristic function to 'Manhattan'.");
        }
        else {
            astar->setEuclideanHeuristicFunction();
            emit sigLogInfo("Set heuristic function to 'Euclidean'.");
        }
    }
}

