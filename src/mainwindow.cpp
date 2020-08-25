#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setMouseTracking(true);
    this->setMinimumSize(600, 400);
    this->setWindowTitle("World");

    // Type 1.
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setMouseTracking(true);
    this->setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    world = new World(centralWidget);
    mainLayout->addWidget(world);

    // or Type 2.
    //world = new World(this);
    //this->setCentralWidget(world);

    QMenu* fileMenu = menuBar()->addMenu("FILE");
    actionLoadData = new QAction("Load Data", this);
    actionSaveAsData  = new QAction("Save As Data",  this);
    actionSaveAsImage = new QAction("Save As Image", this);
    actionSaveAsData->setIcon(QIcon("icon/save-as-data.png"));
    actionSaveAsImage->setIcon(QIcon("icon/save-as-image.png"));
    actionLoadData->setIcon(QIcon("icon/load-data.png"));
    fileMenu->addAction(actionLoadData);
    fileMenu->addAction(actionSaveAsData);
    fileMenu->addAction(actionSaveAsImage);
    connect(actionLoadData,  &QAction::triggered, this, &MainWindow::onLoadData);
    connect(actionSaveAsData,  &QAction::triggered, this, &MainWindow::onSaveAsData);
    connect(actionSaveAsImage, &QAction::triggered, this, &MainWindow::onSaveAsImage);

    QMenu* windowMenu = menuBar()->addMenu("WINDOW");
    actionShowControlDialog = new QAction("Control Dialog", this);
    actionShowControlDialog->setCheckable(true);
    actionShowControlDialog->setChecked(true);
    windowMenu->addAction(actionShowControlDialog);
    connect(actionShowControlDialog, &QAction::triggered, this, [&](bool checked){
        if (checked) {
            controlDialog->show();
        }
        else {
            controlDialog->hide();
        }
    });

    createControlDialog();
    controlDialog->show();
}

void MainWindow::createControlDialog() {
    controlDialog = new ControlDialog(world, nullptr);
    connect(controlDialog, &ControlDialog::sigClosed, this, [&]{
        actionShowControlDialog->setChecked(false);
    });
}

void MainWindow::closeEvent(QCloseEvent*) {
    controlDialog->disconnect();  // Important !!!
    controlDialog->close();
}


void MainWindow::onLoadData() {
    if (world->isFindingPath()) {
        emit controlDialog->logError("Can't load data while finding path.");
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this, "Select file", ".", "data file(*.dat)");
    if (filename.isNull()) {
        return;
    }
    if (world->loadData(filename)) {
        world->refreshAll();
        controlDialog->logInfo("Load data successfully!");
    }
    else {
        controlDialog->logError("Load data failed!");
    }
}

void MainWindow::onSaveAsData() {
    if (world->isFindingPath()) {
        emit controlDialog->logError("Can't save data while finding path.");
        return;
    }

    QDialog* dialog = new QDialog(nullptr);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* checkboxLayout = new QVBoxLayout();
    QCheckBox* checkboxObstacles = new QCheckBox("Obstacles", dialog);
    QCheckBox* checkboxSrc  = new QCheckBox("Source", dialog);
    QCheckBox* checkboxDest = new QCheckBox("Destination", dialog);
    QCheckBox* checkboxPath = new QCheckBox("Path", dialog);
    QCheckBox* checkboxVisited = new QCheckBox("Visited", dialog);
    checkboxObstacles->setChecked(true);
    checkboxSrc->setChecked(false);
    checkboxDest->setChecked(false);
    checkboxPath->setChecked(false);
    checkboxVisited->setChecked(false);
    checkboxLayout->addWidget(checkboxObstacles);
    checkboxLayout->addWidget(checkboxSrc);
    checkboxLayout->addWidget(checkboxDest);
    checkboxLayout->addWidget(checkboxPath);
    checkboxLayout->addWidget(checkboxVisited);
    QPushButton* btnOK = new QPushButton("OK", dialog);
    QPushButton* btnCancel = new QPushButton("Cancel", dialog);
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnOK);
    btnLayout->addWidget(btnCancel);
    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    mainLayout->addLayout(checkboxLayout);
    mainLayout->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::close);
    connect(btnOK, &QPushButton::clicked, this, [=]{
        dialog->close();
        QString filename = QFileDialog::getSaveFileName(this, "Select file", ".", "data file(*.dat)");
        if (filename.isNull()) {
            return;
        }
        bool ret = world->saveData(filename, checkboxObstacles->isChecked(),
                                   checkboxSrc->isChecked(), checkboxDest->isChecked(),
                                   checkboxPath->isChecked(), checkboxVisited->isChecked());
        if (ret) {
            controlDialog->logInfo("Save data successfully!");
        }
        else {
            controlDialog->logError("Save data failed!");
        }
    });
    dialog->setModal(true);
    dialog->show();
}

void MainWindow::onSaveAsImage() {
    if (world->isFindingPath()) {
        emit controlDialog->logError("Can't save image while finding path.");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Select file", ".", "image(*.png)");
    if (filename.isNull()) {
        return;
    }

    if (world->saveImage(filename)) {
        controlDialog->logInfo("Save image successfully!");
    }
    else {
        controlDialog->logError("Save image failed!");
    }
}

