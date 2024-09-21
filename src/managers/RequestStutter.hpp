#ifndef REQUESTSTUTTER_HPP
#define REQUESTSTUTTER_HPP

class RequestStutter {
    static long long lastRequestTime;
    static long long lastDownloadLevelTime;
    static long long getDownloadLevelTime();

public:
    static long long getRequestTime(bool downloadLevel);
};

#endif
