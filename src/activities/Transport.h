#pragma once
#include "Activity.h"

// ============================================================
//  Transport — 交通（繼承自 Activity）
// ============================================================
class Transport : public Activity {
private:
    std::string from;          // 出發地
    std::string to;            // 目的地
    std::string transportType; // 交通方式（飛機/新幹線/巴士...）

public:
    Transport(const std::string& name,
              const std::string& time,
              const std::string& note          = "",
              const std::string& from          = "",
              const std::string& to            = "",
              const std::string& transportType = "");

    // ── 純虛函式實作 ──────────────────────────────────────
    std::string getType()      const override { return "Transport"; }
    std::string getTypeIcon()  const override { return "🚌"; }
    std::string getTypeLabel() const override { return "交通"; }
    std::string getDetails()   const override;
    std::string serialize()    const override;
    Activity*   clone()        const override { return new Transport(*this); }

    // ── 額外 Getter / Setter ──────────────────────────────
    std::string getFrom()          const { return from; }
    std::string getTo()            const { return to; }
    std::string getTransportType() const { return transportType; }
    void setFrom(const std::string& f)          { from = f; }
    void setTo  (const std::string& t)          { to   = t; }
    void setTransportType(const std::string& tt){ transportType = tt; }
};
