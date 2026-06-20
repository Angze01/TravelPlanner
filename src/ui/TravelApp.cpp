#include "TravelApp.h"
#include "FileManager.h"
#include "DateUtils.h"
#include "Attraction.h"
#include "Restaurant.h"
#include "Hotel.h"
#include "Transport.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

// ============================================================
//  建構子 / 解構子
// ============================================================
TravelApp::TravelApp() : currentTrip(nullptr), running(false) {}

TravelApp::~TravelApp() {
    clearTrip();
}

void TravelApp::clearTrip() {
    delete currentTrip;
    currentTrip = nullptr;
    currentFile = "";
}

// ============================================================
//  啟動主迴圈
// ============================================================
void TravelApp::run() {
    UIHelper::setupConsole();
    running = true;
    while (running) {
        menuMain();
    }
    std::cout << Color::BRIGHT_CYAN << "\n  掰掰！旅途愉快 ✈\n\n" << Color::RESET;
}

// ============================================================
//  顯示頂部 Banner + 目前旅程資訊
// ============================================================
void TravelApp::displayHeader() {
    UIHelper::clearScreen();
    UIHelper::printBanner();

    if (currentTrip) {
        std::cout << Color::BRIGHT_WHITE << "  旅程：" << Color::BRIGHT_CYAN
                  << currentTrip->getTripName()
                  << Color::DIM << "  📍 " << currentTrip->getDestination()
                  << "  ✈ " << currentTrip->getTotalDays() << " 天\n"
                  << Color::RESET;
    } else {
        std::cout << Color::DIM << "  （尚未載入旅程）\n" << Color::RESET;
    }
    UIHelper::printSeparator("-", 54, Color::CYAN);
}

// ============================================================
//  主選單
// ============================================================
void TravelApp::menuMain() {
    displayHeader();

    std::vector<std::string> options = {
        "建立新旅程",
        "載入旅程",
        "查看行程總覽",
        "管理每日活動",
        "搜尋行程活動",
        "儲存旅程",
        "離開"
    };

    int choice = UIHelper::selectMenu(options, "主選單");

    if      (choice == 0) menuCreateTrip();
    else if (choice == 1) menuLoadTrip();
    else if (choice == 2) { if (ensureTripLoaded()) menuTripSummary(); }
    else if (choice == 3) { if (ensureTripLoaded()) menuManageDays(); }
    else if (choice == 4) { if (ensureTripLoaded()) menuSearch(); }
    else if (choice == 5) { if (ensureTripLoaded()) menuSaveTrip(); }
    else if (choice == 6 || choice == -1) running = false;
}

// ============================================================
//  建立新旅程
// ============================================================
void TravelApp::menuCreateTrip() {
    displayHeader();
    UIHelper::printSection("建立新旅程");

    std::string name = UIHelper::getInput("旅程名稱（例：日本關東之旅）");
    if (name.empty()) { UIHelper::printError("名稱不能為空"); UIHelper::pressEnterToContinue(); return; }

    std::string dest = UIHelper::getInput("目的地（例：日本東京）");
    if (dest.empty()) { UIHelper::printError("目的地不能為空"); UIHelper::pressEnterToContinue(); return; }

    int days = UIHelper::getIntInput("幾天（例：7）", 1, 365);
    if (days < 1) { UIHelper::printError("天數無效"); UIHelper::pressEnterToContinue(); return; }

    double budget = 0.0;
    std::string budgetStr = UIHelper::getInput("整趟旅程總預算（留空或輸入 0 則不設定）");
    if (!budgetStr.empty()) {
        try { budget = std::stod(budgetStr); } catch(...) { budget = 0.0; }
    }

    // 詢問出發日期
    std::string startDate = "";
    while (true) {
        startDate = UIHelper::getInput("出發日期 YYYY-MM-DD（留空則不設定）");
        if (startDate.empty() || DateUtils::isValidDate(startDate)) break;
        UIHelper::printError("日期格式錯誤，請輸入 YYYY-MM-DD（例：2026-07-01）");
    }

    clearTrip();
    currentTrip = new Trip(name, dest, days, budget);
    currentTrip->initDays(startDate);

    UIHelper::printSuccess("旅程「" + name + "」已建立！共 " + std::to_string(days) + " 天");
    if (!startDate.empty()) {
        int countdown = DateUtils::daysUntil(startDate);
        if      (countdown > 0) UIHelper::printInfo("距離出發還有 " + std::to_string(countdown) + " 天！");
        else if (countdown == 0) UIHelper::printSuccess("今天就是出發日！出發嘍～✈");
        else UIHelper::printInfo("旅程已於 " + startDate + " 開始");
    }
    UIHelper::pressEnterToContinue();
}

// ============================================================
//  載入旅程
// ============================================================
void TravelApp::menuLoadTrip() {
    displayHeader();
    UIHelper::printSection("載入旅程");

    auto files = FileManager::listSaveFiles("saves");
    if (files.empty()) {
        UIHelper::printWarning("saves 目錄中沒有找到任何 .travel 存檔");
        UIHelper::printInfo("請先建立並儲存一個旅程");
        UIHelper::pressEnterToContinue();
        return;
    }

    std::vector<std::string> options;
    for (int i = 0; i < static_cast<int>(files.size()); i++) {
        // 從路徑取出檔名
        std::string fname = files[i];
        auto pos = fname.find_last_of("\\/");
        if (pos != std::string::npos) fname = fname.substr(pos + 1);
        options.push_back(fname);
    }
    options.push_back("手動輸入路徑");
    options.push_back("取消");

    int choice = UIHelper::selectMenu(options, "選擇存檔");
    
    std::string filename = "";
    if (choice == static_cast<int>(files.size())) { // 手動輸入路徑
        filename = UIHelper::getInput("輸入檔案路徑");
    } else if (choice == static_cast<int>(files.size()) + 1 || choice == -1) { // 取消
        return;
    } else if (choice >= 0 && choice < static_cast<int>(files.size())) {
        filename = files[choice];
    } else {
        return;
    }

    Trip* loaded = FileManager::load(filename);
    if (!loaded) {
        UIHelper::printError("載入失敗：找不到檔案或格式錯誤");
        UIHelper::pressEnterToContinue();
        return;
    }

    clearTrip();
    currentTrip = loaded;
    currentFile = filename;
    UIHelper::printSuccess("成功載入：" + currentTrip->getTripName());
    UIHelper::pressEnterToContinue();
}

// ============================================================
//  行程總覽
// ============================================================
void TravelApp::menuTripSummary() {
    UIHelper::clearScreen();
    currentTrip->displaySummary();
    std::cout << "\n";

    // 顯示倒數資訊
    for (const Day& d : currentTrip->getDays()) {
        if (!d.getDate().empty()) {
            int dd = DateUtils::daysUntil(d.getDate());
            if (dd == 0) {
                UIHelper::printSuccess("今天是旅程第 " + std::to_string(d.getDayNumber()) + " 天！✈ 好好玩！");
                break;
            } else if (dd > 0) {
                UIHelper::printInfo("下一天（第 " + std::to_string(d.getDayNumber()) +
                                    " 天）還有 " + std::to_string(dd) + " 天出發");
                break;
            }
        }
    }

    UIHelper::pressEnterToContinue();
}

// ============================================================
//  取得天的狀態字串（含顏色）
// ============================================================
std::string TravelApp::getDayStatusStr(const Day& day) const {
    if (day.getDate().empty()) return "";
    return DateUtils::getStatusColor(day.getDate()) +
           DateUtils::getStatusLabel(day.getDate()) +
           Color::RESET;
}

// ============================================================
//  顯示所有天的清單（含倒數狀態）
// ============================================================
void TravelApp::displayDayList() {
    std::cout << "\n";
    for (const Day& d : currentTrip->getDays()) {
        int done  = d.getCompletedCount();
        int total = d.getActivityCount();
        std::string progress = std::to_string(done) + "/" + std::to_string(total);
        std::string pColor   = (done == total && total > 0) ? Color::BRIGHT_GREEN : Color::BRIGHT_YELLOW;

        std::cout << "  " << Color::BRIGHT_CYAN << Color::BOLD
                  << "[" << std::setw(2) << d.getDayNumber() << "]"
                  << Color::RESET
                  << "  第 " << d.getDayNumber() << " 天";

        if (!d.getDate().empty()) {
            std::cout << Color::DIM << "  " << d.getDate() << Color::RESET;
        }

        std::cout << "  " << pColor << "✦ " << progress << " 完成" << Color::RESET;
        std::cout << "  " << getDayStatusStr(d) << "\n";
    }
    std::cout << "\n";
}

// ============================================================
//  管理每日活動（天數選擇）
// ============================================================
void TravelApp::menuManageDays() {
    while (true) {
        displayHeader();
        UIHelper::printSection("管理每日活動");
        displayDayList();

        std::vector<std::string> options;
        for (int i = 1; i <= currentTrip->getTotalDays(); i++) {
            options.push_back("Day " + std::to_string(i));
        }
        options.push_back("返回主選單");

        int choice = UIHelper::selectMenu(options, "選擇要管理的天數");
        if (choice == currentTrip->getTotalDays() || choice == -1) return;

        menuManageDay(choice + 1);
    }
}

// ============================================================
//  管理特定天的活動
// ============================================================
void TravelApp::menuManageDay(int dayNumber) {
    while (true) {
        UIHelper::clearScreen();
        Day* day = currentTrip->getDay(dayNumber);
        if (!day) return;

        // 標題
        std::cout << "\n" << Color::BRIGHT_CYAN << Color::BOLD
                  << "  📅 第 " << dayNumber << " 天";
        if (!day->getDate().empty()) {
            std::cout << "  " << day->getDate()
                      << "  " << getDayStatusStr(*day);
        }
        std::cout << Color::RESET << "\n";
        UIHelper::printSeparator("-", 54, Color::CYAN);

        // 活動列表
        day->display();

        // 操作選單
        std::vector<std::string> options = {
            "新增活動",
            "刪除活動",
            "切換完成狀態",
            "分類過濾活動",
            "返回"
        };

        int choice = UIHelper::selectMenu(options, "請選擇操作");

        if      (choice == 0) addActivityToDay(dayNumber);
        else if (choice == 1) removeActivityFromDay(dayNumber);
        else if (choice == 2) toggleActivityInDay(dayNumber);
        else if (choice == 3) filterActivitiesInDay(dayNumber);
        else if (choice == 4 || choice == -1) return;
    }
}

// ============================================================
//  新增活動（選擇類型後分派）
// ============================================================
void TravelApp::addActivityToDay(int dayNumber) {
    UIHelper::printSection("新增活動");
    std::vector<std::string> options = {
        "🏛  景點 (Attraction)",
        "🍽  餐廳 (Restaurant)",
        "🏨  住宿 (Hotel)",
        "🚌  交通 (Transport)",
        "取消"
    };

    int type = UIHelper::selectMenu(options, "選擇活動類型");
    if (type == 4 || type == -1) return;

    Activity* act = nullptr;
    switch (type) {
        case 0: act = createAttraction(); break;
        case 1: act = createRestaurant(); break;
        case 2: act = createHotel();      break;
        case 3: act = createTransport();  break;
        default: return;
    }

    if (act) {
        // V4: 檢查時間衝突
        std::string newTime = act->getTime();
        if (!newTime.empty()) {
            Day* d = currentTrip->getDay(dayNumber);
            int conflictCount = 0;
            for (Activity* existing : d->getActivities()) {
                if (existing->getTime() == newTime) {
                    conflictCount++;
                }
            }
            if (conflictCount > 0) {
                UIHelper::printWarning("注意：目前有 " + std::to_string(conflictCount) + " 個其他活動也是在 " + newTime + "！請確認時間是否衝突。");
            }
        }

        currentTrip->addActivityToDay(dayNumber, act);
        UIHelper::printSuccess("活動「" + act->getName() + "」已新增！");
        UIHelper::pressEnterToContinue();
    }
}

// ============================================================
//  V4: 活動過濾器
// ============================================================
void TravelApp::filterActivitiesInDay(int dayNumber) {
    Day* day = currentTrip->getDay(dayNumber);
    if (!day || day->getActivityCount() == 0) {
        UIHelper::printWarning("此天沒有活動可過濾");
        UIHelper::pressEnterToContinue();
        return;
    }

    std::vector<std::string> options = {
        "🏛  景點 (Attraction)",
        "🍽  餐廳 (Restaurant)",
        "🏨  住宿 (Hotel)",
        "🚌  交通 (Transport)",
        "返回"
    };

    int type = UIHelper::selectMenu(options, "選擇要檢視的活動類型");
    if (type == 4 || type == -1) return;

    std::vector<Activity*> filtered;
    const auto& acts = day->getActivities();
    
    // 使用 std::copy_if 和 dynamic_cast 進行過濾
    std::copy_if(acts.begin(), acts.end(), std::back_inserter(filtered),
                 [type](const Activity* a) {
                     if (type == 0) return dynamic_cast<const Attraction*>(a) != nullptr;
                     if (type == 1) return dynamic_cast<const Restaurant*>(a) != nullptr;
                     if (type == 2) return dynamic_cast<const Hotel*>(a) != nullptr;
                     if (type == 3) return dynamic_cast<const Transport*>(a) != nullptr;
                     return false;
                 });

    UIHelper::clearScreen();
    std::cout << "\n  " << Color::BRIGHT_CYAN << "🔍 過濾結果：" << options[type] << Color::RESET << "\n";
    UIHelper::printSeparator();

    if (filtered.empty()) {
        std::cout << Color::DIM << "  （找不到符合條件的活動）\n" << Color::RESET;
    } else {
        for (int i = 0; i < static_cast<int>(filtered.size()); i++) {
            filtered[i]->display(i + 1);
        }
    }

    UIHelper::pressEnterToContinue();
}

// ============================================================
//  V4: 全域搜尋行程
// ============================================================
void TravelApp::menuSearch() {
    displayHeader();
    UIHelper::printSection("搜尋行程活動");

    std::string keyword = UIHelper::getInput("請輸入要搜尋的關鍵字（名稱或備註）");
    if (keyword.empty()) return;

    auto results = currentTrip->searchActivities(keyword);

    if (results.empty()) {
        UIHelper::printWarning("找不到任何包含「" + keyword + "」的活動");
    } else {
        std::cout << "\n  🔍 搜尋結果（共 " << results.size() << " 筆）：\n";
        UIHelper::printSeparator();
        for (const auto& pair : results) {
            int dayNum = pair.first;
            Activity* act = pair.second;
            std::cout << "  [" << Color::BRIGHT_YELLOW << "Day " << dayNum << Color::RESET << "] "
                      << act->getTime() << " " << act->getName() 
                      << " (" << act->getTypeLabel() << ")\n";
        }
    }
    UIHelper::pressEnterToContinue();
}

// ============================================================
//  建立景點
// ============================================================
Activity* TravelApp::createAttraction() {
    UIHelper::printSection("新增景點");
    std::string name = UIHelper::getInput("景點名稱");
    if (name.empty()) return nullptr;
    std::string time = UIHelper::getInput("時間 HH:MM");
    std::string ticket = UIHelper::getInput("門票費用（字串，留空略過）");
    std::string note = UIHelper::getInput("備注（留空略過）");
    
    double cost = 0.0;
    std::string costStr = UIHelper::getInput("實際花費數字（例：1500，留空為 0）");
    if (!costStr.empty()) { try { cost = std::stod(costStr); } catch(...) {} }
    
    return new Attraction(name, time, note, ticket, cost);
}

// ============================================================
//  建立餐廳
// ============================================================
Activity* TravelApp::createRestaurant() {
    UIHelper::printSection("新增餐廳");
    std::string name = UIHelper::getInput("餐廳名稱");
    if (name.empty()) return nullptr;
    std::string time = UIHelper::getInput("時間 HH:MM");
    std::string cuisine = UIHelper::getInput("料理類型（例：壽司、拉麵）");
    std::string note = UIHelper::getInput("備注（留空略過）");

    double cost = 0.0;
    std::string costStr = UIHelper::getInput("實際花費數字（例：3000，留空為 0）");
    if (!costStr.empty()) { try { cost = std::stod(costStr); } catch(...) {} }

    return new Restaurant(name, time, note, cuisine, cost);
}

// ============================================================
//  建立住宿
// ============================================================
Activity* TravelApp::createHotel() {
    UIHelper::printSection("新增住宿");
    std::string name = UIHelper::getInput("住宿名稱");
    if (name.empty()) return nullptr;
    std::string time = UIHelper::getInput("時間 HH:MM（例：15:00）");
    std::string checkIn  = UIHelper::getInput("入住時間（例：15:00）");
    std::string checkOut = UIHelper::getInput("退房時間（例：11:00）");
    std::string note = UIHelper::getInput("備注（留空略過）");

    double cost = 0.0;
    std::string costStr = UIHelper::getInput("實際花費數字（例：8000，留空為 0）");
    if (!costStr.empty()) { try { cost = std::stod(costStr); } catch(...) {} }

    return new Hotel(name, time, note, checkIn, checkOut, cost);
}

// ============================================================
//  建立交通
// ============================================================
Activity* TravelApp::createTransport() {
    UIHelper::printSection("新增交通");
    std::string name = UIHelper::getInput("交通名稱（例：成田機場→新宿）");
    if (name.empty()) return nullptr;
    std::string time = UIHelper::getInput("時間 HH:MM");
    std::string from = UIHelper::getInput("出發地");
    std::string to   = UIHelper::getInput("目的地");
    std::string type = UIHelper::getInput("交通方式（飛機/新幹線/巴士/地鐵）");
    std::string note = UIHelper::getInput("備注（留空略過）");

    double cost = 0.0;
    std::string costStr = UIHelper::getInput("實際花費數字（例：2500，留空為 0）");
    if (!costStr.empty()) { try { cost = std::stod(costStr); } catch(...) {} }

    return new Transport(name, time, note, from, to, type, cost);
}

// ============================================================
//  刪除活動
// ============================================================
void TravelApp::removeActivityFromDay(int dayNumber) {
    Day* day = currentTrip->getDay(dayNumber);
    if (!day || day->getActivityCount() == 0) {
        UIHelper::printWarning("此天沒有活動可刪除");
        UIHelper::pressEnterToContinue();
        return;
    }
    std::vector<std::string> options;
    for (int i = 1; i <= day->getActivityCount(); i++) {
        Activity* act = day->getActivity(i);
        options.push_back(act->getName());
    }
    options.push_back("取消");

    int idx = UIHelper::selectMenu(options, "選擇要刪除的活動");
    if (idx == day->getActivityCount() || idx == -1) return;

    Activity* act = day->getActivity(idx + 1);
    if (!act) return;

    std::string actName = act->getName();
    if (currentTrip->removeActivityFromDay(dayNumber, idx + 1)) {
        UIHelper::printSuccess("已刪除活動：" + actName);
    } else {
        UIHelper::printError("刪除失敗");
    }
    UIHelper::pressEnterToContinue();
}

// ============================================================
//  切換活動完成狀態
// ============================================================
void TravelApp::toggleActivityInDay(int dayNumber) {
    Day* day = currentTrip->getDay(dayNumber);
    if (!day || day->getActivityCount() == 0) {
        UIHelper::printWarning("此天沒有活動");
        UIHelper::pressEnterToContinue();
        return;
    }
    std::vector<std::string> options;
    for (int i = 1; i <= day->getActivityCount(); i++) {
        Activity* act = day->getActivity(i);
        std::string status = act->getIsCompleted() ? "✅ " : "⬜ ";
        options.push_back(status + act->getName());
    }
    options.push_back("取消");

    int idx = UIHelper::selectMenu(options, "選擇要切換狀態的活動");
    if (idx == day->getActivityCount() || idx == -1) return;

    if (currentTrip->toggleActivityInDay(dayNumber, idx + 1)) {
        Activity* act = day->getActivity(idx + 1);
        if (act) {
            std::string status = act->getIsCompleted() ? "✅ 已完成" : "⬜ 待辦中";
            UIHelper::printSuccess("「" + act->getName() + "」→ " + status);
        }
    } else {
        UIHelper::printError("切換失敗");
    }
    UIHelper::pressEnterToContinue();
}

// ============================================================
//  儲存旅程
// ============================================================
void TravelApp::menuSaveTrip() {
    displayHeader();
    UIHelper::printSection("儲存旅程");

    std::string filename = currentFile;
    if (filename.empty()) {
        // 自動生成檔名
        std::string suggested = "saves\\" + currentTrip->getTripName() + ".travel";
        filename = UIHelper::getInput("輸入檔案名稱（直接 Enter 使用：" + suggested + "）");
        if (filename.empty()) filename = suggested;
        // 確保有 .travel 副檔名
        if (filename.find(".travel") == std::string::npos) filename += ".travel";
    }

    if (FileManager::save(*currentTrip, filename)) {
        currentFile = filename;
        UIHelper::printSuccess("已儲存至：" + filename);
    } else {
        UIHelper::printError("儲存失敗！請確認路徑正確且有寫入權限");
    }
    UIHelper::pressEnterToContinue();
}

// ============================================================
//  確保有旅程載入
// ============================================================
bool TravelApp::ensureTripLoaded() {
    if (currentTrip) return true;
    UIHelper::printWarning("尚未載入旅程，請先建立或載入一個旅程");
    UIHelper::pressEnterToContinue();
    return false;
}
