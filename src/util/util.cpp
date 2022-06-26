#include "util.h"

namespace util
{
    std::string currentTime(std::chrono::time_point<std::chrono::system_clock> now)
    {
        // you need to get milliseconds explicitly
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                                now.time_since_epoch()) %
                            1000;
        // and that's a "normal" point of time with seconds
        auto timeNow = std::chrono::system_clock::to_time_t(now);

        std::ostringstream currentTimeStream;
        currentTimeStream << std::put_time(localtime(&timeNow), "%H:%M:%S")
                          << "." << std::setfill('0') << std::setw(3) << milliseconds.count();

        return currentTimeStream.str();
    }

    std::string readFile(std::string fileName)
    {
        std::ifstream ifs(fileName, std::ios::in);

        if (!ifs.is_open())
        {
            std::cerr << "Could not read file " << fileName << ". File does not exist." << std::endl;
            return "";
        }

        std::string content((std::istreambuf_iterator<char>(ifs)),
                            (std::istreambuf_iterator<char>()));

        ifs.close();
        return content;
    }
}