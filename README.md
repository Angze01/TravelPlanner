# 旅遊手冊 Travel Planner — 使用說明文件

> C++ 期末專題 | v2.0 | 2026

---

## 目錄

1. [專案簡介](#專案簡介)
2. [環境需求](#環境需求)
3. [專案結構](#專案結構)
4. [編譯與執行](#編譯與執行)
5. [功能操作說明](#功能操作說明)
6. [活動類型](#活動類型)
7. [檔案格式](#檔案格式)
8. [類別繼承架構](#類別繼承架構)
9. [STL 使用清單](#stl-使用清單)

---

## 專案簡介

旅遊手冊是一個以 **C++ 終端機 UI** 操作的行程管理系統。  
使用者可以：

- 建立旅遊行程（例：日本 7 天 6 夜）
- 為每天新增各類活動（景點、餐廳、住宿、交通）
- 標記活動為「完成」或「待辦」
- 查看自動計算的日期倒數（未到 / 今天 / 已過）
- 儲存行程到 `.travel` 格式檔案，下次繼續使用

---

## 環境需求

| 項目 | 需求 |
|------|------|
| 作業系統 | Windows 10 / 11 |
| 終端機 | **Windows Terminal**（支援 ANSI 彩色顯示） |
| 編譯器 | g++ (MinGW-W64) 8.1.0 以上 |
| C++ 標準 | C++17 |

> **注意**：請使用 **Windows Terminal** 執行，否則 ANSI 彩色可能無法顯示。

---

## 專案結構

```
TravelPlanner/
├── src/
│   ├── core/               核心基礎類別
│   │   ├── Activity.h/.cpp    抽象基類（含純虛函式）
│   │   ├── Day.h/.cpp         單日行程（Rule of 5）
│   │   └── Trip.h/.cpp        整趟旅遊（STL map 統計）
│   ├── activities/         活動子類別（繼承 Activity）
│   │   ├── Attraction.h/.cpp  景點
│   │   ├── Restaurant.h/.cpp  餐廳
│   │   ├── Hotel.h/.cpp       住宿
│   │   └── Transport.h/.cpp   交通
│   ├── ui/                 終端機使用者介面
│   │   ├── TravelApp.h/.cpp   主選單控制器
│   │   └── UIHelper.h         ANSI 彩色工具（header-only）
│   ├── utils/              工具函式
│   │   ├── DateUtils.h        日期倒數計算（header-only）
│   │   ├── FileManager.h/.cpp 讀寫 .travel 格式
│   └── main.cpp            程式進入點
├── docs/
│   ├── SPEC.md             軟體規格說明書
│   └── DEVLOG.md           開發日誌（AI 協作記錄）
├── saves/                  存檔目錄（執行時自動建立，gitignored）
├── .gitignore
└── README.md               本文件
```

---

## 編譯與執行

### 編譯指令

```bash
cd d:\C++檔案位置\TravelPlanner

g++ -std=c++17 ^
    -I src/core -I src/activities -I src/ui -I src/utils ^
    -o TravelPlanner.exe ^
    src/core/Activity.cpp src/core/Day.cpp src/core/Trip.cpp ^
    src/activities/Attraction.cpp src/activities/Restaurant.cpp ^
    src/activities/Hotel.cpp src/activities/Transport.cpp ^
    src/utils/FileManager.cpp ^
    src/ui/TravelApp.cpp ^
    src/main.cpp
```

> `-I` 旗標讓所有子目錄的 `#include "XXX.h"` 都能正確找到對應標頭檔，  
> 不需要修改任何 `#include` 路徑。

### 執行

```bash
.\TravelPlanner.exe
```

---

## 功能操作說明

### 主選單

```
  +=================================================+
  |      [MAP]  Travel Planner  v1.0               |
  +=================================================+

  旅程：日本關東之旅  📍 日本東京  ✈ 7 天

  [1]  建立新旅程
  [2]  載入旅程
  [3]  查看行程總覽
  [4]  管理每日活動
  [5]  儲存旅程
  [0]  離開
```

### 建立新旅程

```
  >> 旅程名稱: 日本關東之旅
  >> 目的地: 日本東京
  >> 幾天 (1-365): 7
  >> 出發日期 YYYY-MM-DD（留空則不設定）: 2026-07-01
```

系統自動為每天計算日期，並顯示出發倒數。

### 管理每日活動

天數清單（含倒數狀態）：

```
  [ 1]  第 1 天  2026-07-01  ✦ 2/5 完成  還有 11 天
  [ 2]  第 2 天  2026-07-02  ✦ 0/3 完成  還有 12 天
```

單日操作選單：

```
  [A]  新增活動    [D]  刪除活動
  [T]  切換完成狀態    [B]  返回
```

### 活動展示樣式

```
  [1] ✅ 09:00  淺草寺  [景點]  🎫 免費
        📝 日本最古老的寺廟
  [2] ⬜ 12:00  壽司大  [餐廳]  🍜 迴轉壽司
```

### 行程總覽進度條

```
  Day  1  [##########..........]  3/5
  Day  2  [####................]  1/4
  Day  3  [....................]  0/3
```

---

## 活動類型

| 類型 | 圖示 | 額外欄位 |
|------|------|---------|
| 景點 Attraction | 🏛 | 門票費用 |
| 餐廳 Restaurant | 🍽 | 料理類型 |
| 住宿 Hotel | 🏨 | 入住時間、退房時間 |
| 交通 Transport | 🚌 | 出發地、目的地、交通方式 |

所有類型共有：名稱、時間（HH:MM）、備注、完成狀態。  
新增後會**自動依時間排序**。

---

## 檔案格式

存檔儲存於 `saves/`，副檔名 `.travel`：

```
TRIP|旅程名稱|目的地|天數|出發日期
DAY|1|2026-07-01
ACT|Attraction|淺草寺|09:00|日本最古老的寺廟|0|免費
ACT|Restaurant|壽司大|12:00|外頭要排隊|0|迴轉壽司
ACT|Hotel|東急 STAY|15:00||1|15:00|11:00
DAY|2|2026-07-02
ACT|Transport|成田→新宿|08:00||0|成田機場|新宿|成田特快
END
```

> ⚠ 名稱與備注中不可使用 `|` 字元

---

## 類別繼承架構

```
Activity  ← 抽象基類（純虛函式：getType / display / serialize / clone）
├── Attraction   景點  + ticketPrice
├── Restaurant   餐廳  + cuisine
├── Hotel        住宿  + checkIn, checkOut
└── Transport    交通  + from, to, transportType

Day          單日行程  vector<Activity*>  Rule of 5
Trip         整趟旅遊  vector<Day>        STL map 統計
FileManager  檔案讀寫  fstream / sstream
TravelApp    UI 控制  所有選單邏輯
DateUtils    日期工具  namespace（header-only）
UIHelper     顯示工具  ANSI 顏色（header-only）
```

---

## STL 使用清單

| STL 功能 | 使用位置 | 用途 |
|----------|---------|------|
| `vector<Activity*>` | `Day` | 儲存活動（擁有指標生命週期） |
| `vector<Day>` | `Trip` | 儲存每天行程 |
| `map<string, int>` | `Trip::getCompletionStatsByType()` | 各類別完成數統計 |
| `fstream` | `FileManager` | 讀寫 .travel 檔案 |
| `sstream` | `FileManager` | 解析每行欄位 |
| `algorithm::sort` | `Day::sortByTime()` | 依時間自動排序 |
| `algorithm::count_if` | `Day::getCompletedCount()` | 計算已完成數 |
| `algorithm::copy_if` | `Day::getCompleted()` | 篩選完成/未完成清單 |
| `algorithm::transform` | `TravelApp` | 輸入字元轉小寫 |
| `iomanip::setw` | `Trip`, `TravelApp` | 格式化對齊輸出 |

---

## 文件索引

| 文件 | 路徑 | 說明 |
|------|------|------|
| 使用說明 | `README.md` | 本文件 |
| 規格說明書 | `docs/SPEC.md` | 需求、類別設計、迭代計畫、測試案例 |
| 開發日誌 | `docs/DEVLOG.md` | AI 協作紀錄、設計決策、Bug 解法 |

---

## 作者資訊

| 欄位 | 內容 |
|------|------|
| 姓名 | 蔡進澤 |
| 學號 | 4B1G0907 |
| 課程 | C++ 程式設計 |
| 學期 | 2026 |
