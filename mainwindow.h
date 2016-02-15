#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <QDir>
#include "logreader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_open_triggered();

    void on_action_refresh_triggered();

private:
    Ui::MainWindow *ui;
    QDir *directory_;
    LogReader *log_reader_;
    QwtPlotCurve *curve_;

    void refresh();
    void setObjectiveFunctionPlotInitialValues();
    QVector<double> createXVector(int length);

    void setPlotDummyData();
    void setProgressDummyData();
    void setVariableValuesDummyData();
    void setTentativeBestCaseDummyData();
};

#endif // MAINWINDOW_H
