#pragma once

#include "UsefulInclude.h"
#include "Database.h"
#include <QThread>

class LoadDataThread: public QThread {
Q_OBJECT
public slots:
    // Load csv file to sqlite and signal the dbname back to main thread
    void loadCSV(const QString &filePath, bool regenerateCache);

signals:
    // Update main thread ui progress bar
    void percentageLoaded(double percentage);

    // Work is done and ready to load main thread
    void doneLoadingCSV(const QString &dbname);
};



