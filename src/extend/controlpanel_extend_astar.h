#ifndef CONTROLPANEL_EXTEND_ASTAR
#define CONTROLPANEL_EXTEND_ASTAR

#include <QDialog>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include "controlpanelextend.h"
#include "../world.h"

class ControlPanelExtendAStar : public ControlPanelExtend
{
   Q_OBJECT
public:
    enum HeuristicType {
        MANHATTAN = 0,
        EUCLIDEAN = 1
    };

public:
    ControlPanelExtendAStar(ControlDialog* mainControlDialog, World* world, QWidget* parent = nullptr);

public slots:
    void onHeuristicTypeChanged(int id);

private:
    void setupLayout();

private:
    QButtonGroup* groupButton;
    QCheckBox* checkBoxEnableDiagonalMove;
    QRadioButton* radioBtnManhattan;
    QRadioButton* radioBtnEuclidean;

    HeuristicType heuristicType = MANHATTAN;
};

#endif
