#include "Attraction.h"
#include <sstream>

Attraction::Attraction(const std::string& name,
                       const std::string& time,
                       const std::string& note,
                       const std::string& ticketPrice,
                       double cost)
    : Activity(name, time, note, cost), ticketPrice(ticketPrice)
{}

std::string Attraction::getDetails() const {
    if (ticketPrice.empty()) return "";
    return "🎫 " + ticketPrice;
}

// 序列化格式：ACT|Attraction|name|time|note|completed|ticketPrice|cost
std::string Attraction::serialize() const {
    std::ostringstream oss;
    oss << "ACT|Attraction|" << name << "|" << time << "|"
        << note << "|" << (isCompleted ? "1" : "0") << "|"
        << ticketPrice << "|" << cost;
    return oss.str();
}
