#ifndef FILE_H
#define FILE_H
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class File {
private:
    std::string filename;

public:
    File();

    File(std::string filename);

    std::string ReadFile();

    bool SearchInFile(std::string text);

    void WriteToFile(std::string text);

    void AppendToFile(std::string text);

    void ReplaceInFile(std::string old_text, std::string new_text);

    std::vector<std::string> ListDir(const std::filesystem::path &path);
};

extern File file;


#endif
