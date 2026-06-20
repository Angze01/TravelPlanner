#include "Trip.h"
#include "DateUtils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

// ============================================================
//  建構子
// ============================================================
Trip::Trip(const std::string& tripName,
           const std::string& destination,
           int totalDays,
           double budget)
    : tripName(tripName), destination(destination), totalDays(totalDays), budget(budget)
{}

// ============================================================
//  初始化每天的 Day 物件
//  startDate: "YYYY-MM-DD"，空字串則使用 "第N天" 格式
// ============================================================
void Trip::initDays(const std::string& startDate) {
    days.clear();
    for (int i = 1; i <= totalDays; i++) {
        std::string dateStr = "";
        if (!startDate.empty() && DateUtils::isValidDate(startDate)) {
            // 第 1 天 = startDate，第 2 天 = startDate + 1 天，以此類推
            dateStr = DateUtils::addDays(startDate, i - 1);
        }
        days.emplace_back(i, dateStr);
    }
}

// ============================================================
//  取得指定天的 Day 指標（1-based）
// ============================================================
Day* Trip::getDay(int dayNumber) {
    if (dayNumber < 1 || dayNumber > static_cast<int>(days.size())) return nullptr;
    return &days[dayNumber - 1];
}

const Day* Trip::getDay(int dayNumber) const {
    if (dayNumber < 1 || dayNumber > static_cast<int>(days.size())) return nullptr;
    return &days[dayNumber - 1];
}

// ============================================================
//  對指定天新增活動
// ============================================================
bool Trip::addActivityToDay(int dayNumber, Activity* act) {
    Day* day = getDay(dayNumber);
    if (!day) { delete act; return false; }
    day->addActivity(act);
    return true;
}

// ============================================================
//  對指定天刪除活動
// ============================================================
bool Trip::removeActivityFromDay(int dayNumber, int actIndex) {
    Day* day = getDay(dayNumber);
    if (!day) return false;
    return day->removeActivity(actIndex);
}

// ============================================================
//  切換指定天的活動完成狀態
// ============================================================
bool Trip::toggleActivityInDay(int dayNumber, int actIndex) {
    Day* day = getDay(dayNumber);
    if (!day) return false;
    return day->toggleActivity(actIndex);
}

// ============================================================
//  統計：總活動數
// ============================================================
int Trip::getTotalActivityCount() const {
    int total = 0;
    for (const Day& d : days) total += d.getActivityCount();
    return total;
}

// ============================================================
//  統計：總完成數
// ============================================================
int Trip::getTotalCompletedCount() const {
    int total = 0;
    for (const Day& d : days) total += d.getCompletedCount();
    return total;
}

// ============================================================
//  統計：總未完成數
// ============================================================
int Trip::getTotalUncompletedCount() const {
    return getTotalActivityCount() - getTotalCompletedCount();
}

// ============================================================
//  各類別完成數量統計（使用 STL map）
// ============================================================
std::map<std::string, int> Trip::getCompletionStatsByType() const {
    std::map<std::string, int> stats;
    for (const Day& day : days) {
        for (const Activity* act : day.getActivities()) {
            if (act->getIsCompleted()) {
                stats[act->getTypeLabel()]++;
            }
        }
    }
    return stats;
}

// ============================================================
//  v2.0 費用統計
// ============================================================
double Trip::getTotalCost() const {
    double total = 0.0;
    for (const Day& d : days) total += d.getTotalCost();
    return total;
}

double Trip::getRemainingBudget() const {
    return budget - getTotalCost();
}

std::map<std::string, double> Trip::getCostStatsByType() const {
    std::map<std::string, double> stats;
    for (const Day& day : days) {
        for (const Activity* act : day.getActivities()) {
            if (act->getCost() > 0.0) {
                stats[act->getTypeLabel()] += act->getCost();
            }
        }
    }
    return stats;
}

// ============================================================
//  V4: 搜尋行程活動
// ============================================================
std::vector<std::pair<int, Activity*>> Trip::searchActivities(const std::string& keyword) const {
    std::vector<std::pair<int, Activity*>> results;
    if (keyword.empty()) return results;

    // 將關鍵字轉為小寫
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

    for (const Day& day : days) {
        for (Activity* act : day.getActivities()) {
            std::string name = act->getName();
            std::string note = act->getNote();
            // 轉為小寫比對
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::transform(note.begin(), note.end(), note.begin(), ::tolower);

            if (name.find(lowerKeyword) != std::string::npos || 
                note.find(lowerKeyword) != std::string::npos) {
                results.push_back({day.getDayNumber(), act});
            }
        }
    }
    return results;
}

// ============================================================
//  顯示行程總覽
// ============================================================
void Trip::displaySummary() const {
    std::cout << "\n" << Color::BRIGHT_CYAN << Color::BOLD;
    std::cout << "  +==================================================+\n";
    std::cout << "  | Trip: " << std::left << std::setw(43) << tripName << "|\n";
    std::cout << "  | Dest: " << std::left << std::setw(43) << destination << "|\n";
    std::cout << "  +==================================================+\n";
    std::cout << Color::RESET;

    int total    = getTotalActivityCount();
    int done     = getTotalCompletedCount();
    int undone   = getTotalUncompletedCount();
    int numDays  = static_cast<int>(days.size());

    UIHelper::printStat("行程天數", std::to_string(numDays) + " 天", Color::BRIGHT_CYAN);
    UIHelper::printStat("總活動數", std::to_string(total) + " 項", Color::BRIGHT_WHITE);
    UIHelper::printStat("✅ 已完成", std::to_string(done) + " 項", Color::BRIGHT_GREEN);
    UIHelper::printStat("⬜ 待辦中", std::to_string(undone) + " 項", Color::BRIGHT_YELLOW);

    // 各類別統計（STL map 迭代）
    auto stats = getCompletionStatsByType();
    if (!stats.empty()) {
        std::cout << "\n";
        UIHelper::printStat("已完成分類統計", "");
        for (const auto& [label, count] : stats) {
            std::cout << Color::DIM << "     " << label << ": "
                      << Color::BRIGHT_GREEN << count << " 項"
                      << Color::RESET << "\n";
        }
    }

    // 預算與花費顯示
    if (budget > 0.0) {
        std::cout << "\n";
        UIHelper::printStat("總預算", "$" + std::to_string(static_cast<int>(budget)), Color::BRIGHT_MAGENTA);
        UIHelper::printStat("已花費", "$" + std::to_string(static_cast<int>(getTotalCost())), Color::BRIGHT_YELLOW);
        double remain = getRemainingBudget();
        std::string remainColor = (remain >= 0) ? Color::BRIGHT_GREEN : Color::BRIGHT_RED;
        UIHelper::printStat("剩餘預算", "$" + std::to_string(static_cast<int>(remain)), remainColor);
        
        // 各類別花費統計
        auto costStats = getCostStatsByType();
        if (!costStats.empty()) {
            std::cout << "\n";
            UIHelper::printStat("各類別花費", "");
            for (const auto& [label, cost] : costStats) {
                std::cout << Color::DIM << "     " << label << ": "
                          << Color::BRIGHT_YELLOW << "$" << static_cast<int>(cost)
                          << Color::RESET << "\n";
            }
        }
    }

    // 每天進度條
    std::cout << "\n";
    UIHelper::printSeparator("-", 54, Color::CYAN);
    for (const Day& d : days) {
        int dc   = d.getCompletedCount();
        int dtot = d.getActivityCount();
        std::string bar = "";
        int barLen = 20;
        int filled = (dtot > 0) ? (dc * barLen / dtot) : 0;
        for (int i = 0; i < filled; i++)  bar += "#";
        for (int i = filled; i < barLen; i++) bar += ".";

        std::cout << Color::DIM << "  Day " << std::setw(2) << d.getDayNumber()
                  << "  "
                  << Color::RESET;
        std::cout << Color::BRIGHT_GREEN << "[" << bar << "]" << Color::RESET;
        std::cout << Color::DIM << "  " << dc << "/" << dtot
                  << Color::RESET << "\n";
    }
    UIHelper::printSeparator("-", 54, Color::CYAN);
}

// ============================================================
//  顯示所有天的活動
// ============================================================
void Trip::displayAllDays() const {
    for (const Day& d : days) {
        d.display();
        std::cout << "\n";
    }
}
