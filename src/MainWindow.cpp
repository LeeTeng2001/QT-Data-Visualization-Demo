#include "MainWindow.h"
#include "MainCalculation.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Set up thread calculation connection
    worker = new MainCalculation;
    worker->moveToThread(&thread);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &MainCalculation::percentageLoaded, this, &MainWindow::updateProgress);
    connect(worker, &MainCalculation::percentageLoadedExtra, this, &MainWindow::updateProgressExtra);
    connect(this, &MainWindow::calculateTop10, worker, &MainCalculation::calculateTop10);
    connect(worker, &MainCalculation::finishedTop10, this, &MainWindow::showTop10);
    connect(this, &MainWindow::calculateTotalUserOverTimeSetPoints, worker, &MainCalculation::calculateTotalUserOverTimeSetPoints);
    connect(worker, &MainCalculation::finishedTotalUserOverTimeSetPoints, this, &MainWindow::showTotalUserOverTimeSetPoints);
    connect(this, &MainWindow::calculateCompareTwo, worker, &MainCalculation::calculateCompareTwo);
    connect(worker, &MainCalculation::finishedCompareTwo, this, &MainWindow::showCompareTwo);
    connect(this, &MainWindow::calculateCubicCache, worker, &MainCalculation::calculateCubicCache);
    connect(worker, &MainCalculation::finishedCubicCache, this, &MainWindow::cacheCubicResult);
    thread.start();

    // Other local connection
    connect(ui->btnShowTopPOIFromUser, &QPushButton::clicked, [this](){ btnCalculateTop10(true);});
    connect(ui->btnShowTopUserFromPOI, &QPushButton::clicked, [this](){ btnCalculateTop10(false);});
    connect(ui->btnCompareUsers, &QPushButton::clicked, [this](){ btnCompareTwo(true);});
    connect(ui->btnComparePOI, &QPushButton::clicked, [this](){ btnCompareTwo(false);});
    connect(ui->btnShowTrajectory, &QPushButton::clicked, [this](){ showTrajectoryGraph(ui->spinTrajectoryUID->value());});
    connect(ui->btnGeneratePOIOvertime, &QPushButton::clicked, this, &MainWindow::btnCalculateTotalUserOverTimeSetPoints);
    connect(ui->btnSetLatLongBound, &QPushButton::clicked, [this](){
        latitudeBound = {ui->latitudeBoundXMin->value(), ui->latitudeBoundXMax->value()};
        longitudeBound = {ui->longitudeBoundXMin->value(), ui->longitudeBoundXMax->value()};
        adjustPOISelection();
    });
    connect(ui->traSlider, &QAbstractSlider::valueChanged, this, &MainWindow::updateTrajectoryGraph);
    connect(ui->btnGenerateCubic, &QPushButton::clicked, this, &MainWindow::btnGenerateCubic);
    connect(ui->btnShowPrediction, &QPushButton::clicked, this, &MainWindow::showPrediction);
}

MainWindow::~MainWindow() {
    thread.quit();
    thread.wait();
    delete ui;
}

void MainWindow::startMainWindow(const QString &dbname, shared_ptr<InspectData> inspectData) {
    // Do main window set up here
    db = std::make_shared<Database>(dbname);

    auto maxUID = db->getMaxID(true);
    auto maxPID = db->getMaxID(false);
    auto dateRange = db->getDateRange();

    // UID and PID must start with 0, Set bound -----------------------
    ui->spinBoxUIDSetLow->setRange(0, maxUID);
    ui->spinBoxUIDSetHigh->setRange(0, maxUID);
    ui->spinCompareUID11->setRange(0, maxUID);
    ui->spinCompareUID12->setRange(0, maxUID);
    ui->spinCompareUID21->setRange(0, maxUID);
    ui->spinCompareUID22->setRange(0, maxUID);
    ui->spinTrajectoryUID->setRange(0, maxUID);
    ui->spinBoxPOISetLow->setRange(0, maxPID);
    ui->spinBoxPOISetHigh->setRange(0, maxPID);
    ui->spinComparePOI11->setRange(0, maxPID);
    ui->spinComparePOI12->setRange(0, maxPID);
    ui->spinComparePOI21->setRange(0, maxPID);
    ui->spinComparePOI22->setRange(0, maxPID);
    ui->spinPredictPOI1->setRange(0, maxPID);
    ui->spinPredictPOI2->setRange(0, maxPID);
    adjustPOISelection();

    // Set up date selection
    ui->dateTimeEditStart->setDateRange(dateRange.first,dateRange.second);
    ui->dateTimeEditEnd->setDateRange(dateRange.first,dateRange.second);
    ui->dateTimeEditStart->setDate(dateRange.first);
    ui->dateTimeEditEnd->setDate(dateRange.second);

    // Hide unselected AKA filter -----------------------------------
    if (!inspectData->userID) {
        ui->groupBoxUser1->hide();
        ui->groupBoxUID1->hide();
        ui->tabWidget_3->setTabEnabled(0, false);
    }
    if (!inspectData->poiID) {
        ui->groupBoxPOI1->hide();
        ui->groupBoxPOI2->hide();
        ui->tabWidget_3->setTabEnabled(1, false);
        if (!inspectData->userID) {
            ui->tabWidget_2->setTabEnabled(0, false);
            ui->tabWidget_2->setTabEnabled(2, false);
        }
    }
    if (!inspectData->latitude) {
        ui->labelLatitude1->hide();
        ui->latitudeBoundXMin->hide();
        ui->latitudeBoundXMax->hide();
    }
    if (!inspectData->longitude) {
        ui->labelLongitude1->hide();
        ui->longitudeBoundXMin->hide();
        ui->longitudeBoundXMax->hide();
    }
    if (!inspectData->latitude && !inspectData->longitude)
        ui->btnSetLatLongBound->hide();
    if (!inspectData->date) {
        ui->dateTimeEditStart->hide();
        ui->dateTimeEditEnd->hide();
        ui->labelStartDate->hide();
        ui->labelEndDate->hide();
    }

    ui->statusbar->showMessage("Done setting up!");
    show();
}

void MainWindow::updateProgress(double percentage) {
    ui->progressBar->setValue(percentage * 100);
}

void MainWindow::updateProgressExtra(double percentage) {
    ui->progressBar_2->setValue(percentage * 100);
}

void MainWindow::btnCalculateTop10(bool calculateUser) {
    if (isProcessing)  {
        ui->statusbar->showMessage("Another calculation is going!");
        return;
    }
    isProcessing = true;
    ui->statusbar->showMessage("Calculating...");

    if (calculateUser)
        emit calculateTop10({ui->spinBoxUIDSetLow->value(), ui->spinBoxUIDSetHigh->value()}, true, ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime());
    else
        emit calculateTop10({ui->spinBoxPOISetLow->value(), ui->spinBoxPOISetHigh->value()}, false, ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime());
}

void MainWindow::showTop10(shared_ptr<vector<pair<int, int>>> res, bool calculateUser) {
    isProcessing = false;
    ui->statusbar->showMessage("Task Finished!");

    // Display
    auto series = new QBarSeries;
    auto set = new QBarSet("POI");
    auto chart = new QChart;
    QStringList axisXList;
    auto axisX = new QBarCategoryAxis;
    auto axisY = new QValueAxis;

    // Set up
    if (calculateUser) chart->setTitle("Top 10 POI of user with ID " + QString::number(ui->spinBoxUIDSetLow->value()) + " to " + QString::number(ui->spinBoxUIDSetHigh->value()));
    else chart->setTitle("Top 10 user of POI with ID " + QString::number(ui->spinBoxPOISetLow->value()) + " to " + QString::number(ui->spinBoxPOISetHigh->value()));
    chart->addSeries(series);
    chart->legend()->setVisible(false);
    series->append(set);

    // Add result
    for (const auto &item : *res) {
        *set << item.second;
        axisXList << QString::number(item.first);
    }

    // set up axis
    if (calculateUser) axisX->setTitleText("Point Of Interest ID");
    else axisX->setTitleText("User ID");
    axisX->append(axisXList);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY->setTitleText("Frequency");
    axisY->setRange(0, res->front().second + 1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setAnimationOptions(QChart::SeriesAnimations);

    ui->singleChartView->setChart(chart);
    ui->singleChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::adjustPOISelection() {
    auto seriesSelected = new QScatterSeries();
    seriesSelected->setName("Selected");
    seriesSelected->setColor(QColor::fromRgb(0, 255, 0));
    seriesSelected->setMarkerSize(15.0);
    auto seriesNSelected = new QScatterSeries();
    seriesNSelected->setName("Not Selected");
    seriesNSelected->setColor(QColor::fromRgb(255, 0, 0));
    seriesNSelected->setMarkerSize(15.0);

    auto allPOI = db->getAllPOI();
    std::set<pair<double, double>> lookupLoc;
    for (auto &row : allPOI) {
        pair<double, double> newPos = {
                (double)((int)(row.first * 1)) / 1,
                (double)((int)(row.second * 1)) / 1
        };
        if (lookupLoc.find(newPos) != lookupLoc.end()) continue;

        lookupLoc.insert(newPos);
        if (newPos.first > latitudeBound.first && newPos.first < latitudeBound.second &&
            newPos.second > longitudeBound.first && newPos.second < longitudeBound.second) {
            // std::cout << "Selected: " << newPos.first << " " << newPos.second << "\n";
            *seriesSelected << QPointF(newPos.first, newPos.second);
        }
        else {
            // std::cout << "Not Selected: " << newPos.first << " " << newPos.second << "\n";
            *seriesNSelected << QPointF(newPos.first, newPos.second);
        }
    }

    ui->poiSelectView->setRenderHint(QPainter::Antialiasing);
    ui->poiSelectView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->poiSelectView->setInteractive(true);
    auto chart = new QChart;
    chart->addSeries(seriesSelected);
    chart->addSeries(seriesNSelected);
    chart->setTitle("Location in latitude and longitude");
    chart->setDropShadowEnabled(false);

    auto axisX = new QValueAxis;
    auto axisY = new QValueAxis;
    axisX->setTitleText("Latitude");
    axisX->setRange(-90, 90);
    chart->addAxis(axisX, Qt::AlignBottom);
    seriesSelected->attachAxis(axisX);
    seriesNSelected->attachAxis(axisX);

    axisY->setTitleText("Longitude");
    axisY->setRange(-180, 180);
    chart->addAxis(axisY, Qt::AlignLeft);
    seriesSelected->attachAxis(axisY);
    seriesNSelected->attachAxis(axisY);

    ui->poiSelectView->setChart(chart);
}

void MainWindow::btnCalculateTotalUserOverTimeSetPoints() {
    if (isProcessing)  {
        ui->statusbar->showMessage("Another calculation is going!");
        return;
    }
    isProcessing = true;
    ui->statusbar->showMessage("Calculating...");

    emit calculateTotalUserOverTimeSetPoints(latitudeBound, longitudeBound,
                                             {ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime()});
}

void MainWindow::showTotalUserOverTimeSetPoints(shared_ptr<vector<pair<QDateTime, int>>> res) {
    isProcessing = false;
    ui->statusbar->showMessage("Task Finished!");

    QLineSeries *series = new QSplineSeries;
    auto chart = new QChart;

    for (const auto &item : (*res)) {
        series->append(item.first.toMSecsSinceEpoch() + 432000000, item.second);  // to MSec + some offset
    }

    chart->addSeries(series);
    chart->legend()->hide();
    chart->setTitle("Total users in POI bound over time");

    auto axisX = new QDateTimeAxis;
    axisX->setTickCount(res->size());
    axisX->setFormat("MMM yy");
    axisX->setTitleText("Date");
    axisX->setLabelsAngle(-90);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QValueAxis;
    axisY->setTitleText("Total User");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->totalUserOTView->setChart(chart);
    ui->totalUserOTView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::btnCompareTwo(bool isUser) {
    if (isProcessing)  {
        ui->statusbar->showMessage("Another calculation is going!");
        return;
    }
    isProcessing = true;
    ui->statusbar->showMessage("Calculating...");

    if (isUser)
        emit calculateCompareTwo({ui->spinCompareUID11->value(), ui->spinCompareUID12->value()}, {ui->spinCompareUID21->value(), ui->spinCompareUID22->value()},
                                 {ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime()}, true);
    else
        emit calculateCompareTwo({ui->spinComparePOI11->value(), ui->spinComparePOI12->value()}, {ui->spinComparePOI21->value(), ui->spinComparePOI22->value()},
                                 {ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime()}, false);
}

void MainWindow::showCompareTwo(shared_ptr<vector<pair<QDateTime, int>>> res1, shared_ptr<vector<pair<QDateTime, int>>> res2, bool isUser) {
    isProcessing = false;
    ui->statusbar->showMessage("Task Finished!");

    // Display
    auto series1 = new QSplineSeries;
    auto series2 = new QSplineSeries;
    auto chart = new QChart;
    auto axisX = new QDateTimeAxis;
    auto axisY = new QValueAxis;

    // Add result
    int maxBound = 0;
    for (const auto &item : *res1) {
        maxBound = std::max(maxBound, item.second);
        series1->append(item.first.toMSecsSinceEpoch(), item.second);
    }
    for (const auto &item : *res2) {
        maxBound = std::max(maxBound, item.second);
        series2->append(item.first.toMSecsSinceEpoch(), item.second);
    }

    // Set up
    if (isUser) {
        chart->setTitle("Total visited POI overtime for 2 users range");
        series1->setName("User ID: " + QString::number(ui->spinCompareUID11->value()) + " to " + QString::number(ui->spinCompareUID12->value()));
        series2->setName("User ID: " + QString::number(ui->spinCompareUID21->value()) + " to " + QString::number(ui->spinCompareUID22->value()));
    }
    else {
        chart->setTitle("Total users overtime for 2 POI range");
        series1->setName("POI ID: " + QString::number(ui->spinComparePOI11->value()) + " to " + QString::number(ui->spinComparePOI12->value()));
        series2->setName("POI ID: " + QString::number(ui->spinComparePOI21->value()) + " to " + QString::number(ui->spinComparePOI22->value()));
    }
    chart->legend()->setVisible(true);
    chart->addSeries(series1);
    chart->addSeries(series2);

    // set up axis
    axisX->setTitleText("Date");
    axisX->setFormat("MMM yy");
    axisX->setLabelsAngle(-90);
    chart->addAxis(axisX, Qt::AlignBottom);
    series1->attachAxis(axisX);
    series2->attachAxis(axisX);

    if (isUser) axisY->setTitleText("POI Counts");
    else axisY->setTitleText("User Counts");
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setMax(maxBound + 1);
    series1->attachAxis(axisY);
    series2->attachAxis(axisY);

    chart->setAnimationOptions(QChart::SeriesAnimations);

    ui->compareTwoChartView->setChart(chart);
    ui->compareTwoChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::showTrajectoryGraph(int id) {
    // Init set up
    ui->traSlider->setEnabled(true);
    userData = std::move(db->getUserDataSimplified(id));
    seriesIdx.clear();

    auto series = new QLineSeries();
    scatterPoint = new QScatterSeries();
    series->setName("Selected");
    scatterPoint->setColor(QColor::fromRgb(200, 0, 0));
    scatterPoint->setMarkerSize(15.0);

    // Calculate bound and record lookup table for updateTrajectoryGraph
    pair<int, int> horizontalRange{90, -90};
    pair<int, int> verticalRange{180, -180};
    int idxAt = 0;
    for (const auto &item : userData) {
        auto &geoPos = item.second;
        horizontalRange = {std::min(horizontalRange.first, geoPos.first), std::max(horizontalRange.second, geoPos.first)};
        verticalRange = {std::min(verticalRange.first, geoPos.second), std::max(verticalRange.second, geoPos.second)};

        series->append(geoPos.first, geoPos.second);
        if (seriesIdx.find(geoPos) == seriesIdx.end()) {  // new point
            seriesIdx[geoPos] = idxAt++;
            scatterPoint->append(geoPos.first, geoPos.second);
        }
    }
    // Extend the bound slightly
    verticalRange = {verticalRange.first - 2, verticalRange.second + 2};
    horizontalRange = {horizontalRange.first - 1, horizontalRange.second + 1};

    ui->trajectoryGraphView->setRenderHint(QPainter::Antialiasing);
    ui->trajectoryGraphView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->trajectoryGraphView->setInteractive(true);

    auto chart = new QChart;
    chart->addSeries(series);
    chart->addSeries(scatterPoint);
    chart->setTitle("Trajectory of user " + QString::number(id) + " in latitude and longitude");
    chart->setDropShadowEnabled(false);

    auto axisX = new QValueAxis;
    auto axisY = new QValueAxis;
    axisX->setTitleText("Latitude");
    axisX->setRange(horizontalRange.first, horizontalRange.second);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    scatterPoint->attachAxis(axisX);

    axisY->setTitleText("Longitude");
    axisY->setRange(verticalRange.first, verticalRange.second);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    scatterPoint->attachAxis(axisY);

    ui->trajectoryGraphView->setChart(chart);
    ui->traSlider->setValue(0);
    updateTrajectoryGraph(0);
}

void MainWindow::updateTrajectoryGraph(int percentageOf100) {
    // get idx of point
    int idxOfData = ((double) percentageOf100 / 100 * (userData.size() - 1));
    int idxOfSeries = seriesIdx[userData[idxOfData].second];

    // Update scale of points
    for (int idx= 0; idx < scatterPoint->count(); idx++){
        QPointF p =  ui->trajectoryGraphView->chart()->mapToPosition(scatterPoint->at(idx) , scatterPoint);
        QGraphicsItem *it = ui->trajectoryGraphView->itemAt(ui->trajectoryGraphView->mapFromScene(p));
        it->setTransformOriginPoint(it->boundingRect().center());
        it->setScale(idx == idxOfSeries ? 2 : 1);
    }

    ui->traLatitude->setText(QString::number(userData[idxOfData].second.first));
    ui->traLongitude->setText(QString::number(userData[idxOfData].second.second));
    ui->traTimeStamp->setText(userData[idxOfData].first);
}

void MainWindow::btnGenerateCubic() {
    pair<int, int> curPOIBound{ ui->spinPredictPOI1->value(),  ui->spinPredictPOI2->value()};
    if (cubicCoords != nullptr && cachePOIBound == curPOIBound) {
        ui->statusbar->showMessage("Newton cache is already generated");
        return;
    }
    if (isProcessing)  {
        ui->statusbar->showMessage("Another calculation is going!");
        return;
    }

    isProcessing = true;
    ui->statusbar->showMessage("Calculating...");

    emit calculateCubicCache(ui->spinPredictPOI1->value(), ui->spinPredictPOI2->value());
}

void MainWindow::cacheCubicResult(pair<int, int> idBound, shared_ptr<vector<pair<int, int>>> coords) {
    isProcessing = false;
    ui->statusbar->showMessage("Task finished.");
    vector<double> X, Y;
    for (const auto &item : (*coords)) {
        X.emplace_back(item.first);
        Y.emplace_back(item.second);
    }
    cubic = std::make_shared<tk::spline>(X, Y);
    cubicCoords = std::move(coords);
    cachePOIBound = idBound;
    ui->labelNewtonCache->setText("Left: " + QString::number(idBound.first) + ", Right: " + QString::number(idBound.second));
}

void MainWindow::showPrediction() {
    if (cubic == nullptr) {
        ui->statusbar->showMessage("Please generate cubic cache first");
        return;
    }

    // Get result
    auto originDate = QDateTime::fromString("2010-01-01", "yyyy-MM-dd");
    auto predictX = originDate.daysTo(ui->predictDate->dateTime());
    auto result = (*cubic)(predictX);

    // Update graph
    auto series = new QLineSeries;
    auto targetDot = new QScatterSeries;
    series->setName("Original Data");
    targetDot->setName("Prediction");
    targetDot->setColor(QColor::fromRgb(200, 0, 0));
    targetDot->setMarkerSize(20.0);

    pair<int, int> horizontalRange{predictX, predictX};
    int verticalMax = result;
    int totalSize = (*cubicCoords).size();
    for (int i = 0; i < totalSize; i++) {
        horizontalRange = {std::min(horizontalRange.first, (*cubicCoords)[i].first), std::max(horizontalRange.second, (*cubicCoords)[i].first)};
        verticalMax = std::max(verticalMax, (*cubicCoords)[i].second);
        series->append((*cubicCoords)[i].first, (*cubicCoords)[i].second);
    }
    verticalMax += 1;
    // horizontalRange = {horizontalRange.first - 1, horizontalRange.second + 1};
    targetDot->append(predictX, result);

    ui->predictionGraphView->setRenderHint(QPainter::Antialiasing);
    ui->predictionGraphView->setRubberBand(QChartView::HorizontalRubberBand);
    ui->predictionGraphView->setInteractive(true);

    auto chart = new QChart;
    chart->addSeries(series);
    chart->addSeries(targetDot);
    chart->setTitle("Total users overtime of a POI");
    chart->setDropShadowEnabled(false);

    auto axisX = new QValueAxis;
    auto axisY = new QValueAxis;
    axisX->setTitleText("Normalized time");
    axisX->setRange(horizontalRange.first, horizontalRange.second);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    targetDot->attachAxis(axisX);

    axisY->setTitleText("Total Users");
    axisY->setRange(0, verticalMax);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    targetDot->attachAxis(axisY);

    ui->predictResultLabel->setText("Your input date after normalizing is: " + QString::number(predictX) + ", result is: " + QString::number(result));
    ui->predictionGraphView->setChart(chart);
}


