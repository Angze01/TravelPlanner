# 旅遊手冊 Travel Planner — C++ 期末專題

## 專案簡介

旅遊手冊是一個以 **C++ 終端機 UI** 操作的行程管理系統。  
使用者可建立旅遊行程（如「日本 7 天 6 夜」），為每天新增活動，標記完成狀態，並自動顯示倒數天數。

---

## 開發環境

| 項目 | 版本 |
|------|------|
| 語言 | C++17 |
| 編譯器 | g++ (MinGW-W64 8.1.0) |
| 作業系統 | Windows 10/11 |
| 終端機 | Windows Terminal（建議）|

---

## 編譯方式

```bash
# 進入專案目錄
cd d:\C++檔案位置\TravelPlanner

# 編譯（一行指令）
g++ -std=c++17 -o TravelPlanner.exe Activity.cpp Attraction.cpp Restaurant.cpp Hotel.cpp Transport.cpp Day.cpp Trip.cpp FileManager.cpp TravelApp.cpp main.cpp

# 執行
.\TravelPlanner.exe
```

---

## 類別繼承架構

```
Activity  (抽象基類)
├── Attraction   繼承：景點（含門票費用）
├── Restaurant   繼承：餐廳（含料理類型）
├── Hotel        繼承：住宿（含入退房時間）
└── Transport    繼承：交通（含出發地/目的地/交通方式）

Day              單日行程（管理多個 Activity，Rule of 5）
Trip             整趟旅遊（管理多個 Day，含統計）
FileManager      檔案讀寫（.travel 格式）
TravelApp        終端機 UI 主控制器
DateUtils        日期計算工具（命名空間）
UIHelper         ANSI 終端機顯示工具
```

---

## STL 使用清單

| STL 功能 | 使用位置 | 說明 |
|----------|---------|------|
| `vector<Activity*>` | `Day` | 儲存活動指標 |
| `vector<Day>` | `Trip` | 儲存每天行程 |
| `map<string, int>` | `Trip::getCompletionStatsByType()` | 各類別完成統計 |
| `string` | 所有類別 | 文字欄位 |
| `fstream` | `FileManager` | 讀寫 .travel 檔案 |
| `sstream` | `FileManager` | 解析行資料 |
| `algorithm::sort` | `Day::sortByTime()` | 依時間自動排序 |
| `algorithm::count_if` | `Day::getCompletedCount()` | 計算完成數 |
| `algorithm::copy_if` | `Day::getCompleted()` | 篩選已完成活動 |
| `iomanip` | `Trip`, `TravelApp` | 格式化輸出 |

---

## 功能說明（v1.0）

### 主選單
| 功能 | 說明 |
|------|------|
| 建立新旅程 | 輸入名稱、目的地、天數、出發日期 |
| 載入旅程 | 從 saves/ 目錄選擇 .travel 存檔 |
| 查看行程總覽 | 顯示統計資料與每天進度條 |
| 管理每日活動 | 新增/刪除/切換活動完成狀態 |
| 儲存旅程 | 存成 .travel 格式檔案 |

### 活動類型
| 類型 | 額外欄位 |
|------|---------|
| 景點 (Attraction) | 門票費用 |
| 餐廳 (Restaurant) | 料理類型 |
| 住宿 (Hotel) | 入住時間、退房時間 |
| 交通 (Transport) | 出發地、目的地、交通方式 |

### 日期倒數功能
- 未到：顯示「還有 N 天」（黃色）
- 當天：顯示「今天！✈」（綠色）
- 已過：顯示「已結束」（灰色）

---

## 檔案格式（.travel）

存檔儲存於 `saves/` 目錄，格式如下：

```
TRIP|旅程名稱|目的地|天數|出發日期
DAY|天數|日期
ACT|Attraction|名稱|時間|備注|完成(0/1)|門票
ACT|Restaurant|名稱|時間|備注|完成(0/1)|料理
ACT|Hotel|名稱|時間|備注|完成(0/1)|入住|退房
ACT|Transport|名稱|時間|備注|完成(0/1)|出發地|目的地|方式
END
```

---

## 開發迭代規劃

| 版本 | 功能 | 狀態 |
|------|------|------|
| **v1.0** | 基本行程管理、4 種活動類型、存讀檔、日期倒數 | ✅ 完成 |
| **v2.0** | 預算追蹤、費用統計、每日花費報告 | 計畫中 |
| **v3.0** | 多旅程管理、搜尋/篩選活動、匯出行程摘要 | 計畫中 |
| **v4.0** | 天氣提醒、地圖連結、行程分享 | 計畫中 |

---

## 物件導向設計重點

### 抽象基類（Activity）
- 使用純虛函式 `getType()`, `display()`, `serialize()`, `clone()` 強制子類實作
- `virtual ~Activity()` 支援多型安全刪除

### 繼承（Rule of 3/5）
- `Day` 持有 `vector<Activity*>`，實作完整 Rule of 5
  - 解構子：釋放所有指標
  - 複製建構子：深度複製（呼叫 `clone()`）
  - 複製指派、移動建構、移動指派

### 多型展示
- `Activity::display()` 為虛函式，各子類覆寫 `getDetails()` 提供差異化資訊
- `FileManager` 讀檔時根據類型字串動態建立正確子類物件

---

