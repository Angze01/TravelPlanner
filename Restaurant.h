#pragma once
#include "Activity.h"

// ============================================================
//  Restaurant — 餐廳（繼承自 Activity）
// ============================================================
class Restaurant : public Activity {
private:
    std::string cuisine;   // 料理類型（e.g. 壽司、拉麵）

public:
    Restaurant(const std::string& name,
               const std::string& time,
               const std::string& note    = "",
               const std::string& cuisine = "");

    // ── 純虛函式實作 ──────────────────────────────────────
    std::string getType()      const override { return "Restaurant"; }
    std::string getTypeIcon()  const override { return "🍽"; }
    std::string getTypeLabel() const override { return "餐廳"; }
    std::string getDetails()   const override;
    std::string serialize()    const override;
    Activity*   clone()        const override { return new Restaurant(*this); }

    // ── 額外 Getter / Setter ──────────────────────────────
    std::string getCuisine() const                  { return cuisine; }
    void        setCuisine(const std::string& c)    { cuisine = c; }
};
