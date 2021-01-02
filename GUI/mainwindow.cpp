#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{




    ui->setupUi(this);
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/rec/BusB.png"), "111");
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/rec/BusR.png"), "222");
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(":/rec/BusG.png"), "333");
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(":/rec/BusY.png"), "444");

    ui->listWidgetTitle->addItem("노선번호                  도착예정                  현재위치");
    ui->listWidgetBus->addItem(item1);
    ui->listWidgetBus->addItem(item2);
    ui->listWidgetBus->addItem(item3);
    ui->listWidgetBus->addItem(item4);

    ui->listWidgetTime->addItem("");
    ui->listWidgetTime->addItem("");
    ui->listWidgetTime->addItem("");
    ui->listWidgetTime->addItem("");

    ui->listWidgetState->addItem("");
    ui->listWidgetState->addItem("");
    ui->listWidgetState->addItem("");
    ui->listWidgetState->addItem("");
}

MainWindow::~MainWindow()
{
    delete ui;
}
