#include <iostream>
#include <unistd.h>
#include "threadpool.h"
#include "easylogging++.h"


class Handler {
public:
    void process() {
        std::cout << "hello world" << std::endl;
    }
};

INITIALIZE_EASYLOGGINGPP

int main() {
    el::Configurations conf("../conf/easylog.conf");
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::reconfigureAllLoggers(conf);

    thanos::ThreadPool<Handler> tp;
    if (tp.init(12, 1024) == false) {
        std::cout << "tp.init failed" << std::endl;
        return 0;
    }

    Handler handler[12];

    for (int i = 0; i < 12; ++i) {
        tp.append(&handler[i]);
    }
    sleep(3);
    return 0;
}
