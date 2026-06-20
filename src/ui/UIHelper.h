#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <conio.h>

// 手動定義 ENABLE_VIRTUAL_TERMINAL_PROCESSING（MinGW 8.x 可能未定義）
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#include <windows.h>

// ============================================================
//  ANSI 顏色碼命名空間
// ============================================================
namespace Color {
    const std::string RESET          = "\033[0m";
    const std::string BOLD           = "\033[1m";
    const std::string DIM            = "\033[2m";
    const std::string RED            = "\033[31m";
    const std::string GREEN          = "\033[32m";
    const std::string YELLOW         = "\033[33m";
    const std::string BLUE           = "\033[34m";
    const std::string MAGENTA        = "\033[35m";
    const std::string CYAN           = "\033[36m";
    const std::string BRIGHT_RED     = "\033[91m";
    const std::string BRIGHT_GREEN   = "\033[92m";
    const std::string BRIGHT_YELLOW  = "\033[93m";
    const std::string BRIGHT_BLUE    = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN    = "\033[96m";
    const std::string BRIGHT_WHITE   = "\033[97m";
}

// ============================================================
//  UIHelper：終端機工具靜態類別
// ============================================================
class UIHelper {
public:
    // 初始化 Windows 終端機（UTF-8 + ANSI 顏色）
    static void setupConsole() {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }

    // 清除畫面
    static void clearScreen() {
        system("cls");
    }

    // 印出分隔線（使用 string 字元而非 char，支援 UTF-8 多位元組字元）
    static void printSeparator(const std::string& ch = "-", int width = 54,
                                const std::string& color = Color::CYAN) {
        std::cout << color;
        for (int i = 0; i < width; i++) std::cout << ch;
        std::cout << Color::RESET << "\n";
    }

    // 印出主標題 Banner
    static void printBanner() {
        std::cout << "\n" << Color::BRIGHT_CYAN << Color::BOLD;
        std::cout << "  +=================================================+\n";
        std::cout << "  |                                                 |\n";
        std::cout << "  |      [MAP]  Travel Planner  v3.0               |\n";
        std::cout << "  |          C++ Final Project                      |\n";
        std::cout << "  |                                                 |\n";
        std::cout << "  +=================================================+\n";
        std::cout << Color::RESET << "\n";
    }

    // 印出小節標題
    static void printSection(const std::string& title) {
        std::cout << "\n" << Color::BRIGHT_YELLOW << Color::BOLD;
        std::cout << "  >> " << title << "\n";
        std::cout << Color::RESET;
        printSeparator("-", 54, Color::YELLOW);
    }

    // 成功訊息
    static void printSuccess(const std::string& msg) {
        std::cout << Color::BRIGHT_GREEN << "  [OK]  " << msg << Color::RESET << "\n";
    }

    // 錯誤訊息
    static void printError(const std::string& msg) {
        std::cout << Color::BRIGHT_RED << "  [!!]  " << msg << Color::RESET << "\n";
    }

    // 警告訊息
    static void printWarning(const std::string& msg) {
        std::cout << Color::BRIGHT_YELLOW << "  [**]  " << msg << Color::RESET << "\n";
    }

    // 資訊訊息
    static void printInfo(const std::string& msg) {
        std::cout << Color::BRIGHT_CYAN << "  [i]   " << msg << Color::RESET << "\n";
    }

    // 按 Enter 繼續
    static void pressEnterToContinue() {
        std::cout << Color::DIM << Color::YELLOW
                  << "\n  [ Press Enter to continue... ]" << Color::RESET;
        std::cin.get();
    }

    // 取得字串輸入
    static std::string getInput(const std::string& prompt) {
        std::cout << Color::BRIGHT_WHITE << "  >> " << prompt
                  << ": " << Color::RESET;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    // 取得整數輸入，失敗或超出範圍回傳 -1
    static int getIntInput(const std::string& prompt, int minVal = 0, int maxVal = 999) {
        while (true) {
            std::cout << Color::BRIGHT_WHITE << "  >> " << prompt
                      << " (" << minVal << "-" << maxVal << ")"
                      << ": " << Color::RESET;
            std::string s;
            std::getline(std::cin, s);
            if (s.empty()) return -1;
            try {
                int v = std::stoi(s);
                if (v >= minVal && v <= maxVal) return v;
                printError("請輸入 " + std::to_string(minVal) +
                           " 到 " + std::to_string(maxVal) + " 之間的數字");
            } catch (...) {
                printError("請輸入有效數字");
            }
        }
    }

    // 印出選單項目（保留供舊版或純顯示使用）
    static void printMenuItem(const std::string& key,
                              const std::string& label,
                              const std::string& keyColor = Color::BRIGHT_CYAN) {
        std::cout << "  " << keyColor << Color::BOLD << "[" << key << "]"
                  << Color::RESET << "  " << label << "\n";
    }

    // 方向鍵互動選單
    static int selectMenu(const std::vector<std::string>& options, const std::string& title = "") {
        int selected = 0;
        int numOptions = static_cast<int>(options.size());
        if (numOptions == 0) return -1;
        
        bool firstDraw = true;

        // 確保游標隱藏，避免閃爍
        std::cout << "\033[?25l";

        while (true) {
            if (!firstDraw) {
                // 移動游標上去覆寫（標題有 2 行的話要多退 2 行）
                int linesUp = numOptions;
                if (!title.empty()) linesUp += 2;
                std::cout << "\033[" << linesUp << "A";
            }
            firstDraw = false;

            if (!title.empty()) {
                std::cout << "\033[2K" << "  " << Color::BRIGHT_YELLOW << ">> " << title << "\n";
                std::cout << "\033[2K" << "\n";
            }

            for (int i = 0; i < numOptions; i++) {
                std::cout << "\033[2K"; // 清除該行
                if (i == selected) {
                    std::cout << "  " << Color::BRIGHT_GREEN << "▶ " 
                              << Color::BRIGHT_WHITE << options[i] << Color::RESET << "\n";
                } else {
                    std::cout << "    " << Color::DIM << options[i] << Color::RESET << "\n";
                }
            }

            int key = _getch();
            if (key == 224 || key == 0) { // Windows 方向鍵前綴
                key = _getch();
                if (key == 72) { // 上
                    selected = (selected - 1 + numOptions) % numOptions;
                } else if (key == 80) { // 下
                    selected = (selected + 1) % numOptions;
                }
            } else if (key == 13) { // Enter
                // 恢復游標顯示
                std::cout << "\033[?25h";
                return selected;
            }
        }
    }

    // 印出統計列
    static void printStat(const std::string& label, const std::string& value = "",
                           const std::string& valueColor = Color::BRIGHT_GREEN) {
        std::cout << Color::DIM << "  " << label;
        if (!value.empty()) {
            std::cout << ": " << Color::RESET << valueColor << value;
        }
        std::cout << Color::RESET << "\n";
    }
};
