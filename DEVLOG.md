# 開發日誌 (Development Log)

> 旅遊手冊 Travel Planner | C++ 期末專題  
> 記錄 AI 協作開發過程中的所有決策、討論與設計理由

---

## 目錄

1. [專案緣起](#專案緣起)
2. [AI 協作對話紀錄](#ai-協作對話紀錄)
3. [關鍵設計決策](#關鍵設計決策)
4. [遭遇問題與解決方案](#遭遇問題與解決方案)
5. [版本變更記錄](#版本變更記錄)

---

## 專案緣起

**時間**：2026-06-20  
**來源**：C++ 程式設計課程期末專題

### 教授要求

```
請各位同學使用 AI 來完成期末專題。題目要求如下：
  ✅ 呈現 C++ 的類別繼承功能
  ✅ 讀檔與寫檔功能
  ✅ 使用 STL 類別庫
  ✅ 使用終端機模擬完整的 UI 操作介面
  ✅ 完整的規格說明與開發迭代流程
  ✅ 完整的說明文件
```

### 題目選定

選擇「旅遊手冊」作為主題：  
> *「類似說我今天想玩日本 7 天 6 夜，那我可以把行程加入，有已完成和未完成，可以把它選取完成自動歸類到當天完成」*

---

## AI 協作對話紀錄

### 第一輪對話 — 確認需求

**使用者**：提出期末專題需求，主題為旅遊手冊  
**AI 行動**：建立完整 implementation_plan.md，包含：
- 類別繼承架構圖
- 終端機 UI 草稿
- 檔案格式設計
- 四個開放問題待確認

**AI 提出的開放問題**：

```
1. 日期格式：輸入實際日期還是「第 N 天」？
2. 活動分類：v1 要 4 種類型還是先統一？
3. 終端機：是否使用 Windows Terminal？
4. 編譯器：g++ (MinGW) 還是 MSVC？
```

---

### 第二輪對話 — 確認設計決策

**使用者回答**：

| 問題 | 使用者回答 | 採用方案 |
|------|-----------|---------|
| 日期 | 「寫日期子加倒數(還未到)或第N天(正玩)」 | 實際日期 + 倒數/今天/已過三種狀態 |
| 分類 | 「V1~V4 版慢慢做」 | V1 全做 4 種類型，後續版本加新功能 |
| 終端機 | 「可以」 | Windows Terminal + ANSI 色彩 ✅ |
| 編譯器 | 「哪個好我不知道，選好的」 | 選 **g++ (MinGW)**，理由見下方 |

---

### 第三輪對話 — 文件需求

**使用者**：「我也希望聊天的東西要記憶起來，還有完整的規格說明與開發迭代流程，完整的說明文件，共三個MD」

**採用方案**：
- `README.md` — 使用者說明文件
- `SPEC.md` — 軟體規格說明書
- `DEVLOG.md` — 開發日誌（本文件）

---

## 關鍵設計決策

### 決策 1：編譯器選擇 → g++ (MinGW-W64)

**考量因素**：
- g++ 是 GCC 標準，跨平台可移植
- MinGW-W64 在 Windows 上免費、安裝簡單
- 課程常用工具，教授熟悉
- MSVC 雖效能略好，但語法有些 Windows 獨有擴充，不適合教學

**結論**：✅ 使用 `g++ (MinGW-W64)` 搭配 `-std=c++17`

---

### 決策 2：日期顯示邏輯

**需求**：日期要顯示倒數，且當天要特別標示

**設計**：
```
未到的天 → 黃色「還有 N 天」
今天     → 綠色「今天！✈」
已過的天 → 灰色「已結束」
```

**實作**：獨立封裝為 `DateUtils.h`（header-only namespace），避免循環依賴  
**函式**：`daysUntil()`, `getStatusLabel()`, `getStatusColor()`

---

### 決策 3：抽象基類設計

**問題**：Activity 如何讓 Day 能深度複製（Rule of 5 需要）？

**方案 A**：使用 `shared_ptr<Activity>` → 簡單但稍微複雜
**方案 B**：使用 `unique_ptr<Activity>` → Move-only，vector<Day> 不易使用
**方案 C**：使用 raw `Activity*` + 純虛 `clone()` + Rule of 5 → **選用**

**理由**：
- `clone()` 是教科書級多型設計
- Rule of 5 是 C++ 重要知識點，適合期末展示
- 對教授展示效果最好

```cpp
// Activity 加入純虛 clone()
virtual Activity* clone() const = 0;

// 子類實作（Prototype Pattern）
Activity* Attraction::clone() const { return new Attraction(*this); }

// Day 複製建構子利用 clone() 深度複製
Day::Day(const Day& other) {
    for (Activity* a : other.activities)
        activities.push_back(a->clone());
}
```

---

### 決策 4：STL 的使用策略

**目標**：讓 STL 的使用明顯且有意義（不只是裝飾）

| STL 功能 | 使用位置 | 是否有意義 |
|----------|---------|-----------|
| `vector<Activity*>` | Day 的活動儲存 | ✅ 動態大小 |
| `vector<Day>` | Trip 的天數儲存 | ✅ 動態大小 |
| `map<string, int>` | 各類別完成統計 | ✅ 自動排序鍵值 |
| `sort` | 活動依時間排序 | ✅ lambda 展示 |
| `count_if` | 計算完成數 | ✅ 函數式風格 |
| `copy_if` | 篩選完成活動 | ✅ 函數式風格 |
| `fstream` | 讀寫 .travel | ✅ 讀寫分離 |
| `sstream` | 解析行欄位 | ✅ 字串處理 |

---

### 決策 5：檔案格式設計

**考量**：簡單、可讀、不依賴第三方函式庫（如 JSON）

**格式選擇**：Pipe-separated（`|` 分隔）

**理由**：
- 中文旅程名稱不含 `|`（常見）
- 不需引入 JSON/XML 解析器
- 用 `stringstream` + `getline` 就能解析
- 方便直接用文字編輯器查看和手動修改

**限制**：名稱和備注中不可使用 `|` 字元（已記錄於 README.md）

---

### 決策 6：UI 的設計哲學

**使用 ASCII 框線而非 Unicode 方框字元**

原本設計使用：`╔══╗`, `║`, `╚══╝`  
**問題**：MinGW 8.1.0 的 `char` 不支援多位元組字元常數，導致編譯警告

**解決**：
1. 主標題 Banner 改用 `+===+` 格式
2. `printSeparator()` 的參數從 `char` 改為 `string`
3. 其他地方的 `'─'` 全改為 `"-"`

**教訓**：不要在 `char` 常數裡放多位元組字元，應改用 `const std::string`

---

## 遭遇問題與解決方案

### Bug 1：ENABLE_VIRTUAL_TERMINAL_PROCESSING 未定義

**問題**：MinGW 8.1.0 的 `<windows.h>` 中沒有定義 `ENABLE_VIRTUAL_TERMINAL_PROCESSING`

**錯誤訊息**：
```
UIHelper.h:41: error: 'ENABLE_VIRTUAL_TERMINAL_PROCESSING' was not declared
```

**解決**：在 `UIHelper.h` 的 `#include <windows.h>` 之前手動定義：
```cpp
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#include <windows.h>
```

**根本原因**：此常數在 Windows SDK 較新版本才定義，MinGW 8.x 的 SDK 版本較舊

---

### Bug 2：多位元組字元傳入 char 參數

**問題**：`printSeparator('─', ...)` 中，`'─'` 是 UTF-8 多位元組字元，不能放入 `char`

**錯誤訊息**：
```
warning: multi-character character constant
warning: overflow in conversion from 'int' to 'char'
```

**解決**：將 `printSeparator` 的第一個參數從 `char` 改為 `const std::string&`：
```cpp
// 修改前
static void printSeparator(char c = '-', int width = 54, ...);

// 修改後
static void printSeparator(const std::string& ch = "-", int width = 54, ...);
```

---

### Bug 3：Trip::initDays() 每天日期相同

**問題**：原本 `initDays()` 所有天都設成 `startDate`，沒有遞增

**原程式碼**：
```cpp
dateStr = startDate; // 簡化：全部顯示起始日期
```

**解決**：引入 `DateUtils.h`，用 `addDays(startDate, i - 1)` 正確計算：
```cpp
dateStr = DateUtils::addDays(startDate, i - 1);
// 第1天 = startDate + 0
// 第2天 = startDate + 1
// ...
```

---

## 版本變更記錄

### v1.0.0 — 2026-06-20（初始版本）

**新增**：
- Activity 抽象基類 + 4 個子類別（Attraction, Restaurant, Hotel, Transport）
- Day 類別（含 Rule of 5 + STL sort/count_if/copy_if）
- Trip 類別（含 STL map 統計 + initDays 日期計算）
- FileManager（.travel 格式序列化/反序列化）
- TravelApp 終端機 UI（ANSI 彩色選單）
- DateUtils（日期倒數計算 namespace）
- UIHelper（ANSI 顏色 + 工具函式 header-only）

**修復**：
- `ENABLE_VIRTUAL_TERMINAL_PROCESSING` 未定義問題
- `char` 多位元組字元問題
- `initDays()` 日期不正確遞增問題

**已知限制**：
- 名稱和備注不可使用 `|` 字元
- 日期格式只支援 `YYYY-MM-DD`
- 僅支援 Windows 系統

---

## 未來改進備忘

從開發過程中記錄的改進想法：

- [ ] **v2.0**：加入 `cost` 欄位到每個活動，顯示每天和整趟花費
- [ ] **v2.0**：設定行程預算，顯示「剩餘預算」
- [ ] **v3.0**：多旅程管理（TripManager 類別）
- [ ] **v3.0**：活動全文搜尋功能
- [ ] **v3.0**：匯出行程為純文字摘要
- [ ] **v4.0**：活動時間衝突偵測
- [ ] **v4.0**：活動自訂標籤（#必去 #美食）
- [ ] **技術**：改用 `nlohmann/json` 替換自訂 `.travel` 格式（v3+）
- [ ] **技術**：加入簡單的 unit test（catch2 或 Google Test）
- [ ] **Windows**：支援 `SetConsoleTitle()` 顯示旅程名稱於視窗標題列
