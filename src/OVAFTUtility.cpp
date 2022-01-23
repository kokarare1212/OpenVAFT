#include "OVAFTUtility.h"

#include <string>
#include <whereami.h>

std::string OVAFTUtility::basename(const std::string &path) {
    // Windows Style
    long winPos(static_cast<long>(path.find_last_of('\\')));
    if (winPos != std::string::npos) return path.substr(winPos + 1);

    // Unix Style
    long unixPos(static_cast<long>(path.find_last_of('/')));
    if (unixPos != std::string::npos) return path.substr(unixPos + 1);

    // Unknown Style
    return path;
}

std::string OVAFTUtility::dirname(const std::string &path) {
    // Windows Style
    long winPos(static_cast<long>(path.find_last_of('\\')));
    if (winPos != std::string::npos) return path.substr(0, winPos + 1);

    // Unix Style
    long unixPos(static_cast<long>(path.find_last_of('/')));
    if (unixPos != std::string::npos) return path.substr(0, unixPos + 1);

    // Unknown Style
    return path;
}

std::string OVAFTUtility::getExecutablePath() {
    int length = wai_getExecutablePath(nullptr, 0, nullptr);
    char *pathChar = static_cast<char *>(malloc(length + 1));
    wai_getExecutablePath(pathChar, length, nullptr);
    pathChar[length] = '\0';
    std::string pathStr(pathChar);
    return pathStr;
}
