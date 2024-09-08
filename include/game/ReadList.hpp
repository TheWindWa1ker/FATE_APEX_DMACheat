//#pragma once
//#include <fstream>
//#include <unordered_set>
//#include <string>
//
//struct ReadUid {
//    std::unordered_set<long long int> UIDList;
//    std::unordered_set<long long int> WhiteList;
//    std::unordered_set<long long int> BlackList;
//
//    std::unordered_set<long long int> ReadUIDList(const std::string &file_path) {
//
//        std::ifstream file_stream(file_path);
//        std::string line;
//
//        while (std::getline(file_stream, line)) {
//            // 假设每行的格式是 "ID = XXXXXXX"
//            std::size_t equals_position = line.find("UID = ");
//            if (equals_position != std::string::npos) {
//                std::string IDstring = line.substr(equals_position + 6); // 获取ID部分
//                long long int ID = std::stoll(IDstring); // 将字符串转换为长整型
//                UIDList.insert(ID); // 将ID添加到集合中
//            }
//        }
//
//        return UIDList;
//    }
//
//    std::unordered_set<long long int> ReadWhiteList(const std::string &file_path) {
//
//        std::ifstream file_stream(file_path);
//        std::string line;
//
//        while (std::getline(file_stream, line)) {
//            // 假设每行的格式是 "ID = XXXXXXX"
//            std::size_t equals_position = line.find("UID = ");
//            if (equals_position != std::string::npos) {
//                std::string IDstring = line.substr(equals_position + 6); // 获取ID部分
//                long long int ID = std::stoll(IDstring); // 将字符串转换为长整型
//                WhiteList.insert(ID); // 将ID添加到集合中
//            }
//        }
//
//        return WhiteList;
//    }
//
//    std::unordered_set<long long int> ReadBlackList(const std::string &file_path) {
//
//        std::ifstream file_stream(file_path);
//        std::string line;
//
//        while (std::getline(file_stream, line)) {
//            // 假设每行的格式是 "ID = XXXXXXX"
//            std::size_t equals_position = line.find("UID = ");
//            if (equals_position != std::string::npos) {
//                std::string IDstring = line.substr(equals_position + 6); // 获取ID部分
//                long long int ID = std::stoll(IDstring); // 将字符串转换为长整型
//                BlackList.insert(ID); // 将ID添加到集合中
//            }
//        }
//
//        return BlackList;
//    }
//
//    bool IsAdmin(long long int ID, const std::unordered_set<long long int> &AdminUIDList) {
//        return AdminUIDList.find(ID) != AdminUIDList.end();
//    }
//
//    bool IsWhite(long long int ID, const std::unordered_set<long long int> &WhiteUIDList) {
//        return WhiteUIDList.find(ID) != WhiteUIDList.end();
//    }
//
//    bool IsBlack(long long int ID, const std::unordered_set<long long int> &BlackUIDList) {
//        return BlackUIDList.find(ID) != BlackUIDList.end();
//    }
//
//    void AddUIDToWhitelist(std::string uid) {
//        // 构造INI文件的路径
//        std::string iniFilePath = "Whitelist.ini";
//        // 以追加模式打开文件
//        std::ofstream outfile;
//        outfile.open(iniFilePath, std::ios_base::app);
//        if (outfile.is_open()) {
//            // 添加新的UID到文件
//            outfile << "\n"  << "UID = " << uid;
//            outfile.close();
//        } else {
//            std::cerr << "无法打开文件: " << iniFilePath << std::endl;
//        }
//    }
//    void AddUIDToBlacklist(std::string uid) {
//        std::string iniFilePath = "Blacklist.ini";
//        // 以追加模式打开文件
//        std::ofstream outfile;
//        outfile.open(iniFilePath, std::ios_base::app);
//        if (outfile.is_open()) {
//            // 添加新的UID到文件
//            outfile << "\n"  << "UID = " << uid;
//            outfile.close();
//        } else {
//            std::cerr << "无法打开文件: " << iniFilePath << std::endl;
//        }
//    }
//    void AddUIDToAdminlist(std::string uid) {
//        std::string iniFilePath = "list.ini";
//        // 以追加模式打开文件
//        std::ofstream outfile;
//        outfile.open(iniFilePath, std::ios_base::app);
//        if (outfile.is_open()) {
//            // 添加新的UID到文件
//            outfile << "\n"  << "UID = " << uid;
//            outfile.close();
//        } else {
//            std::cerr << "无法打开文件: " << iniFilePath << std::endl;
//        }
//    }
//
//    void UpdateLists() {
//        ReadUIDList("list.ini");
//        ReadWhiteList("Whitelist.ini");
//        ReadBlackList("Blacklist.ini");
//    }
//};
//
//inline ReadUid RList;

#pragma once
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>

#include "json.hpp"

#pragma execution_character_set("utf-8")

struct ReadUid {

    using json = nlohmann::json;

    std::vector<long long int> adminList;
    std::vector<long long int> whiteList;
    std::vector<long long int> blackList;

    void loadLists() {
        auto loadList = [](const std::string& filename, std::vector<long long int>& list) {
            std::ifstream ifs(filename);
            if (ifs.is_open()) {
                json j = json::parse(ifs);
                auto& items = j["list"];
                for (const auto& item : items) {
                    list.push_back(item["uid"]);
                }
            }
        };

        loadList("list_Admin.json", adminList);
        loadList("list_White.json", whiteList);
        loadList("list_Black.json", blackList);
    }

    bool IsAdmin(long long int UID) {
        return std::find(adminList.begin(), adminList.end(), UID) != adminList.end();
    }

    bool IsWhite(long long int UID) {
        return std::find(whiteList.begin(), whiteList.end(), UID) != whiteList.end();
    }

    bool IsBlack(long long int UID) {
        return std::find(blackList.begin(), blackList.end(), UID) != blackList.end();
    }

    int returnPlayer(long long int UID)
    {
        // 从文件中读取 JSON 数据
        std::ifstream ifs("list.json");
        json j = json::parse(ifs);

        // 提取 list 数组
        auto& list = j["list"];

        // 初始化一个标志，用于指示是否找到 uid
        bool found = false;
        int level = -1;

        // 遍历 list 数组，查找特定的 uid
        for (auto& element : list) {
            long long int uid = element["uid"];
            if (uid == UID) {
                found = true;
                level = element["level"]; // 找到匹配的 uid，保存 level
                break;
            }
        }

        return level;
    }

//    bool IsAdmin(long long int UID) {
//        // 从文件中读取 JSON 数据
//        std::ifstream ifs("list_Admin.json");
//        json j = json::parse(ifs);
//
//        // 提取 list 数组
//        auto& list = j["list"];
//
//        // 初始化一个标志，用于指示是否找到 uid
//        bool found = false;
//
//        // 遍历 list 数组，查找特定的 uid
//        for (auto& element : list) {
//            long long int uid = element["uid"];
//            if (uid == UID) {
//                found = true;
//                break;
//            }
//        }
//
//        return found;
//    }
//
//    bool IsWhite(long long int UID) {
//        // 从文件中读取 JSON 数据
//        std::ifstream ifs("list_White.json");
//        json j = json::parse(ifs);
//
//        // 提取 list 数组
//        auto& list = j["list"];
//
//        // 初始化一个标志，用于指示是否找到 uid
//        bool found = false;
//
//        // 遍历 list 数组，查找特定的 uid
//        for (auto& element : list) {
//            long long int uid = element["uid"];
//            if (uid == UID) {
//                found = true;
//                break;
//            }
//        }
//
//        return found;
//    }
//
//    bool IsBlack(long long int UID) {
//        // 从文件中读取 JSON 数据
//        std::ifstream ifs("list_Black.json");
//        json j = json::parse(ifs);
//
//        // 提取 list 数组
//        auto& list = j["list"];
//
//        // 初始化一个标志，用于指示是否找到 uid
//        bool found = false;
//
//        // 遍历 list 数组，查找特定的 uid
//        for (auto& element : list) {
//            long long int uid = element["uid"];
//            if (uid == UID) {
//                found = true;
//                break;
//            }
//        }
//
//        return found;
//    }

    void AddToList(const std::string& path, const std::string& name, long long int uid, int level) {
        try {
            // 尝试从文件中读取 JSON 数据
            std::ifstream ifs(path);
            json j;
            if (ifs.is_open()) {
                j = json::parse(ifs);
                ifs.close();
            } else {
                // 如果文件不存在或无法打开，创建一个新的 JSON 对象
                j = {{"list", json::array()}};
            }

            // 提取 list 数组
            auto& list = j["list"];

            // 要添加的新元素
            json newElement = {
                    {"name", name},
                    {"uid", uid},
                    {"level", level}
            };

            // 将新元素添加到 list 数组
            list.push_back(newElement);

            // 将修改后的 JSON 对象写回到文件中
            std::ofstream ofs(path);
            if (!ofs.is_open()) {
                throw std::runtime_error("无法打开文件进行写入: " + path);
            }
            ofs << j.dump(4);
        } catch (const std::exception& e) {
            std::cerr << "发生错误: " << e.what() << std::endl;
        }
    }

};
inline ReadUid RList;