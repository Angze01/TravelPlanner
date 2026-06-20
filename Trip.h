#pragma once
#include <string>
#include <vector>
#include <map>
#include "Day.h"
#include "UIHelper.h"

// ============================================================
//  Trip — 整趟旅遊行程
//  包含多個 Day，提供統計與整體管理功能
// ============================================================
class Trip {
private:
    std::string       tripName;    // 行程名稱（e.g. 日本關東之旅）
    std::string       destination; // 目的地（e.g. 日本東京）
    int               totalDays;  // 共幾天
    std::vector<Day>  days;       // 每天的行程

public:
    // ── 建構子 ──────────────────────────────────────────────
    Trip(const std::string& tripName,
         const std::string& destination,
         int totalDays);

    // ── 天數管理 ────────────────────────────────────────────
    void initDays(const std::string& startDate = "");  // 初始化 totalDays 個 Day
    Day*  getDay(int dayNumber);                        // 1-based，失敗回 nullptr
    const Day* getDay(int dayNumber) const;

    // ── 活動新增（委派給對應 Day）──────────────────────────
    bool addActivityToDay(int dayNumber, Activity* act);
    bool removeActivityFromDay(int dayNumber, int actIndex);
    bool toggleActivityInDay(int dayNumber, int actIndex);

    // ── 統計（使用 STL map）─────────────────────────────────
    int getTotalActivityCount()   const;
    int getTotalCompletedCount()  const;
    int getTotalUncompletedCount()const;
    // 回傳各活動類型完成數量統計 map<類型, 完成數>
    std::map<std::string, int> getCompletionStatsByType() const;

    // ── Getter / Setter ─────────────────────────────────────
    std::string getTripName()    const { return tripName; }
    std::string getDestination() const { return destination; }
    int         getTotalDays()   const { return totalDays; }
    std::vector<Day>& getDays()        { return days; }
    const std::vector<Day>& getDays()  const { return days; }

    void setTripName   (const std::string& n) { tripName    = n; }
    void setDestination(const std::string& d) { destination = d; }

    // ── 展示 ────────────────────────────────────────────────
    void displaySummary()  const;   // 總覽（含統計）
    void displayAllDays()  const;   // 顯示所有天的活動
};
