#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIntValidator>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    instanceHolder = this;
    ui->setupUi(this);

    validator = new QIntValidator(0, 255, this);
    ui->redEdit->setValidator(validator);
    ui->greenEdit->setValidator(validator);
    ui->blueEdit->setValidator(validator);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete validator;
}

MainWindow* MainWindow::instanceHolder = nullptr;

MainWindow* MainWindow::instance()
{
    if (instanceHolder == nullptr) return new MainWindow();
    return instanceHolder;
}

void MainWindow::on_updateButton_clicked()
{
    GLint r = ui->redEdit->text().toInt();
    GLint g = ui->greenEdit->text().toInt();
    GLint b = ui->blueEdit->text().toInt();

    emit updateColorSignal(r, g, b);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index != 1) return;

    QVector<QPointF> points = ui->roomEditor->getPoints();
    QVector<int> indices = ui->roomEditor->triangulate();
    emit updateGeometrySignal(points, indices);
}
