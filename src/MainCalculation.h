#pragma once

#include "UsefulInclude.h"
#include "Database.h"
#include <QThread>


class MainCalculation: public QThread {
Q_OBJECT
public slots:
    // Create a new connection to a database in this thread
    void recordData(const QString &dbname, shared_ptr<InspectData> inspect);

    // Calculate top 10 POI or USER in a time frame
    void calculateTop10(pair<int, int> id, bool calculateUser, const QDateTime& startDate, const QDateTime& endDate);

    // Calculate total user over time in a GPS bound and time range
    void calculateTotalUserOverTimeSetPoints(const pair<int, int> &latitudeBound, const pair<int, int> &longitudeBound, const pair<QDateTime, QDateTime> &timeRange);

    // Calculate two user/POI data over a specific time range
    void calculateCompareTwo(pair<int, int> id1, pair<int, int> id2, const pair<QDateTime, QDateTime> &timeRange, bool isUser);

    // Calculate newton algorithm and return the newton model
    void calculateCubicCache(int POIId1, int POIId2);

signals:
    // Update percentage loaded in first tab or second tab
    void percentageLoaded(double percentage);
    void percentageLoadedExtra(double percentage);

    // Signals for finished calculation, each one corresponding to the slot on top, return calculation result
    void finishedTop10(shared_ptr<vector<pair<int, int>>> res, bool calculateUser);
    void finishedTotalUserOverTimeSetPoints(shared_ptr<vector<pair<QDateTime, int>>> res);
    void finishedCompareTwo(shared_ptr<vector<pair<QDateTime, int>>> res1, shared_ptr<vector<pair<QDateTime, int>>> res2, bool isUser);
    void finishedCubicCache(pair<int, int> idBound, shared_ptr<vector<pair<int, int>>> y);

private:
    shared_ptr<Database> db;  // current thread db conenction
};



