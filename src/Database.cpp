#include "Database.h"

// To track and assign new connection name for each thread that uses the same db
int Database::connectionCount = 1;

// Create database from given dbname, will create missing table if necessary
Database::Database(QString dbname) : dbName(std::move(dbname)) {
    db = QSqlDatabase::addDatabase("QSQLITE", "db" + QString::number(connectionCount++));
    db.setDatabaseName(dbName + ".sqlite");
    if (!db.open()) qDebug() << "Can't Connect to database: " + db.databaseName();
    else qDebug() << "Connected to: " + db.databaseName();

    // Create table for new database
    QSqlQuery qry(db);
    qry.prepare("CREATE TABLE IF NOT EXISTS poi\n"
                "(\n"
                "    PID       INTEGER PRIMARY KEY,\n"
                "    latitude  DOUBLE NOT NULL,\n"
                "    longitude DOUBLE NOT NULL\n"
                ");");
    if (!qry.exec()) qDebug() << "Error creating poi table";
    qry.prepare( "CREATE TABLE IF NOT EXISTS user_entry\n"
                 "(\n"
                 "    id INTEGER PRIMARY KEY,\n"
                 "    UID INTEGER NOT NULL,\n"
                 "    date DATETIME NOT NULL,\n"
                 "    poi INTEGER NOT NULL\n"
                 ");");
    if (!qry.exec()) qDebug() << "Error creating user table";

    // Get total entries in the table
    qry.prepare("SELECT COUNT(*) FROM user_entry;");
    if (!qry.exec()) qDebug() << "Error querying total data from table";
    while (qry.next()) {
        auto size = qry.value(0).toInt();
        qDebug() << "Loaded user_entry table Size: " << size;
    }
}

// close db connection
Database::~Database() {
    db.close();
}

// database name without suffix, like 'gowalla'
QString Database::getDbName() const {
    return dbName;
}

// return total rows != 0
bool Database::hasData() {
    QSqlQuery qry(db);
    qry.prepare("SELECT COUNT(*) FROM user_entry;");
    if (!qry.exec()) qDebug() << "Error querying total data from table";
    while (qry.next()) {
        auto size = qry.value(0).toInt();
        return size != 0;
    }
    return false;
}

// Regenerate all tables from the db
void Database::resetTable() {
    QSqlQuery qry(db);
    qry.prepare("DROP TABLE IF EXISTS poi;");
    if (!qry.exec()) qDebug() << "Error dropping poi table";
    qry.prepare("DROP TABLE IF EXISTS user_entry;");
    if (!qry.exec()) qDebug() << "Error dropping user_entry table";
    qry.prepare("CREATE TABLE IF NOT EXISTS poi\n"
                "(\n"
                "    PID       INTEGER PRIMARY KEY,\n"
                "    latitude  DOUBLE NOT NULL,\n"
                "    longitude DOUBLE NOT NULL\n"
                ");");
    if (!qry.exec()) qDebug() << "Error creating poi table";
    qry.prepare( "CREATE TABLE IF NOT EXISTS user_entry\n"
                 "(\n"
                 "    id INTEGER PRIMARY KEY,\n"
                 "    UID INTEGER NOT NULL,\n"
                 "    date DATETIME NOT NULL,\n"
                 "    poi INTEGER NOT NULL\n"
                 ");");
    if (!qry.exec()) qDebug() << "Error creating user table";
}

// Insert single row from a list of csv row
void Database::insertRow(const QList<QString> &lst) {
    QSqlQuery qry(db);
    qry.prepare("INSERT OR REPLACE INTO poi (PID, latitude, longitude) VALUES (?, ?, ?);");
    qry.addBindValue(lst[1].toInt());
    qry.addBindValue(lst[3]);
    qry.addBindValue(lst[4]);
    if (!qry.exec()) { qDebug() << "Error inserting value into poi"; return; }
    qry.prepare("INSERT INTO user_entry (UID, date, poi) VALUES (?, ?, ?);");
    qry.addBindValue(lst[0].toInt());
    qry.addBindValue(lst[2]);
    qry.addBindValue(lst[1].toInt());
    if (!qry.exec()) { qDebug() << "Error inserting value into user_entry"; return; }
}

// Start db transaction
void Database::startTransaction() {
    QSqlQuery qry(db);
    if (qry.exec("BEGIN TRANSACTION;"))
        qDebug() << "Start transaction";
    else qDebug() << "Failed to start transaction";
}

// Stop db transaction and commit
void Database::stopTransaction() {
    QSqlQuery qry(db);
    if (qry.exec("COMMIT;"))
        qDebug() << "End and commit transaction";
    else qDebug() << "Failed to end and commit transaction";
}

// get max ID from user or POI id
int Database::getMaxID(bool isUser) {
    QSqlQuery qry(db);
    if (isUser) qry.prepare("SELECT max(user_entry.UID) FROM user_entry;");
    else qry.prepare("SELECT max(poi.PID) FROM poi;");

    if (!qry.exec()) qDebug() << "Error querying max ID from table";
    while (qry.next()) {
        auto size = qry.value(0).toInt();
        return size;
    }
    return 0;
}

// Get date limit range for user to select
pair<QDate, QDate> Database::getDateRange() {
    pair<QDate, QDate> result;
    QSqlQuery qry(db);

    qry.prepare("SELECT min(user_entry.date) FROM user_entry;");
    if (!qry.exec()) qDebug() << "Error querying min Date from table";
    while (qry.next())
        result.first = qry.value(0).toDate();
    qry.prepare("SELECT max(user_entry.date) FROM user_entry;");
    if (!qry.exec()) qDebug() << "Error querying max Date from table";
    while (qry.next())
        result.second = qry.value(0).toDate();

    return result;
}

// Get all poi in a vector of (latitude, longitude), since it's 0 indexed we don't need to query index
vector<pair<double, double>> Database::getAllPOI() {
    vector<pair<double, double>> result;
    QSqlQuery qry(db);

    qry.prepare("SELECT latitude, longitude FROM poi;");
    if (!qry.exec()) qDebug() << "Error selecting all latitude and longitude from table";
    while (qry.next()) {
        result.emplace_back(qry.value(0).toDouble(), qry.value(1).toDouble());
    }

    return result;
}

// get top 10 between date, can query user top 10 or poi top 10
vector<pair<int, int>> Database::getTop10(int id, bool isUser, const QDateTime& startDate, const QDateTime& endDate) {
    vector<pair<int, int>> result;
    QSqlQuery qry(db);
    if (isUser) {
        qry.prepare("SELECT poi, COUNT(*) as `num` FROM user_entry\n"
                    "WHERE UID == ? AND date BETWEEN ? AND ?\n"
                    "GROUP BY poi\n"
                    "ORDER BY num DESC\n"
                    "LIMIT 10;");
    }
    else {
        qry.prepare("SELECT UID, COUNT(*) as `num` FROM user_entry\n"
                    "WHERE poi == ? AND date BETWEEN ? AND ?\n"
                    "GROUP BY UID\n"
                    "ORDER BY num DESC\n"
                    "LIMIT 10;");
    }
    qry.addBindValue(id);
    qry.addBindValue(startDate);
    qry.addBindValue(endDate);

    if (!qry.exec()) qDebug() << "Error getting top 10 from table";
    while (qry.next()) {
        result.emplace_back(qry.value(0).toInt(), qry.value(1).toInt());
    }

    return result;
}

// get total user in a GPS area and time frame
vector<pair<QDateTime, int>> Database::getUsersOverTimeInArea(const pair<int, int> &latitudeBound, const pair<int, int> &longitudeBound, const pair<QDateTime, QDateTime> &timeRange) {
    vector<pair<QDateTime, int>> result;
    QSqlQuery qry(db);

    qry.prepare("SELECT strftime('%Y-%m', date) as `year-month`, COUNT(*) as `num` FROM user_entry\n"
                "INNER JOIN poi ON poi.PID==user_entry.poi\n"
                "WHERE latitude BETWEEN ? AND ? AND\n"
                "      longitude BETWEEN ? AND ? AND\n"
                "      date BETWEEN ? AND ?\n"
                "GROUP BY \"year-month\";");
    qry.addBindValue(latitudeBound.first);
    qry.addBindValue(latitudeBound.second);
    qry.addBindValue(longitudeBound.first);
    qry.addBindValue(longitudeBound.second);
    qry.addBindValue(timeRange.first);
    qry.addBindValue(timeRange.second);

    if (!qry.exec()) qDebug() << "Error getting user over time in area from table";
    while (qry.next()) {
        result.emplace_back(QDateTime::fromString(qry.value(0).toString(), "yyyy-MM"), qry.value(1).toInt());
    }

    return result;
}

// Get monthly data from a poi id or user id in a time frame
vector<pair<QDateTime, int>> Database::getMonthlyFromID(int id, const pair<QDateTime, QDateTime> &timeRange, bool isUser) {
    vector<pair<QDateTime, int>> result;
    QSqlQuery qry(db);

    if (isUser) {
        qry.prepare("SELECT strftime('%Y-%m', date) as `year-month`, COUNT(*) as `num` FROM user_entry\n"
                    "INNER JOIN poi ON poi.PID==user_entry.poi\n"
                    "WHERE UID == ? AND\n"
                    "      date BETWEEN ? AND ?\n"
                    "GROUP BY \"year-month\";");
    }
    else {
        qry.prepare("SELECT strftime('%Y-%m', date) as `year-month`, COUNT(*) as `num` FROM user_entry\n"
                    "INNER JOIN poi ON poi.PID==user_entry.poi\n"
                    "WHERE poi == ? AND\n"
                    "      date BETWEEN ? AND ?\n"
                    "GROUP BY \"year-month\";");
    }
    qry.addBindValue(id);
    qry.addBindValue(timeRange.first);
    qry.addBindValue(timeRange.second);

    if (!qry.exec()) qDebug() << "Error getting single id over time from table";
    while (qry.next()) {
        result.emplace_back(QDateTime::fromString(qry.value(0).toString(), "yyyy-MM"), qry.value(1).toInt());
    }

    return result;
}

// get user data from id in a form of (DateString, (latitude(int), longitude(int)))
vector<pair<QString, pair<int, int>>> Database::getUserDataSimplified(int id) {
    vector<pair<QString, pair<int, int>>> result;
    QSqlQuery qry(db);

    qry.prepare("SELECT date, cast(latitude as INTEGER), cast(longitude as INTEGER) FROM user_entry\n"
                "INNER JOIN poi ON poi.PID==user_entry.poi\n"
                "WHERE UID == ?;");
    qry.addBindValue(id);

    if (!qry.exec()) qDebug() << "Error getting user's simplified data from table";
    while (qry.next()) {
        result.push_back({qry.value(0).toString(), {qry.value(1).toInt(), qry.value(2).toInt()}});
    }

    return result;
}

// get daily total user over time of a poi ID and return result in a newton input form, that is ((2010-01-01 - queryDate).toDays, count)
// You can feed the result directly into newton algorithm
vector<pair<int, int>> Database::getNewtonInput(int poiID1, int poiID2) {
    vector<pair<int, int>> result;
    QSqlQuery qry(db);

    qry.prepare("SELECT strftime('%Y-%m-%d', date) as `year-month-day`, COUNT(*) as `num`\n"
                "FROM user_entry\n"
                "INNER JOIN poi ON poi.PID==user_entry.poi\n"
                "WHERE poi.PID BETWEEN ? AND ?\n"
                "GROUP BY \"year-month-day\";");
    qry.addBindValue(poiID1);
    qry.addBindValue(poiID2);

    auto originDate = QDateTime::fromString("2010-01-01", "yyyy-MM-dd");
    if (!qry.exec()) qDebug() << "Error getting newton input from table";
    while (qry.next()) {
        result.emplace_back(originDate.daysTo(QDateTime::fromString(qry.value(0).toString(), "yyyy-MM-dd")),
                            qry.value(1).toInt());
    }

    return result;
}
