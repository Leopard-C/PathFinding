#ifndef CONTROLPANELEXTEND_H
#define CONTROLPANELEXTEND_H

#include <QDialog>
#include "../findpath/findpath_base.h"

class World;
class ControlDialog;

class ControlPanelExtend : public QDialog
{
    Q_OBJECT
public:
    ControlPanelExtend(ControlDialog* mainControlDialog, World* world, QWidget* parent);

signals:
    void sigLogInfo(QString);
    void sigLogError(QString);

protected:
    ControlDialog* mainControlDialog;
    World* world;
};

#endif // CONTROLPANELEXTEND_H
