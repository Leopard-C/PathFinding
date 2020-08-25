#include "controlpanelextend.h"
#include "../controldialog.h"


ControlPanelExtend::ControlPanelExtend(ControlDialog* mainControlDialog, World* world, QWidget* parent)
    : QDialog(parent), mainControlDialog(mainControlDialog), world(world)
{
    connect(this, &ControlPanelExtend::sigLogInfo, mainControlDialog, &ControlDialog::logInfo);
    connect(this, &ControlPanelExtend::sigLogError, mainControlDialog, &ControlDialog::logError);
}
