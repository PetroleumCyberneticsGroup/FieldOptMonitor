#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <QVector>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    log_reader_ = 0;
    setObjectiveFunctionPlotInitialValues();

    setPlotDummyData();
    setProgressDummyData();
    setVariableValuesDummyData();
    setTentativeBestCaseDummyData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setObjectiveFunctionPlotInitialValues()
{
    // Titles and labels
    ui->objFunPlot->setTitle("Objective function values");
    ui->objFunPlot->setAxisTitle(QwtPlot::xBottom, "Case number");
    ui->objFunPlot->setAxisTitle(QwtPlot::yLeft, "Objective function value");

    // Background
    QBrush background = QBrush();
    background.setColor(Qt::white);
    background.setStyle(Qt::SolidPattern);
    ui->objFunPlot->setCanvasBackground(background);

    ui->objFunPlot->setAutoReplot();
}

QVector<double> MainWindow::createXVector(int length)
{
    QVector<double> vector = QVector<double>();
    for (int i = 0; i < length; ++i) {
        vector.append(i);
    }
    return vector;
}

void MainWindow::setPlotDummyData()
{
    curve_ = new QwtPlotCurve("Curve");
    QVector<double> xdata, ydata;
    xdata << 0.0 << 1.0 << 2.0 << 3.0 << 4.0;
    ydata << 5.0 << 4.0 << 3.0 << 3.0 << 1.0;
    curve_->setSamples(xdata, ydata);
    curve_->attach(ui->objFunPlot);
}

void MainWindow::setProgressDummyData()
{
    int maxSim = 60;
    int cursims = 30;
    double maxstep = 128;
    double minstep = 2;
    double curstep = 64;

    ui->progressBarSimulations->setMinimum(0);
    ui->progressBarSimulations->setMaximum(maxSim);
    ui->progressBarSimulations->setValue(cursims);

    ui->lcdNumberCurStep->display(curstep);
    ui->lcdNumberMaxStep->display(maxstep);
    ui->lcdNumberMinStep->display(minstep);
}

void MainWindow::setVariableValuesDummyData()
{
    QStandardItemModel *model = new QStandardItemModel(4, 2);
    for (int row = 0; row < 8; ++row) {
        QStandardItem *varl = new QStandardItem(QString("PROD-BHP-%1").arg(row));
        QStandardItem *varv = new QStandardItem(QString::number(1001.0+row));
        model->setItem(row, 0, varl);
        model->setItem(row, 1, varv);
    }

    ui->tableView->setModel(model);
}

void MainWindow::setTentativeBestCaseDummyData()
{
    int iterationNumber = 2;
    double objectiveFunctionValue = 1.0;
    ui->labelUuid->setText("de305d54-75b4-431b-adb2-eb6b9e546014");
    ui->lcdNumberIterationNumber->display(iterationNumber);
    ui->lcdNumberObjectiveFunctionValue->display(objectiveFunctionValue);
}



void MainWindow::on_action_open_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::List);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    QStringList selected_files;
    if (dialog.exec())
        selected_files = dialog.selectedFiles();

    QDir selected_dir = QDir(selected_files.first());
    if (!selected_dir.entryList().contains("log_settings.csv")) {
        QMessageBox msgBox;
        msgBox.setText("Invalid directory: The selected directory does not contain valid log files.");
        msgBox.exec();
    }
    else {
        directory_ = new QDir(selected_files.first());
        log_reader_ = new LogReader(directory_);
    }

    refresh();
}

void MainWindow::on_action_refresh_triggered()
{
    refresh();
}

void MainWindow::refresh()
{
    if (log_reader_ == 0) return;

    log_reader_->Refresh();

    // Simulations progress bar
    ui->progressBarSimulations->setMinimum(0);
    ui->progressBarSimulations->setMaximum(log_reader_->GetMaxSims());
    ui->progressBarSimulations->setValue(log_reader_->GetCurSims());

    // Step lengths
    ui->lcdNumberMaxStep->display(log_reader_->GetMaxStepLength());
    ui->lcdNumberMinStep->display(log_reader_->GetMinStepLength());
    ui->lcdNumberCurStep->display(log_reader_->GetCurStepLength());

    // Tentative best case
    ui->labelUuid->setText(log_reader_->GetTentBestUuid());
    ui->lcdNumberObjectiveFunctionValue->display(log_reader_->GetTentBestOFValue());
    ui->lcdNumberIterationNumber->display(log_reader_->GetCurrentIteration());

    // Objective function plot
    delete curve_;
    curve_ = new QwtPlotCurve("Objective function values");
    QVector<double> ydata = log_reader_->GetObjectiveFunctionValues();
    QVector<double> xdata = createXVector(ydata.length());
    curve_->setSamples(xdata, ydata);
    curve_->attach(ui->objFunPlot);

    // Best case property values
    QList<QPair<QString, double> > props = log_reader_->GetTentBestVariableValues();
    QStandardItemModel *model = new QStandardItemModel(props.length(), 2);
    for (int i = 0; i < props.length(); ++i) {
        QStandardItem *name = new QStandardItem(props[i].first);
        QStandardItem *value = new QStandardItem(QString::number(props[i].second));
        model->setItem(i, 0, name);
        model->setItem(i, 1, value);
        ui->tableView->setModel(model);
    }
}
