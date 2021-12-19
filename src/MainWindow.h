#pragma once

#include "UsefulInclude.h"
#include "MainCalculation.h"
#include "spline.h"
#include "Database.h"
#include <QMainWindow>
#include <QtCharts>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    // Set up main window data and filter, then show main window
    void startMainWindow(const QString &dbname, shared_ptr<InspectData> inspectData);

    // Slot to update progress bar in tab 1 and tab 2
    void updateProgress(double percentage);
    void updateProgressExtra(double percentage);

    // Adjust POI selection based on user input
    void adjustPOISelection();

    // response to calculate top 10 of user or POi, thread will call show<...> when it's done
    void btnCalculateTop10(bool calculateUser);
    void showTop10(shared_ptr<vector<pair<int, int>>> res, bool calculateUser);

    // Same structure but for total user over time
    void btnCalculateTotalUserOverTimeSetPoints();
    void showTotalUserOverTimeSetPoints(shared_ptr<vector<pair<QDateTime, int>>> res);

    // Same structure but for two ID
    void btnCompareTwo(bool isUser);
    void showCompareTwo(shared_ptr<vector<pair<QDateTime, int>>> res1, shared_ptr<vector<pair<QDateTime, int>>> res2, bool isUser);

    // Extra: show a trajectory graph of a specific user, don't need multi thread because the workload is light
    void showTrajectoryGraph(int id);
    // Response to slider change in user input and update graph
    void updateTrajectoryGraph(int percentageOf100);

    // Generate newton cache
    void btnGenerateCubic();
    // Save the result from other thread and it's now ready to accept prediction
    void cacheCubicResult(pair<int, int> idBound, shared_ptr<vector<pair<int, int>>> coords);
    // Response to user input prediction
    void showPrediction();

signals:
    // Signals to other thread to do the calculation
    void calculateTop10(pair<int, int> id, bool isUser, const QDateTime&, const QDateTime&);
    void calculateTotalUserOverTimeSetPoints(const pair<int, int> &latitudeBound, const pair<int, int> &longitudeBound, const pair<QDateTime, QDateTime> &timeRange);
    void calculateCompareTwo(pair<int, int> id1Range, pair<int, int> id2Range, const pair<QDateTime, QDateTime> &timeRange, bool isUser);
    void calculateCubicCache(int POIId1, int POIId2);

public:
    MainCalculation *worker;

private:
    // Main data
    Ui::MainWindow *ui;
    shared_ptr<Database> db;

    // Calculation thread and state
    bool isProcessing = false;
    QThread thread;

    // Trajectory state
    vector<pair<QString, pair<int, int>>> userData;
    QScatterSeries* scatterPoint = nullptr;
    map<pair<int, int>, int> seriesIdx;

    // interpolation cache
    pair<int, int> cachePOIBound;
    shared_ptr<vector<pair<int, int>>> cubicCoords;
    shared_ptr<tk::spline> cubic;

    // Bound selection
    pair<int, int> latitudeBound{-90, 90};
    pair<int, int> longitudeBound{-180, 180};
};



