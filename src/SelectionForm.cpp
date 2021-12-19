#include <QFileDialog>
#include "SelectionForm.h"
#include "ui_SelectionForm.h"
#include "LoadDataThread.h"

SelectionForm::SelectionForm(QWidget *parent): QDialog(parent), ui(new Ui::SelectionDialog) {
    ui->setupUi(this);

    // Set up load CSV thread
    auto worker = new LoadDataThread;
    worker->moveToThread(&loadCSVThread);
    connect(&loadCSVThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &SelectionForm::loadCSV, worker, &LoadDataThread::loadCSV);
    connect(worker, &LoadDataThread::doneLoadingCSV, this, &SelectionForm::doneLoadingCSV);
    connect(worker, &LoadDataThread::percentageLoaded, this, &SelectionForm::percentageLoaded);
    loadCSVThread.start();

    // Set up local connection
    connect(ui->btnSelectCSV, &QPushButton::clicked, this, &SelectionForm::browseCSV);
    connect(ui->btnFormBox, &QDialogButtonBox::accepted, this, &SelectionForm::loadMainWindow);
}

SelectionForm::~SelectionForm() {
    loadCSVThread.quit();
    loadCSVThread.wait();
    delete ui;
}

void SelectionForm::browseCSV() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open csv file"), QDir::currentPath(), tr("csv(*.csv)"));

    // Only load a new csv file if the file path is valid
    if (!filePath.isEmpty()) {
        if (isProcessingCsv) {
            ui->labelStatus->setText("You must wait for other CSV to finish processing");
            return;
        }
        // pass heavy loading job to a new thread
        ui->labelStatus->setText("Processing CSV file...");
        ui->labelCSVPath->setText(filePath);
        isProcessingCsv = true;
        emit loadCSV(filePath, ui->regenerateCache->isChecked());
    }
}

void SelectionForm::doneLoadingCSV(const QString &dbname) {
    dbName = dbname;
    isProcessingCsv = false;
    ui->progressBar->setValue(100);
    ui->btnFormBox->setDisabled(false);
    ui->labelStatus->setText("CSV loaded, please select filters and press OK to proceed");
}

void SelectionForm::percentageLoaded(double percentage) {
    ui->progressBar->setValue(percentage * 100);
}

void SelectionForm::loadMainWindow() {
    auto inspectDataPtr = std::make_shared<InspectData>();
    inspectDataPtr->userID = ui->checkUID->isChecked();
    inspectDataPtr->poiID = ui->checkLID->isChecked();
    inspectDataPtr->date = ui->checkTime->isChecked();
    inspectDataPtr->latitude = ui->checkLat->isChecked();
    inspectDataPtr->longitude = ui->checkLong->isChecked();
    emit startMainWindow(dbName, inspectDataPtr);
    close();
}
