//
// Created by koncord on 24.01.16.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <vector>

#if (defined __WIN32__ || defined _WIN32 || defined WIN32)
#define __WINDOWS
#endif

#ifdef __WINDOWS
int setenv(const char *name, const char *value, int overwrite);
#endif

namespace Utils
{
    std::string convertPath(std::string str);

    void timestamp();

    int progressFunc(double TotalToDownload, double NowDownloaded);

    bool compareDoubles(double a, double b, double epsilon);

    bool vectorContains(std::vector<int>* vectorChecked, int value);
    bool vectorContains(std::vector<std::string>* vectorChecked, std::string value);

    std::string replaceString(const std::string &source, const char *find, const char *replace);

    std::string toString(int num);

    std::string &removeExtension(std::string &file);

    long int getFileLength(const char *file);

    unsigned int crc32Checksum(const std::string &file);

    void printVersion(std::string appName, std::string version, std::string commitHash, int protocol);

    void printWithWidth(std::ostringstream &sstr, std::string str, size_t width);
    std::string intToHexStr(unsigned val);
}
#endif //UTILS_HPP
