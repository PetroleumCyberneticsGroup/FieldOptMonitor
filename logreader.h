#ifndef LOGREADER_H
#define LOGREADER_H

#include <QDir>
#include <QStringList>
#include <QHash>

class LogReader
{
public:
    LogReader(QDir *directory);
    void Refresh();

    int GetMaxSims();
    int GetCurSims();

    double GetMinStepLength();
    double GetMaxStepLength();
    double GetCurStepLength();

    QString GetTentBestUuid();
    double GetTentBestOFValue();
    int GetCurrentIteration();

    QList<QPair<QString, double> > GetTentBestVariableValues();

    QVector<double> GetObjectiveFunctionValues();

private:
    QDir *directory_;

    QString lp_settings_ = "";
    QString lp_cases_ = "";
    QString lp_uuidmap_ = "";
    QString lp_optimization_ = "";
    QString lp_simulation_ = "";

    QString getPropertyName(QString uuid);

    class CsvFile
    {
    public:
        CsvFile(QString path);
        QStringList GetHeaders();
        QStringList GetColumnValues(QString column_header);

    private:
        QStringList headers_;
        QHash<QString, QStringList> columns_;
        QStringList readFile(QString path);
        QVector<QVector<QString> > buildCsvMatrix(QStringList lines);
    };

    CsvFile *log_settings_ = 0;
    CsvFile *log_cases_ = 0;
    CsvFile *log_uuidmap_ = 0;
    CsvFile *log_optimization_ = 0;
    CsvFile *log_simulation_ = 0;
};

#endif // LOGREADER_H
