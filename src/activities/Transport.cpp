#include "Transport.h"
#include <sstream>

Transport::Transport(const std::string& name,
                     const std::string& time,
                     const std::string& note,
                     const std::string& from,
                     const std::string& to,
                     const std::string& transportType,
                     double cost)
    : Activity(name, time, note, cost), from(from), to(to), transportType(transportType)
{}

std::string Transport::getDetails() const {
    std::string result = "";
    if (!transportType.empty()) result += transportType + " ";
    if (!from.empty() && !to.empty()) result += from + " → " + to;
    return result;
}

// 序列化格式：ACT|Transport|name|time|note|completed|from|to|transportType|cost
std::string Transport::serialize() const {
    std::ostringstream oss;
    oss << "ACT|Transport|" << name << "|" << time << "|"
        << note << "|" << (isCompleted ? "1" : "0") << "|"
        << from << "|" << to << "|" << transportType << "|" << cost;
    return oss.str();
}
