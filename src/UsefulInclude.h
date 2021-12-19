#pragma once

#include <memory>
#include <map>
#include <set>
#include <queue>
#include <ctime>
#include <vector>
#include <sstream>
#include <iostream>

using std::shared_ptr;
using std::pair;
using std::map;
using std::set;
using std::string;
using std::vector;
using std::cout;
using std::endl;

struct InspectData {
    bool userID;
    bool poiID;
    bool date;
    bool latitude;
    bool longitude;
};