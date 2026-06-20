#include "Day.h"
#include <algorithm>
#include <iostream>

// ============================================================
//  建構子
// ============================================================
Day::Day(int dayNumber, const std::string& date)
    : dayNumber(dayNumber), date(date)
{}

// ============================================================
//  解構子 — 釋放所有 Activity 指標
// ============================================================
Day::~Day() {
    clearActivities();
}

// ============================================================
//  複製建構子 — 深度複製（呼叫每個 Activity 的 clone()）
// ============================================================
Day::Day(const Day& other)
    : dayNumber(other.dayNumber), date(other.date)
{
    for (Activity* a : other.activities) {
        activities.push_back(a->clone());
    }
}

// ============================================================
//  複製指派運算子
// ============================================================
Day& Day::operator=(const Day& other) {
    if (this != &other) {
        clearActivities();
        dayNumber = other.dayNumber;
        date      = other.date;
        for (Activity* a : other.activities) {
            activities.push_back(a->clone());
        }
    }
    return *this;
}

// ============================================================
//  移動建構子
// ============================================================
Day::Day(Day&& other) noexcept
    : dayNumber(other.dayNumber), date(std::move(other.date)),
      activities(std::move(other.activities))
{
    other.activities.clear(); // 避免 other 解構時雙重釋放
}

// ============================================================
//  移動指派運算子
// ============================================================
Day& Day::operator=(Day&& other) noexcept {
    if (this != &other) {
        clearActivities();
        dayNumber  = other.dayNumber;
        date       = std::move(other.date);
        activities = std::move(other.activities);
        other.activities.clear();
    }
    return *this;
}

// ============================================================
//  私有：釋放所有 Activity 指標
// ============================================================
void Day::clearActivities() {
    for (Activity* a : activities) {
        delete a;
    }
    activities.clear();
}

// ============================================================
//  新增活動（接管所有權）
// ============================================================
void Day::addActivity(Activity* act) {
    activities.push_back(act);
    sortByTime(); // 新增後自動依時間排序（STL sort）
}

// ============================================================
//  依時間字串排序（使用 STL sort + lambda）
// ============================================================
void Day::sortByTime() {
    std::sort(activities.begin(), activities.end(),
              [](const Activity* a, const Activity* b) {
                  return a->getTime() < b->getTime();
              });
}

// ============================================================
//  刪除活動（1-based 索引）
// ============================================================
bool Day::removeActivity(int index) {
    if (index < 1 || index > static_cast<int>(activities.size())) return false;
    auto it = activities.begin() + (index - 1);
    delete *it;
    activities.erase(it);
    return true;
}

// ============================================================
//  切換活動完成狀態（1-based 索引）
// ============================================================
bool Day::toggleActivity(int index) {
    if (index < 1 || index > static_cast<int>(activities.size())) return false;
    activities[index - 1]->toggleCompleted();
    return true;
}

// ============================================================
//  查詢：已完成數
// ============================================================
int Day::getCompletedCount() const {
    // STL count_if
    return static_cast<int>(
        std::count_if(activities.begin(), activities.end(),
                      [](const Activity* a) { return a->getIsCompleted(); }));
}

// ============================================================
//  查詢：未完成數
// ============================================================
int Day::getUncompletedCount() const {
    return getActivityCount() - getCompletedCount();
}

// ============================================================
//  取得已完成活動清單（使用 STL copy_if）
// ============================================================
std::vector<Activity*> Day::getCompleted() const {
    std::vector<Activity*> result;
    std::copy_if(activities.begin(), activities.end(),
                 std::back_inserter(result),
                 [](const Activity* a) { return a->getIsCompleted(); });
    return result;
}

// ============================================================
//  取得未完成活動清單
// ============================================================
std::vector<Activity*> Day::getUncompleted() const {
    std::vector<Activity*> result;
    std::copy_if(activities.begin(), activities.end(),
                 std::back_inserter(result),
                 [](const Activity* a) { return !a->getIsCompleted(); });
    return result;
}

// ============================================================
//  以 1-based 索引取得活動指標
// ============================================================
Activity* Day::getActivity(int index) const {
    if (index < 1 || index > static_cast<int>(activities.size())) return nullptr;
    return activities[index - 1];
}

// ============================================================
//  顯示標題列
// ============================================================
void Day::displayHeader() const {
    std::string dateStr = date.empty() ? "" : "  |  " + date;
    std::cout << "\n" << Color::BRIGHT_CYAN << Color::BOLD
              << "  📅 第 " << dayNumber << " 天" << dateStr
              << Color::RESET << "\n";
    UIHelper::printSeparator("-", 54, Color::CYAN);
}

// ============================================================
//  顯示整天活動列表
// ============================================================
void Day::display() const {
    displayHeader();

    if (activities.empty()) {
        std::cout << Color::DIM << "  （此天尚無活動）\n" << Color::RESET;
    } else {
        for (int i = 0; i < static_cast<int>(activities.size()); i++) {
            activities[i]->display(i + 1);
        }
    }

    // 底部統計
    UIHelper::printSeparator("-", 54, Color::CYAN);
    int done  = getCompletedCount();
    int total = getActivityCount();
    std::string progressColor = (done == total && total > 0)
                                ? Color::BRIGHT_GREEN : Color::BRIGHT_YELLOW;
    std::cout << progressColor
              << "  ✦ 完成進度：" << done << " / " << total << " 項"
              << Color::RESET << "\n";
}
