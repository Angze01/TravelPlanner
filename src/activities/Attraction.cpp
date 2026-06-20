#include "Attraction.h"
#include <sstream>

Attraction::Attraction(const std::string& name,
                       const std::string& time,
                       const std::string& note,
                       const std::string& ticketPrice)
    : Activity(name, time, note), ticketPrice(ticketPrice)
{}

std::string Attraction::getDetails() const {
    if (ticketPrice.empty()) return "";
    return "🎫 " + ticketPrice;
}

// 序列化格式：ACT|Attraction|name|time|note|completed|ticketPrice
std::string Attraction::serialize() const {
    std::ostringstream oss;
    oss << "ACT|Attraction|" << name << "|" << time << "|"
        << note << "|" << (isCompleted ? "1" : "0") << "|"
        << ticketPrice;
    return oss.str();
}
