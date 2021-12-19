#include "MainCalculation.h"

void MainCalculation::recordData(const QString &dbname, shared_ptr<InspectData> inspect) {
    db = std::make_shared<Database>(dbname);
}

void MainCalculation::calculateTop10(int id, bool isUser, const QDateTime& startDate, const QDateTime& endDate) {
    emit percentageLoaded(0.5);
    auto res = std::make_shared<vector<pair<int, int>>>(std::move(db->getTop10(id, isUser, startDate, endDate)));
    emit percentageLoaded(1);
    emit finishedTop10(res, isUser);
}

void MainCalculation::calculateTotalUserOverTimeSetPoints(const pair<int, int> &latitudeBound, const pair<int, int> &longitudeBound, const pair<QDateTime, QDateTime> &timeRange) {
    emit percentageLoaded(0.5);
    auto res = std::make_shared<vector<pair<QDateTime, int>>>(std::move(db->getUsersOverTimeInArea(latitudeBound, longitudeBound, timeRange)));
    emit percentageLoaded(1);
    emit finishedTotalUserOverTimeSetPoints(res);
}

void MainCalculation::calculateCompareTwo(int id1, int id2, const pair<QDateTime, QDateTime> &timeRange, bool isUser) {
    emit percentageLoaded(0.5);
    auto res1 = std::make_shared<vector<pair<QDateTime, int>>>(db->getMonthlyFromID(id1, timeRange, isUser));
    auto res2 = std::make_shared<vector<pair<QDateTime, int>>>(db->getMonthlyFromID(id2, timeRange, isUser));
    emit percentageLoaded(1);
    emit finishedCompareTwo(res1, res2, isUser);
}

void MainCalculation::calculateNewtonCache(int POIId1, int POIId2) {
    emit percentageLoadedExtra(0.5);
    pair<int, int> id{POIId1, POIId2};
    auto newtonPtr = std::make_shared<Newton>(id, std::move(db->getNewtonInput(POIId1, POIId2)));
    emit percentageLoadedExtra(1);
    emit finishedNewtonCache(newtonPtr);
}


