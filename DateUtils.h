#pragma once
#include <string>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <iomanip>

// ============================================================
//  DateUtils — 日期工具命名空間
//  提供日期字串解析、日期加法、倒數計算等功能
// ============================================================
namespace DateUtils {

    // 解析 "YYYY-MM-DD" 字串為 time_t（失敗回傳 -1）
    inline time_t parseDate(const std::string& dateStr) {
        if (dateStr.size() < 10) return -1;
        struct tm t = {};
        if (sscanf(dateStr.c_str(), "%d-%d-%d",
                   &t.tm_year, &t.tm_mon, &t.tm_mday) != 3) return -1;
        t.tm_year -= 1900;
        t.tm_mon  -= 1;
        t.tm_hour = 0; t.tm_min = 0; t.tm_sec = 0;
        t.tm_isdst = -1;
        return mktime(&t);
    }

    // 取得今日日期字串 "YYYY-MM-DD"
    inline std::string today() {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", t);
        return std::string(buf);
    }

    // 日期加 N 天，回傳 "YYYY-MM-DD"
    inline std::string addDays(const std::string& dateStr, int days) {
        time_t t = parseDate(dateStr);
        if (t < 0) return dateStr;
        t += static_cast<time_t>(days) * 86400;
        struct tm* tm_info = localtime(&t);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
        return std::string(buf);
    }

    // 計算距今天數（正 = 未來，0 = 今天，負 = 過去）
    inline int daysUntil(const std::string& dateStr) {
        time_t target = parseDate(dateStr);
        if (target < 0) return 0;
        time_t now    = time(nullptr);
        struct tm* tmNow = localtime(&now);
        tmNow->tm_hour = 0;
        tmNow->tm_min  = 0;
        tmNow->tm_sec  = 0;
        tmNow->tm_isdst = -1;
        time_t todayT = mktime(tmNow);
        double diff = difftime(target, todayT);
        return static_cast<int>(diff / 86400.0);
    }

    // 取得顯示用的狀態字串
    //   未來：「還有 N 天」
    //   今天：「今天！✈」
    //   過去：「已結束」
    inline std::string getStatusLabel(const std::string& dateStr) {
        if (dateStr.empty()) return "";
        int d = daysUntil(dateStr);
        if (d > 0)  return "還有 " + std::to_string(d) + " 天";
        if (d == 0) return "今天！✈";
        return "已結束";
    }

    // 取得狀態對應的 ANSI 顏色碼
    inline std::string getStatusColor(const std::string& dateStr) {
        if (dateStr.empty()) return "\033[2m";  // dim
        int d = daysUntil(dateStr);
        if (d > 0)  return "\033[93m";  // bright yellow
        if (d == 0) return "\033[92m";  // bright green
        return "\033[2m";               // dim (past)
    }

    // 驗證日期格式是否正確 "YYYY-MM-DD"
    inline bool isValidDate(const std::string& dateStr) {
        if (dateStr.size() != 10) return false;
        if (dateStr[4] != '-' || dateStr[7] != '-') return false;
        return parseDate(dateStr) != -1;
    }

} // namespace DateUtils
