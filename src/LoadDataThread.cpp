#include "LoadDataThread.h"

void LoadDataThread::loadCSV(const QString &filePath, bool regenerateCache) {
    QString fileNameWithoutExt = filePath;
    fileNameWithoutExt.truncate(fileNameWithoutExt.size() - 4);
    Database db(fileNameWithoutExt);
    QFile f(filePath);

    // skip if (has data and not regenerating)
    if (f.open(QIODevice::ReadOnly) && (regenerateCache || !db.hasData())) {
        QTextStream stream(&f);
        QString line;
        const auto totalSize = f.size();

        // Create fresh table
        db.resetTable();
        db.startTransaction();
        do {
            // Process each csv line into SQLite table
            line = stream.readLine();
            if (line.size() <= 3) continue;  // skip corrupted field

            auto lst = line.split(QChar(','));
            db.insertRow(lst);

            // Calculate progress base on bytes remaining
            auto remainingProgress = (double) (totalSize - f.bytesAvailable()) / totalSize;
            emit percentageLoaded(remainingProgress);
        } while (!line.isNull());
        db.stopTransaction();
    }

    emit doneLoadingCSV(db.getDbName());
}
