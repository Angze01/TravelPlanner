#pragma once
#include <string>
#include "Trip.h"

// ============================================================
//  FileManager — 旅遊手冊讀寫管理員
//  檔案格式（.travel）：
//
//  TRIP|旅程名稱|目的地|天數|出發日期
//  DAY|1|2024-03-15
//  ACT|Attraction|名稱|時間|備注|完成|票價
//  ACT|Restaurant|名稱|時間|備注|完成|料理
//  ACT|Hotel|名稱|時間|備注|完成|入住|退房
//  ACT|Transport|名稱|時間|備注|完成|出發|到達|類型
//  DAY|2|...
//  END
// ============================================================
class FileManager {
public:
    // 儲存旅程到檔案，回傳是否成功
    static bool save(const Trip& trip, const std::string& filename);

    // 從檔案載入旅程（成功回傳 new Trip*，失敗回 nullptr）
    // 呼叫者負責 delete
    static Trip* load(const std::string& filename);

    // 取得儲存目錄中所有 .travel 檔案列表
    static std::vector<std::string> listSaveFiles(const std::string& dir = "saves");

    // 確保儲存目錄存在
    static bool ensureSaveDir(const std::string& dir = "saves");

private:
    // 以 '|' 分割字串，回傳各欄位
    static std::vector<std::string> split(const std::string& line, char delim = '|');

    // 由序列化字串重建 Activity 物件
    static Activity* deserializeActivity(const std::string& line);
};
