#ifndef REQUESTSTUTTER_HPP
#define REQUESTSTUTTER_HPP

class RequestStutter {
protected:
    static long long lastRequestTime;

public:
    static long long getRequestTime();
};

#endif
