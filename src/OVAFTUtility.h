#pragma once

#include <string>

class OVAFTUtility {
public:
    static std::string basename(const std::string &path);

    static std::string dirname(const std::string &path);

    static std::string getExecutablePath();
};
