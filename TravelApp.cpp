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
    std::cout << "\n";
    UIHelper::printMenuItem("1", "建立新旅程");
    UIHelper::printMenuItem("2", "載入旅程");
    UIHelper::printMenuItem("3", "查看行程總覽");
    UIHelper::printMenuItem("4", "管理每日活動");
    UIHelper::printMenuItem("5", "儲存旅程");
    UIHelper::printMenuItem("0", "離開", Color::BRIGHT_RED);
    std::cout << "\n";

    std::string choice = UIHelper::getInput("請選擇功能");
    if      (choice == "1") menuCreateTrip();
    else if (choice == "2") menuLoadTrip();
    else if (choice == "3") { if (ensureTripLoaded()) menuTripSummary(); }
    else if (choice == "4") { if (ensureTripLoaded()) menuManageDays(); }
    else if (choice == "5") { if (ensureTripLoaded()) menuSaveTrip(); }
    else if (choice == "0") running = false;
    else UIHelper::printWarning("無效的選項，請重試");
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

    // 詢問出發日期
    std::string startDate = "";
    while (true) {
        startDate = UIHelper::getInput("出發日期 YYYY-MM-DD（留空則不設定）");
        if (startDate.empty() || DateUtils::isValidDate(startDate)) break;
        UIHelper::printError("日期格式錯誤，請輸入 YYYY-MM-DD（例：2026-07-01）");
    }

    clearTrip();
    currentTrip = new Trip(name, dest, days);
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

    std::cout << "\n  找到以下存檔：\n";
    for (int i = 0; i < static_cast<int>(files.size()); i++) {
        // 從路徑取出檔名
        std::string fname = files[i];
        auto pos = fname.find_last_of("\\/");
        if (pos != std::string::npos) fname = fname.substr(pos + 1);
        UIHelper::printMenuItem(std::to_string(i + 1), fname);
    }
    UIHelper::printMenuItem("0", "手動輸入路徑");
    std::cout << "\n";

    int choice = UIHelper::getIntInput("請選擇", 0, static_cast<int>(files.size()));
    std::string filename = "";
    if (choice == 0) {
        filename = UIHelper::getInput("輸入檔案路徑");
    } else if (choice >= 1 && choice <= static_cast<int>(files.size())) {
        filename = files[choice - 1];
    } else {
        UIHelper::printError("無效選項"); UIHelper::pressEnterToContinue(); return;
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
        UIHelper::printMenuItem("0", "返回主選單", Color::BRIGHT_RED);

        int choice = UIHelper::getIntInput("選擇要管理的天數", 0, currentTrip->getTotalDays());
        if (choice == 0) return;
        if (choice < 1 || choice > currentTrip->getTotalDays()) {
            UIHelper::printError("無效天數");
            continue;
        }
        menuManageDay(choice);
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
        std::cout << "\n";
        UIHelper::printMenuItem("A", "新增活動",       Color::BRIGHT_GREEN);
        UIHelper::printMenuItem("D", "刪除活動",       Color::BRIGHT_RED);
        UIHelper::printMenuItem("T", "切換完成狀態",   Color::BRIGHT_YELLOW);
        UIHelper::printMenuItem("B", "返回",            Color::BRIGHT_RED);
        std::cout << "\n";

        std::string choice = UIHelper::getInput("請選擇操作");
        // 轉小寫
        std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower);

        if      (choice == "a") addActivityToDay(dayNumber);
        else if (choice == "d") removeActivityFromDay(dayNumber);
        else if (choice == "t") toggleActivityInDay(dayNumber);
        else if (choice == "b") return;
        else UIHelper::printWarning("無效操作");
    }
}

// ============================================================
//  新增活動（選擇類型後分派）
// ============================================================
void TravelApp::addActivityToDay(int dayNumber) {
    UIHelper::printSection("新增活動");
    std::cout << "\n";
    UIHelper::printMenuItem("1", "🏛  景點 (Attraction)");
    UIHelper::printMenuItem("2", "🍽  餐廳 (Restaurant)");
    UIHelper::printMenuItem("3", "🏨  住宿 (Hotel)");
    UIHelper::printMenuItem("4", "🚌  交通 (Transport)");
    UIHelper::printMenuItem("0", "取消");
    std::cout << "\n";

    int type = UIHelper::getIntInput("選擇活動類型", 0, 4);
    if (type == 0) return;

    Activity* act = nullptr;
    switch (type) {
        case 1: act = createAttraction(); break;
        case 2: act = createRestaurant(); break;
        case 3: act = createHotel();      break;
        case 4: act = createTransport();  break;
        default: return;
    }

    if (act) {
        currentTrip->addActivityToDay(dayNumber, act);
        UIHelper::printSuccess("活動「" + act->getName() + "」已新增！");
        UIHelper::pressEnterToContinue();
    }
}

// ============================================================
//  建立景點
// ============================================================
Activity* TravelApp::createAttraction() {
    UIHelper::printSection("新增景點");
    std::string name = UIHelper::getInput("景點名稱");
    if (name.empty()) return nullptr;
    std::string time = UIHelper::getInput("時間 HH:MM");
    std::string ticket = UIHelper::getInput("門票費用（留空略過）");
    std::string note = UIHelper::getInput("備注（留空略過）");
    return new Attraction(name, time, note, ticket);
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
    return new Restaurant(name, time, note, cuisine);
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
    return new Hotel(name, time, note, checkIn, checkOut);
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
    return new Transport(name, time, note, from, to, type);
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
    int idx = UIHelper::getIntInput("輸入要刪除的活動編號", 1, day->getActivityCount());
    if (idx < 1) return;

    Activity* act = day->getActivity(idx);
    if (!act) { UIHelper::printError("無效編號"); UIHelper::pressEnterToContinue(); return; }

    std::string actName = act->getName();
    if (currentTrip->removeActivityFromDay(dayNumber, idx)) {
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
    int idx = UIHelper::getIntInput("輸入要切換的活動編號", 1, day->getActivityCount());
    if (idx < 1) return;

    if (currentTrip->toggleActivityInDay(dayNumber, idx)) {
        Activity* act = day->getActivity(idx);
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
