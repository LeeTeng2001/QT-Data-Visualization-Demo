#pragma once

#include "UsefulInclude.h"
#include <QSqlDatabase>
#include <QSql>
#include <QSqlQuery>
#include <QFileInfo>

class Database {
private:
    QSqlDatabase db;
    QString dbName;  // bare db name without .sqlite
    static int connectionCount;  // for generating new name for each db connection in different thread

public:
    explicit Database(QString dbname);
    ~Database();

    // Utility function for multi threading (new database connection)
    [[nodiscard]] QString getDbName() const;
    bool hasData();
    void resetTable();

    // INSERT statement, enable transaction for fast INSERT
    void insertRow(const QList<QString>& lst);
    void startTransaction();
    void stopTransaction();

    // various SELECT statements, they mostly have the same structure, only differ in query statement and return result
    int getMaxID(bool isUser);
    pair<QDate, QDate> getDateRange();
    vector<pair<double, double>> getAllPOI();
    vector<pair<int, int>> getTop10(int id, bool isUser, const QDateTime& startDate, const QDateTime& endDate);
    vector<pair<QDateTime, int>> getUsersOverTimeInArea(const pair<int, int> &latitudeBound, const pair<int, int> &longitudeBound, const pair<QDateTime, QDateTime> &timeRange);
    vector<pair<QDateTime, int>> getMonthlyFromID(int id, const pair<QDateTime, QDateTime> &timeRange, bool isUser);
    vector<pair<QString, pair<int, int>>> getUserDataSimplified(int id);
    vector<pair<int, int>> getNewtonInput(int poiID1, int poiID2);
};

