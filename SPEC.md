# 軟體規格說明書 (Software Specification)

> 旅遊手冊 Travel Planner | C++ 期末專題 | v1.0

---

## 目錄

1. [需求來源](#需求來源)
2. [系統概述](#系統概述)
3. [功能需求規格](#功能需求規格)
4. [技術規格](#技術規格)
5. [類別設計規格](#類別設計規格)
6. [資料流程圖](#資料流程圖)
7. [檔案格式規格](#檔案格式規格)
8. [開發迭代計畫](#開發迭代計畫)
9. [測試計畫](#測試計畫)

---

## 需求來源

### 課程要求（教授規格）

| 編號 | 要求 | 實作對應 |
|------|------|---------|
| R1 | C++ 類別繼承功能 | `Activity` → 4 個子類別 |
| R2 | 讀檔與寫檔功能 | `FileManager` + `.travel` 格式 |
| R3 | 使用 STL 類別庫 | `vector`, `map`, `sort`, `count_if` 等 |
| R4 | 終端機模擬完整 UI | `TravelApp` + ANSI 彩色選單 |
| R5 | 完整規格說明與開發迭代流程 | `SPEC.md`（本文件） |
| R6 | 完整說明文件 | `README.md` |

### 題目選擇

**旅遊手冊**：使用者可規劃旅遊行程（如「日本 7 天 6 夜」），  
管理每天的活動、追蹤完成狀態、儲存並重新載入行程。

---

## 系統概述

### 使用者故事（User Stories）

| ID | 身份 | 我想要 | 目的 |
|----|------|--------|------|
| US-1 | 旅行者 | 建立一個有名稱、目的地和天數的旅程 | 開始規劃旅遊 |
| US-2 | 旅行者 | 為每天新增景點、餐廳、住宿、交通 | 安排詳細行程 |
| US-3 | 旅行者 | 標記活動為「已完成」 | 追蹤今日進度 |
| US-4 | 旅行者 | 查看距離出發的倒數天數 | 掌握出行時間 |
| US-5 | 旅行者 | 儲存行程並下次繼續 | 保留規劃成果 |
| US-6 | 旅行者 | 查看行程總覽與完成率統計 | 了解整體進度 |

---

## 功能需求規格

### FR-1：旅程管理

| 功能 ID | 功能名稱 | 說明 | 優先度 |
|---------|---------|------|--------|
| FR-1.1 | 建立旅程 | 輸入名稱、目的地、天數、出發日期 | P0 |
| FR-1.2 | 載入旅程 | 從 saves/ 選擇 .travel 存檔 | P0 |
| FR-1.3 | 儲存旅程 | 序列化存入 .travel 檔 | P0 |
| FR-1.4 | 行程總覽 | 顯示統計資料 + 每天進度條 | P1 |

### FR-2：活動管理

| 功能 ID | 功能名稱 | 說明 | 優先度 |
|---------|---------|------|--------|
| FR-2.1 | 新增景點 | 名稱、時間、門票、備注 | P0 |
| FR-2.2 | 新增餐廳 | 名稱、時間、料理類型、備注 | P0 |
| FR-2.3 | 新增住宿 | 名稱、時間、入住/退房、備注 | P0 |
| FR-2.4 | 新增交通 | 名稱、時間、出發/到達/方式、備注 | P0 |
| FR-2.5 | 刪除活動 | 依編號刪除指定活動 | P0 |
| FR-2.6 | 切換完成 | 已完成 ↔ 待辦切換 | P0 |
| FR-2.7 | 自動排序 | 新增後依時間 HH:MM 自動排列 | P1 |

### FR-3：日期倒數

| 狀態 | 顯示 | 顏色 |
|------|------|------|
| 距今 > 0 | `還有 N 天` | 黃色 |
| 距今 = 0 | `今天！✈` | 綠色 |
| 距今 < 0 | `已結束` | 灰色 |

---

## 技術規格

### 編譯環境

```
語言標準：C++17
編譯器：g++ (MinGW-W64) 8.1.0+
指令：g++ -std=c++17 -o TravelPlanner.exe *.cpp
```

### 終端機需求

```
作業系統：Windows 10 / 11
終端機：Windows Terminal
編碼：UTF-8 (SetConsoleOutputCP(CP_UTF8))
ANSI：ENABLE_VIRTUAL_TERMINAL_PROCESSING
```

### 記憶體管理策略

- `Day` 類別擁有 `vector<Activity*>` 的所有權
- 實作完整 **Rule of 5**：解構子 + 複製建構 + 複製指派 + 移動建構 + 移動指派
- 複製使用純虛函式 `Activity::clone()` 進行深度複製
- 多型刪除：`Activity` 有 `virtual ~Activity()`

---

## 類別設計規格

### Activity（抽象基類）

```cpp
class Activity {
protected:
    string name;         // 活動名稱
    string time;         // 時間 "HH:MM"
    string note;         // 備注
    bool   isCompleted;  // 完成狀態

public:
    // 純虛函式（子類必須實作）
    virtual string   getType()      const = 0;
    virtual string   getTypeIcon()  const = 0;
    virtual string   getTypeLabel() const = 0;
    virtual string   getDetails()   const = 0;  // 子類額外欄位
    virtual string   serialize()    const = 0;  // 序列化為檔案格式
    virtual Activity* clone()       const = 0;  // 深度複製

    // 共用展示
    virtual void display(int index) const;
};
```

### 四個子類別

| 類別 | 額外私有欄位 | serialize() 格式 |
|------|------------|-----------------|
| `Attraction` | `ticketPrice` | `ACT\|Attraction\|name\|time\|note\|done\|ticketPrice` |
| `Restaurant` | `cuisine` | `ACT\|Restaurant\|name\|time\|note\|done\|cuisine` |
| `Hotel` | `checkIn`, `checkOut` | `ACT\|Hotel\|name\|time\|note\|done\|checkIn\|checkOut` |
| `Transport` | `from`, `to`, `transportType` | `ACT\|Transport\|name\|time\|note\|done\|from\|to\|type` |

### Day（單日行程）

```
屬性：dayNumber (int), date (string), activities (vector<Activity*>)
Rule of 5：~Day(), Day(const Day&), Day(Day&&), operator=(&), operator=(&&)
STL：sort (sortByTime), count_if (getCompletedCount), copy_if (getCompleted)
```

### Trip（整趟旅遊）

```
屬性：tripName, destination, totalDays, days (vector<Day>)
統計：getTotalCompletedCount(), getCompletionStatsByType() → map<string,int>
日期：initDays(startDate) — 使用 DateUtils::addDays() 逐天計算
```

### FileManager（讀寫）

```
save(Trip&, filename) → bool
load(filename)        → Trip*   // 呼叫者負責 delete
listSaveFiles(dir)    → vector<string>
ensureSaveDir(dir)    → bool
```

---

## 資料流程圖

### 新增活動流程

```
使用者輸入
    │
    ▼
TravelApp::addActivityToDay()
    │
    ├─ createAttraction() → new Attraction(...)
    ├─ createRestaurant() → new Restaurant(...)
    ├─ createHotel()      → new Hotel(...)
    └─ createTransport()  → new Transport(...)
         │
         ▼
Trip::addActivityToDay(dayNumber, Activity*)
         │
         ▼
Day::addActivity(Activity*)
         │
    push_back + sortByTime()
         │
    (STL sort by time string)
```

### 存檔流程

```
TravelApp::menuSaveTrip()
    │
    ▼
FileManager::save(trip, filename)
    │
    ├─ 寫 "TRIP|name|dest|days|startDate"
    ├─ for each Day:
    │   ├─ 寫 "DAY|n|date"
    │   └─ for each Activity*:
    │       └─ 寫 act->serialize()
    └─ 寫 "END"
```

### 讀檔流程

```
FileManager::load(filename)
    │
    ├─ 讀 "TRIP|..." → new Trip(...)
    ├─ 讀 "DAY|..."  → currentDayNum
    └─ 讀 "ACT|..."  → deserializeActivity()
             │
             ├─ fields[1] == "Attraction" → new Attraction(...)
             ├─ fields[1] == "Restaurant" → new Restaurant(...)
             ├─ fields[1] == "Hotel"      → new Hotel(...)
             └─ fields[1] == "Transport"  → new Transport(...)
                  │
                  └─ day->addActivity(act)
```

---

## 檔案格式規格

### .travel 格式

```
TRIP|{tripName}|{destination}|{totalDays}|{startDate}
DAY|{dayNumber}|{date}
ACT|{type}|{name}|{time}|{note}|{completed}|[extra fields...]
...
END
```

### 各欄位規格

| 欄位 | 格式 | 範例 |
|------|------|------|
| tripName | 任意字串（不含`\|`） | `日本關東之旅` |
| destination | 任意字串 | `日本東京` |
| totalDays | 整數 1-365 | `7` |
| startDate | `YYYY-MM-DD` 或空 | `2026-07-01` |
| dayNumber | 整數 1-based | `1` |
| date | `YYYY-MM-DD` | `2026-07-01` |
| type | 固定字串 | `Attraction` / `Restaurant` / `Hotel` / `Transport` |
| time | `HH:MM` | `09:00` |
| completed | `0` 或 `1` | `0` |

---

## 開發迭代計畫

### v1.0（當前版本）✅ 已完成

**目標**：建立可用的核心功能

| 功能 | 狀態 |
|------|------|
| 建立 / 儲存 / 載入旅程 | ✅ |
| 新增 / 刪除活動（4 種類型） | ✅ |
| 切換活動完成狀態 | ✅ |
| 自動依時間排序 | ✅ |
| 日期倒數（還有N天/今天/已過） | ✅ |
| 行程總覽與進度條 | ✅ |
| ANSI 彩色終端機 UI | ✅ |
| 完整文件（README / SPEC / DEVLOG） | ✅ |

---

### v2.0（第二版）— 預算管理

**目標**：為每個活動加入費用欄位，提供整趟旅遊的花費統計

| 功能 | 說明 |
|------|------|
| 活動費用欄位 | 每個活動增加 `cost` 欄位（整數或浮點） |
| 每天花費統計 | `Day::getTotalCost()` |
| 整趟花費統計 | `Trip::getTotalCost()` |
| 預算設定 | `Trip::budget` 欄位 + 剩餘預算顯示 |
| 費用分類統計 | `map<string, double>` 各類別花費 |

**預計新增/修改的類別**：`Activity`（加 cost）、`Day`、`Trip`、`FileManager`（格式擴充）

---

### v3.0（第三版）— 多旅程管理

**目標**：同時管理多個旅程

| 功能 | 說明 |
|------|------|
| 旅程清單主頁 | 顯示所有已儲存的旅程 |
| 旅程比較 | 比較兩個旅程的花費與天數 |
| 活動搜尋 | 跨天搜尋活動名稱 |
| 行程篩選 | 只顯示「未完成」的活動 |
| 匯出摘要 | 輸出純文字版行程摘要（.txt） |

**預計新增的類別**：`TripManager`（管理多個 Trip）

---

### v4.0（第四版）— 進階功能

**目標**：提升實用性

| 功能 | 說明 |
|------|------|
| 活動時間衝突偵測 | 同一天同一時段有兩個活動時警告 |
| 天氣備注 | 可為每天新增天氣狀況備注 |
| 活動標籤 | 自訂 tag（#必去 #美食 等） |
| 全文搜尋 | 搜尋所有旅程的活動 |
| 行程模板 | 儲存常用行程結構為模板 |

---

## 測試計畫

### 單元測試情境

| 測試 ID | 測試對象 | 測試內容 | 預期結果 |
|---------|---------|---------|---------|
| T-1 | `Day::addActivity()` | 新增 3 個活動，時間順序相反 | 顯示時依時間排序 |
| T-2 | `Day::toggleActivity()` | 切換第 2 個活動 | isCompleted 反轉 |
| T-3 | `Day::removeActivity()` | 刪除編號 2，共 3 個 | 剩 2 個，無洞 |
| T-4 | `Day` 複製建構子 | 複製一個有 3 個活動的 Day | 獨立深度複製，改原不影響複製 |
| T-5 | `Trip::getCompletionStatsByType()` | 加入 2 景點 1 餐廳，完成 1 景點 | `{"景點": 1}` |
| T-6 | `FileManager::save + load` | 儲存後載入 | 資料完全一致 |
| T-7 | `DateUtils::daysUntil()` | 輸入昨天/今天/明天 | -1 / 0 / 1 |
| T-8 | `FileManager::load` 錯誤 | 輸入不存在的檔案 | 回傳 nullptr |

### 整合測試流程

```
1. 啟動程式
2. 建立旅程「測試旅程」，目的地「台灣」，3天，出發日 2026-07-01
3. 第 1 天新增：景點（A）、餐廳（B）、交通（C）
4. 切換 A 為完成
5. 刪除 B
6. 儲存旅程
7. 離開程式
8. 重新啟動，載入剛才的存檔
9. 驗證：第 1 天有 A(完成) 和 C(待辦)，無 B
```
