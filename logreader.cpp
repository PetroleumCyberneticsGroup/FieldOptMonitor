#include "logreader.h"
#include <QFileInfoList>
#include <QDebug>
#include <QFile>

LogReader::LogReader(QDir *directory)
{
    directory_ = directory;
    directory_->setNameFilters(QStringList() << "log_*.csv");
    Refresh();
}

void LogReader::Refresh()
{
    QFileInfoList log_info = directory_->entryInfoList();

    foreach (QFileInfo file, log_info) {
        if (file.baseName().contains("settings")) {
            lp_settings_ = file.absoluteFilePath();
            log_settings_ = new CsvFile(lp_settings_);
        }
        else if (file.baseName().contains("cases")) {
            lp_cases_ = file.absoluteFilePath();
            log_cases_ = new CsvFile(lp_cases_);
        }
        else if (file.baseName().contains("optimization")) {
            lp_optimization_ = file.absoluteFilePath();
            log_optimization_ = new CsvFile(lp_optimization_);
        }
        else if (file.baseName().contains("simulation")) {
            lp_simulation_ = file.absoluteFilePath();
            log_simulation_ = new CsvFile(lp_simulation_);
        }
        else if (file.baseName().contains("property_uuid")) {
            lp_uuidmap_ = file.absoluteFilePath();
            log_uuidmap_ = new CsvFile(lp_uuidmap_);
        }
    }
}

int LogReader::GetMaxSims()
{
    if (log_settings_ == 0) return 0;
    else return log_settings_->GetColumnValues("maxevals").first().toInt();
}

int LogReader::GetCurSims()
{
    if (log_optimization_ == 0) return 0;
    else return log_optimization_->GetColumnValues("EvaluatedCases").last().toInt();
}

double LogReader::GetMinStepLength()
{
    if (log_settings_ == 0) return 0;
    else return log_settings_->GetColumnValues("minstep").first().toDouble();
}

double LogReader::GetMaxStepLength()
{
    if (log_settings_ == 0) return 0;
    else return log_settings_->GetColumnValues("initstep").first().toDouble();
}

double LogReader::GetCurStepLength()
{
    if (log_optimization_ == 0) return 0;
    else return log_optimization_->GetColumnValues("StepLength").last().toDouble();
}

QString LogReader::GetTentBestUuid()
{
    if (log_optimization_ == 0) return "";
    else return log_optimization_->GetColumnValues("TentativeBestCaseID").last();
}

double LogReader::GetTentBestOFValue()
{
    if (log_optimization_ == 0) return 0;
    else return log_optimization_->GetColumnValues("TentativeBestCaseOFValue").last().toDouble();
}

int LogReader::GetCurrentIteration()
{
    if (log_optimization_ == 0) return 0;
    else return log_optimization_->GetColumnValues("Iteration").last().toInt();
}

QList<QPair<QString, double> > LogReader::GetTentBestVariableValues()
{
    if (log_optimization_ == 0 || log_cases_ == 0) return QList<QPair<QString, double> >();

    QList<QPair<QString, double> > values = QList<QPair<QString, double> >();
    QString best_case_id_ = GetTentBestUuid();
    QStringList ReaVarID = log_cases_->GetColumnValues("ReaVarID");
    QStringList ReaVarVal = log_cases_->GetColumnValues("ReaVarVal");
    QStringList CaseIDs = log_cases_->GetColumnValues("CaseID");
    QStringList CaseEvaluated = log_cases_->GetColumnValues("Evaluated");

    for (int i = 0; i < CaseIDs.length(); ++i) {
        if (QString::compare(CaseIDs[i], best_case_id_) == 0) {
            if (QString::compare(CaseEvaluated[i], "true") == 0) {
                QString var_name = getPropertyName(ReaVarID[i]);
                double var_val = ReaVarVal[i].toDouble();
                values.append(QPair<QString, double>(var_name, var_val));
            }
        }
    }
    return values;
}

QVector<double> LogReader::GetObjectiveFunctionValues()
{
    if (log_optimization_ == 0) return QVector<double>();
    QStringList values = log_optimization_->GetColumnValues("TentativeBestCaseOFValue");

    QVector<double> obj_fun_vals = QVector<double>();
    for (int i = 0; i < values.length(); ++i) {
        obj_fun_vals.append(values[i].toDouble());
    }
    return obj_fun_vals;
}

QString LogReader::getPropertyName(QString uuid)
{
    if (log_uuidmap_ == 0) return "";

    QStringList uuids = log_uuidmap_->GetColumnValues("UUID");
    QStringList names = log_uuidmap_->GetColumnValues("name");
    for (int i = 0; i < uuids.length(); ++i) {
        if (QString::compare(uuid, uuids[i]) == 0)
            return names[i];
    }
    return QString();
}


LogReader::CsvFile::CsvFile(QString path)
{
    QStringList file_contents = readFile(path);
    headers_ = file_contents.first().split(",");
    QVector<QVector<QString> > matrix = buildCsvMatrix(file_contents);

    for (int col = 0; col < matrix.first().length(); ++col) {
        QStringList column = QStringList();
        for (int row = 1; row < matrix.length(); ++row) { // Skipping header line
            column.append(matrix[row][col]);
        }
        columns_[headers_[col]] = column;
    }
}

QStringList LogReader::CsvFile::GetHeaders()
{
    return headers_;
}

QStringList LogReader::CsvFile::GetColumnValues(QString column_header)
{
    return columns_[column_header];
}

QStringList LogReader::CsvFile::readFile(QString path)
{
    QStringList string_list = QStringList();
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream text_stream(&file);
    while (true) {
        QString line = text_stream.readLine();
        if (line.isNull())
            break;
        else
            string_list.append(line);
    }
    file.close();
    return string_list;
}

QVector<QVector<QString> > LogReader::CsvFile::buildCsvMatrix(QStringList lines)
{
    QVector<QVector<QString> > matrix = QVector<QVector<QString> >();
    foreach (QString line, lines) {
        if (line.split(",").length() > 1)
            matrix.append(line.split(",").toVector());
    }
    return matrix;
}
