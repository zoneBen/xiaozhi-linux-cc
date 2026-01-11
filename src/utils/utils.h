#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

namespace xiaozhi {

// 辅助函数：扩展波浪号路径
inline std::string expandHomeDirectory(const std::string& path) {
    if (!path.empty() && path[0] == '~') {
        const char* home = getenv("HOME");
        if (!home) {
            struct passwd* pw = getpwuid(getuid());
            home = pw ? pw->pw_dir : "/tmp";
        }
        return std::string(home) + path.substr(1);
    }
    return path;
}

}

#endif // UTILS_H