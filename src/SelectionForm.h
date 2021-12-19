#pragma once

#include "UsefulInclude.h"
#include "Database.h"
#include <QThread>
#include <QDialog>

namespace Ui {
    class SelectionDialog;
}

class SelectionForm : public QDialog {
Q_OBJECT
public:
    explicit SelectionForm(QWidget *parent = nullptr);
    ~SelectionForm() override;

public slots:
    // Select CSV and load it if valid
    void browseCSV();

    // Finished loading CSV
    void doneLoadingCSV(const QString &dbname);

    // Update percentage loaded in ui
    void percentageLoaded(double percentage);

    // Load main window and close form
    void loadMainWindow();

signals:
    // Load csv in another thread
    void loadCSV(const QString &, bool);

    // Signal main window to start and load database + load selected inspection field
    void startMainWindow(const QString &, shared_ptr<InspectData>);

private:
    Ui::SelectionDialog *ui;

    // State
    bool isProcessingCsv = false;
    QThread loadCSVThread;
    QString dbName;
};



