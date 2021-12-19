#pragma once

#include "UsefulInclude.h"
#include <QSqlDatabase>
#include <QSql>
#include <QSqlQuery>
#include <QFileInfo>

class Database {
private:
    QSqlDatabase db;
    QString dbName;  // bare db path without .sqlite
    static int connectionCount;  // for generating new name for each db connection in different thread

public:
    explicit Database(QString dbname);
    ~Database();

    // Utility function for multi threading (new database connection)
    [[nodiscard]] QString getDbName() const;
    [[nodiscard]] bool hasData() const;

    // CREATE and DROP statement
    void resetTable() const;

    // INSERT statement, enable transaction for fast INSERT
    void insertRow(const QList<QString>& lst) const;
    void startTransaction() const;
    void stopTransaction() const;

    // various SELECT statements, they mostly have the same structure, only differ in query statement and return result
    [[nodiscard]] int getMaxID(bool isUser) const;
    [[nodiscard]] pair<QDate, QDate> getDateRange() const;
    [[nodiscard]] vector<pair<double, double>> getAllPOI() const;
    [[nodiscard]] vector<pair<int, int>> getTop10(const pair<int, int> &idRange, bool isUser, const QDateTime& startDate, const QDateTime& endDate) const;
    [[nodiscard]] vector<pair<QDateTime, int>> getUsersOverTimeInArea(const pair<int, int> &latitudeBound, const pair<int, int> &longitudeBound, const pair<QDateTime, QDateTime> &timeRange) const;
    [[nodiscard]] vector<pair<QDateTime, int>> getMonthlyFromID(int id1, int id2, const pair<QDateTime, QDateTime> &timeRange, bool isUser) const;
    [[nodiscard]] vector<pair<QString, pair<int, int>>> getUserDataSimplified(int id) const;
    [[nodiscard]] vector<pair<int, int>> getInterpolateInput(int poiID1, int poiID2) const;
};
