#include "Newton.h"

#include <utility>

Newton::Newton(pair<int, int> id, vector<pair<int, int>> dt) : pointId(std::move(id)), data(std::move(dt)), diffTable(data.size(), vector<double>(data.size())) {
    for (int i = 0; i < data.size(); ++i) diffTable[i][0] = data[i].second;

    for (const auto &item : data)
        cout << item.first << ", ";
    cout << endl;
    for (const auto &item : data)
        cout << item.second << ", ";
    cout << endl;

    // calculate diff table for future prediction use
    for (int i = 1; i < data.size(); i++) {
        for (int j = 0; j < data.size() - i; j++)
            diffTable[j][i] = (double) (diffTable[j + 1][i - 1] - diffTable[j][i - 1]) / (data[j + i].first - data[j].first);
    }
}

double Newton::interpolate(double targetX) {
    // Plug in the formula and calculate f(x)
    double ansY = diffTable[0][0];
    double tmpFac = targetX - data[0].first;
    for (int i = 1; i < data.size(); ++i) {
        ansY += tmpFac * diffTable[0][i];
        tmpFac *= (targetX - data[i].first);
    }

    return ansY;
}

pair<int, int> Newton::getId() const {
    return pointId;
}

const vector<pair<int, int>> &Newton::getCoord() {
    return data;
}
