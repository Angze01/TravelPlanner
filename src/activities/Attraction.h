#pragma once
#include "Activity.h"

// ============================================================
//  Attraction — 景點（繼承自 Activity）
// ============================================================
class Attraction : public Activity {
private:
    std::string ticketPrice;  // 門票費用

public:
    Attraction(const std::string& name,
               const std::string& time,
               const std::string& note        = "",
               const std::string& ticketPrice = "",
               double             cost        = 0.0);

    // ── 純虛函式實作 ──────────────────────────────────────
    std::string getType()      const override { return "Attraction"; }
    std::string getTypeIcon()  const override { return "🏛"; }
    std::string getTypeLabel() const override { return "景點"; }
    std::string getDetails()   const override;
    std::string serialize()    const override;
    Activity*   clone()        const override { return new Attraction(*this); }

    // ── 額外 Getter / Setter ──────────────────────────────
    std::string getTicketPrice() const            { return ticketPrice; }
    void        setTicketPrice(const std::string& tp) { ticketPrice = tp; }
};
