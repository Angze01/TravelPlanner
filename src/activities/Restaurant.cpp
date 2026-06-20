#include "Restaurant.h"
#include <sstream>

Restaurant::Restaurant(const std::string& name,
                       const std::string& time,
                       const std::string& note,
                       const std::string& cuisine,
                       double cost)
    : Activity(name, time, note, cost), cuisine(cuisine)
{}

std::string Restaurant::getDetails() const {
    if (cuisine.empty()) return "";
    return "🍜 " + cuisine;
}

// 序列化格式：ACT|Restaurant|name|time|note|completed|cuisine|cost
std::string Restaurant::serialize() const {
    std::ostringstream oss;
    oss << "ACT|Restaurant|" << name << "|" << time << "|"
        << note << "|" << (isCompleted ? "1" : "0") << "|"
        << cuisine << "|" << cost;
    return oss.str();
}
