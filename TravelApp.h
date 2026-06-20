#pragma once
#include <string>
#include <memory>
#include "Trip.h"
#include "UIHelper.h"

// ============================================================
//  TravelApp — 主程式 UI 控制器
//  負責所有終端機選單的顯示與使用者互動
// ============================================================
class TravelApp {
private:
    Trip*       currentTrip;  // 目前載入的旅程（nullptr 表示無）
    bool        running;      // 主迴圈控制旗標
    std::string currentFile;  // 目前檔案路徑（空 = 未儲存）

    // ── 主選單 ─────────────────────────────────────────────
    void menuMain();
    void menuCreateTrip();
    void menuLoadTrip();
    void menuTripSummary();
    void menuManageDays();
    void menuSaveTrip();
    void menuSaveTripAs();

    // ── 單日管理 ────────────────────────────────────────────
    void menuManageDay(int dayNumber);

    // ── 活動操作 ────────────────────────────────────────────
    void addActivityToDay(int dayNumber);
    void removeActivityFromDay(int dayNumber);
    void toggleActivityInDay(int dayNumber);

    // ── 活動建立輔助 ────────────────────────────────────────
    Activity* createAttraction();
    Activity* createRestaurant();
    Activity* createHotel();
    Activity* createTransport();

    // ── 顯示輔助 ────────────────────────────────────────────
    void displayHeader();          // 頂部 banner + 目前旅程資訊
    void displayDayList();         // 所有天的清單（含倒數）
    std::string getDayStatusStr(const Day& day) const;  // 取得天的狀態字串

    // ── 工具 ────────────────────────────────────────────────
    bool ensureTripLoaded();       // 確認有旅程載入，否則提示
    void clearTrip();              // 釋放 currentTrip 記憶體

public:
    TravelApp();
    ~TravelApp();
    void run();  // 啟動主迴圈
};
