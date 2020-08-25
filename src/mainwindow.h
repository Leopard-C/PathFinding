#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "world.h"
#include "controldialog.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

    void createControlDialog();

public slots:
    void onLoadData();
    void onSaveAsData();
    void onSaveAsImage();

protected:
    void closeEvent(QCloseEvent* ev) override;

private:
    World* world;
    ControlDialog* controlDialog;

    QAction* actionShowControlDialog;
    QAction* actionLoadData;
    QAction* actionSaveAsData;
    QAction* actionSaveAsImage;
};
#endif // MAINWINDOW_H
