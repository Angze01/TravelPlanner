#include "Hotel.h"
#include <sstream>

Hotel::Hotel(const std::string& name,
             const std::string& time,
             const std::string& note,
             const std::string& checkIn,
             const std::string& checkOut,
             double cost)
    : Activity(name, time, note, cost), checkIn(checkIn), checkOut(checkOut)
{}

std::string Hotel::getDetails() const {
    std::string result = "";
    if (!checkIn.empty())  result += "入住:" + checkIn;
    if (!checkOut.empty()) result += "  退房:" + checkOut;
    return result;
}

// 序列化格式：ACT|Hotel|name|time|note|completed|checkIn|checkOut|cost
std::string Hotel::serialize() const {
    std::ostringstream oss;
    oss << "ACT|Hotel|" << name << "|" << time << "|"
        << note << "|" << (isCompleted ? "1" : "0") << "|"
        << checkIn << "|" << checkOut << "|" << cost;
    return oss.str();
}
