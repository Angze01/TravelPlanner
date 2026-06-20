#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "Activity.h"
#include "UIHelper.h"

// ============================================================
//  Day — 單日行程
//  管理一天內的所有活動（含 Rule of 5 記憶體管理）
// ============================================================
class Day {
private:
    int         dayNumber;           // 第幾天（1-based）
    std::string date;                // 日期字串 "YYYY-MM-DD"
    std::vector<Activity*> activities; // 擁有所有 Activity 指標的生命週期

public:
    // ── 建構 / 解構 ──────────────────────────────────────────
    Day(int dayNumber, const std::string& date);
    ~Day();                               // 釋放所有 activities

    // 複製建構子（深度複製，利用 clone()）
    Day(const Day& other);
    Day& operator=(const Day& other);

    // 移動語意
    Day(Day&& other) noexcept;
    Day& operator=(Day&& other) noexcept;

    // ── 活動管理 ────────────────────────────────────────────
    void addActivity(Activity* act);          // 新增活動（接管所有權）
    bool removeActivity(int index);           // 以 1-based 索引刪除
    bool toggleActivity(int index);           // 切換完成狀態
    void sortByTime();                        // 依時間排序

    // ── 查詢 ───────────────────────────────────────────────
    int  getActivityCount()   const { return static_cast<int>(activities.size()); }
    int  getCompletedCount()  const;
    int  getUncompletedCount()const;

    std::vector<Activity*> getCompleted()   const;  // 取得已完成清單
    std::vector<Activity*> getUncompleted() const;  // 取得未完成清單

    const std::vector<Activity*>& getActivities() const { return activities; }
    Activity* getActivity(int index) const; // 1-based，失敗回 nullptr

    // ── Getter ───────────────────────────────────────────
    int         getDayNumber() const { return dayNumber; }
    std::string getDate()      const { return date; }
    void        setDate(const std::string& d) { date = d; }

    // ── 展示 ─────────────────────────────────────────────
    void display() const;       // 顯示整天活動列表
    void displayHeader() const; // 只顯示標題列

private:
    void clearActivities();          // 釋放所有指標
};
