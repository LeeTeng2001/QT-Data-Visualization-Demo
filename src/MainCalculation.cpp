#include "MainCalculation.h"

void MainCalculation::recordData(const QString &dbname, shared_ptr<InspectData> inspect) {
    db = std::make_shared<Database>(dbname);
}

void MainCalculation::calculateTop10(pair<int, int> idRange, bool isUser, const QDateTime& startDate, const QDateTime& endDate) {
    emit percentageLoaded(0.5);
    auto res = std::make_shared<vector<pair<int, int>>>(std::move(db->getTop10(idRange, isUser, startDate, endDate)));
    emit percentageLoaded(1);
    emit finishedTop10(res, isUser);
}

void MainCalculation::calculateTotalUserOverTimeSetPoints(const pair<int, int> &latitudeBound, const pair<int, int> &longitudeBound, const pair<QDateTime, QDateTime> &timeRange) {
    emit percentageLoaded(0.5);
    auto res = std::make_shared<vector<pair<QDateTime, int>>>(std::move(db->getUsersOverTimeInArea(latitudeBound, longitudeBound, timeRange)));
    emit percentageLoaded(1);
    emit finishedTotalUserOverTimeSetPoints(res);
}

void MainCalculation::calculateCompareTwo(pair<int, int> id1Range, pair<int, int> id2Range, const pair<QDateTime, QDateTime> &timeRange, bool isUser) {
    emit percentageLoaded(0.5);
    auto res1 = std::make_shared<vector<pair<QDateTime, int>>>(db->getMonthlyFromID(id1Range.first, id1Range.second, timeRange, isUser));
    auto res2 = std::make_shared<vector<pair<QDateTime, int>>>(db->getMonthlyFromID(id2Range.first, id2Range.second, timeRange, isUser));
    emit percentageLoaded(1);
    emit finishedCompareTwo(res1, res2, isUser);
}

void MainCalculation::calculateCubicCache(int POIId1, int POIId2) {
    emit percentageLoadedExtra(0.5);
    pair<int, int> id{POIId1, POIId2};
    auto coordinates = std::make_shared<vector<pair<int, int>>>(std::move(db->getInterpolateInput(POIId1, POIId2)));
    emit percentageLoadedExtra(1);
    emit finishedCubicCache(id, coordinates);
}


