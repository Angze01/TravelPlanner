#include "Activity.h"
#include <iomanip>

// ============================================================
//  Activity 建構子
// ============================================================
Activity::Activity(const std::string& name,
                   const std::string& time,
                   const std::string& note)
    : name(name), time(time), note(note), isCompleted(false)
{}

// ============================================================
//  共用展示函式
//  在終端機上印出一列活動資訊（供子類別呼叫）
// ============================================================
void Activity::display(int index) const {
    // 狀態圖示與顏色
    std::string statusIcon  = isCompleted ? "✅" : "⬜";
    std::string statusColor = isCompleted ? Color::BRIGHT_GREEN : Color::BRIGHT_YELLOW;

    // 索引（若有）
    std::string idxStr = "";
    if (index >= 0) {
        idxStr = Color::DIM + "[" + std::to_string(index) + "] " + Color::RESET;
    }

    // 主行：索引 + 狀態 + 時間 + 名稱 + 類型標籤
    std::cout << "  " << idxStr
              << statusIcon << " "
              << statusColor << Color::BOLD
              << std::setw(5) << std::left << time
              << Color::RESET << "  "
              << Color::BRIGHT_WHITE << name << Color::RESET
              << Color::DIM << "  [" << getTypeLabel() << "] " << Color::RESET;

    // 子類別細節（票價、料理等）
    std::string details = getDetails();
    if (!details.empty()) {
        std::cout << Color::MAGENTA << details << Color::RESET;
    }
    std::cout << "\n";

    // 備注（若有）
    if (!note.empty()) {
        std::cout << Color::DIM
                  << "        📝 " << note
                  << Color::RESET << "\n";
    }
}
