#pragma once
#include "Activity.h"

// ============================================================
//  Hotel — 住宿（繼承自 Activity）
// ============================================================
class Hotel : public Activity {
private:
    std::string checkIn;   // 入住時間
    std::string checkOut;  // 退房時間

public:
    Hotel(const std::string& name,
          const std::string& time,
          const std::string& note     = "",
          const std::string& checkIn  = "",
          const std::string& checkOut = "");

    // ── 純虛函式實作 ──────────────────────────────────────
    std::string getType()      const override { return "Hotel"; }
    std::string getTypeIcon()  const override { return "🏨"; }
    std::string getTypeLabel() const override { return "住宿"; }
    std::string getDetails()   const override;
    std::string serialize()    const override;
    Activity*   clone()        const override { return new Hotel(*this); }

    // ── 額外 Getter / Setter ──────────────────────────────
    std::string getCheckIn()  const { return checkIn; }
    std::string getCheckOut() const { return checkOut; }
    void setCheckIn (const std::string& t) { checkIn  = t; }
    void setCheckOut(const std::string& t) { checkOut = t; }
};
