#include <signal.h>
#include "connection.h"
#include "http_connection.h"
#include "easylogging++.h"
#include "server.h"

INITIALIZE_EASYLOGGINGPP

static thanos::Server http_server;

void handle_sigpipe(int snum) {
    return;
}

void handle_termsig(int snum) {
    http_server.ask_to_quit();
}

int main() {
    el::Configurations conf("../conf/easylog.conf");
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::reconfigureAllLoggers(conf);

    signal(SIGPIPE, handle_sigpipe);
    signal(SIGABRT, handle_termsig);
    signal(SIGINT, handle_termsig);
    signal(SIGTERM, handle_termsig);

    if (!http_server.init("../conf/thanos.conf")) {
        LOG(FATAL) << "[main]: server init failed";
    }
    if (!http_server.run()) {
        LOG(FATAL) << "[main]: server run failed";
    }
    if (!http_server.uninit()) {
        LOG(FATAL) << "[main]: server uninit failed";
    }
    return 0;
}
