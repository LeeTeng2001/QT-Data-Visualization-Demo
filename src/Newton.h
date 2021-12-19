#pragma once

#include "UsefulInclude.h"
#include "spline.h"

class Newton {
private:
    vector<pair<int, int>> data;  // Coordinate, a vector of (x, y), y is frequency, x is (date - 2010-1-1) to days
    vector<vector<double>> diffTable;  // newton diff table
    pair<int, int> pointId;  // which POI id cache is this newton object holding

public:
    // Initialize newton algorithm with POI id and coordinates
    Newton(pair<int, int> id, vector<pair<int, int>> data);

    // Interpolate X from cached diff table
    double interpolate(double targetX);

    // Return cache's POI id
    [[nodiscard]] pair<int, int> getId() const;

    // Return unmodifiable coordinates
    const vector<pair<int, int>> &getCoord();
};


