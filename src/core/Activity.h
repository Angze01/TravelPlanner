#pragma once
#include <string>
#include <iostream>
#include "UIHelper.h"

// ============================================================
//  Activity — 旅遊活動抽象基類
//  所有活動類型（景點、餐廳、住宿、交通）皆繼承此類
// ============================================================
class Activity {
protected:
    std::string name;         // 活動名稱
    std::string time;         // 時間 HH:MM
    std::string note;         // 備注
    bool        isCompleted;  // 是否已完成

public:
    // 建構子
    Activity(const std::string& name,
             const std::string& time,
             const std::string& note = "");

    // 虛擬解構子（支援多型刪除）
    virtual ~Activity() = default;

    // ── 純虛函式（子類別必須實作）──────────────────────────
    virtual std::string getType()      const = 0;  // e.g. "Attraction"
    virtual std::string getTypeIcon()  const = 0;  // e.g. "🏛"
    virtual std::string getTypeLabel() const = 0;  // e.g. "景點"
    virtual std::string getDetails()   const = 0;  // 子類別額外屬性
    virtual std::string serialize()    const = 0;  // 序列化為檔案格式
    virtual Activity*   clone()        const = 0;  // 深度複製（Rule of 5 用）

    // ── 共用展示函式 ──────────────────────────────────────
    // 在終端機顯示此活動（含索引編號）
    virtual void display(int index) const;

    // ── Getter ────────────────────────────────────────────
    std::string getName()        const { return name; }
    std::string getTime()        const { return time; }
    std::string getNote()        const { return note; }
    bool        getIsCompleted() const { return isCompleted; }

    // ── Setter ────────────────────────────────────────────
    void setName(const std::string& n)  { name = n; }
    void setTime(const std::string& t)  { time = t; }
    void setNote(const std::string& n)  { note = n; }
    void setCompleted(bool c)           { isCompleted = c; }
    void toggleCompleted()              { isCompleted = !isCompleted; }
};
