#include "FileManager.h"
#include "Attraction.h"
#include "Restaurant.h"
#include "Hotel.h"
#include "Transport.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <direct.h>   // _mkdir (Windows)
#include <windows.h>

// ============================================================
//  字串轉換工具 (UTF-8 <-> ANSI) 以支援 Windows MinGW fstream
// ============================================================
static std::string utf8_to_ansi(const std::string& str) {
    if (str.empty()) return "";
    // UTF-8 to UTF-16
    int wlen = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], wlen);

    // UTF-16 to ANSI (CP_ACP)
    int alen = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string ansiStr(alen, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &ansiStr[0], alen, NULL, NULL);

    return ansiStr;
}

static std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

static std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// ============================================================
//  split — 以分隔符切割字串
// ============================================================
std::vector<std::string> FileManager::split(const std::string& line, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ============================================================
//  ensureSaveDir — 確保存檔目錄存在
// ============================================================
bool FileManager::ensureSaveDir(const std::string& dir) {
    _mkdir(dir.c_str());  // 目錄已存在時不報錯
    return true;
}

// ============================================================
//  save — 將旅程序列化寫入 .travel 檔
// ============================================================
bool FileManager::save(const Trip& trip, const std::string& filename) {
    ensureSaveDir("saves");
    std::ofstream file(utf8_to_ansi(filename), std::ios::out | std::ios::trunc);
    if (!file.is_open()) return false;

    // 找出第一天的日期（作為 startDate）
    std::string startDate = "";
    if (!trip.getDays().empty()) {
        startDate = trip.getDays()[0].getDate();
    }

    // 寫入旅程標頭
    file << "TRIP|" << trip.getTripName()    << "|"
                    << trip.getDestination() << "|"
                    << trip.getTotalDays()   << "|"
                    << startDate             << "|"
                    << trip.getBudget()      << "\n";

    // 寫入每天資料
    for (const Day& day : trip.getDays()) {
        file << "DAY|" << day.getDayNumber() << "|" << day.getDate() << "\n";
        for (const Activity* act : day.getActivities()) {
            file << act->serialize() << "\n";
        }
    }

    file << "END\n";
    file.close();
    return true;
}

// ============================================================
//  deserializeActivity — 由一行文字重建 Activity 物件
// ============================================================
Activity* FileManager::deserializeActivity(const std::string& line) {
    auto fields = split(line, '|');
    // 最少需要 7 個欄位：ACT|type|name|time|note|completed|...
    if (fields.size() < 7 || fields[0] != "ACT") return nullptr;

    std::string type      = fields[1];
    std::string name      = fields[2];
    std::string time      = fields[3];
    std::string note      = fields[4];
    bool        completed = (fields[5] == "1");

    Activity* act = nullptr;

    if (type == "Attraction") {
        std::string ticketPrice = (fields.size() > 6) ? fields[6] : "";
        double cost = (fields.size() > 7 && !fields[7].empty()) ? std::stod(fields[7]) : 0.0;
        act = new Attraction(name, time, note, ticketPrice, cost);

    } else if (type == "Restaurant") {
        std::string cuisine = (fields.size() > 6) ? fields[6] : "";
        double cost = (fields.size() > 7 && !fields[7].empty()) ? std::stod(fields[7]) : 0.0;
        act = new Restaurant(name, time, note, cuisine, cost);

    } else if (type == "Hotel") {
        std::string checkIn  = (fields.size() > 6) ? fields[6] : "";
        std::string checkOut = (fields.size() > 7) ? fields[7] : "";
        double cost = (fields.size() > 8 && !fields[8].empty()) ? std::stod(fields[8]) : 0.0;
        act = new Hotel(name, time, note, checkIn, checkOut, cost);

    } else if (type == "Transport") {
        std::string from          = (fields.size() > 6) ? fields[6] : "";
        std::string to            = (fields.size() > 7) ? fields[7] : "";
        std::string transportType = (fields.size() > 8) ? fields[8] : "";
        double cost = (fields.size() > 9 && !fields[9].empty()) ? std::stod(fields[9]) : 0.0;
        act = new Transport(name, time, note, from, to, transportType, cost);
    }

    if (act) act->setCompleted(completed);
    return act;
}

// ============================================================
//  load — 從 .travel 檔還原旅程
// ============================================================
Trip* FileManager::load(const std::string& filename) {
    std::ifstream file(utf8_to_ansi(filename));
    if (!file.is_open()) return nullptr;

    Trip* trip = nullptr;
    std::string line;
    int currentDayNum = -1;

    while (std::getline(file, line)) {
        if (line.empty() || line == "END") continue;

        auto fields = split(line, '|');
        if (fields.empty()) continue;

        if (fields[0] == "TRIP" && fields.size() >= 5) {
            std::string name      = fields[1];
            std::string dest      = fields[2];
            int         totalDays = std::stoi(fields[3]);
            std::string startDate = fields[4];
            double      budget    = (fields.size() > 5 && !fields[5].empty()) ? std::stod(fields[5]) : 0.0;
            trip = new Trip(name, dest, totalDays, budget);
            trip->initDays(startDate);  // 使用 DateUtils 計算每天日期

        } else if (fields[0] == "DAY" && fields.size() >= 3 && trip) {
            currentDayNum = std::stoi(fields[1]);
            std::string date = fields[2];
            // 更新對應天的日期（initDays 已建立，這裡同步日期）
            Day* day = trip->getDay(currentDayNum);
            if (day) day->setDate(date);

        } else if (fields[0] == "ACT" && trip && currentDayNum >= 1) {
            Activity* act = deserializeActivity(line);
            if (act) {
                Day* day = trip->getDay(currentDayNum);
                if (day) {
                    day->addActivity(act);
                } else {
                    delete act;
                }
            }
        }
    }

    file.close();
    return trip;
}

// ============================================================
//  listSaveFiles — 列出存檔目錄中所有 .travel 檔案
// ============================================================
std::vector<std::string> FileManager::listSaveFiles(const std::string& dir) {
    std::vector<std::string> files;
    std::string pattern = dir + "\\*.travel";
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(utf8_to_wstring(pattern).c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) return files;
    do {
        std::string fname = wstring_to_utf8(findData.cFileName);
        files.push_back(dir + "\\" + fname);
    } while (FindNextFileW(hFind, &findData));
    FindClose(hFind);
    return files;
}
