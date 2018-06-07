#include <iostream>
#include <signal.h>
#include "connection.h"
#include "easylogging++.h"
#include "server.h"

namespace thanos {

class EchoConnection : public Connection {
public:
    void process() override {
        Buffer tmp;
        if (!_dump_read(&tmp)) {
            LOG(WARNING) << "[EchoConnection::process]:dump read_buffer failed";
            return;
        }
        if (!_dump_to_write(tmp)) {
            LOG(WARNING) << "[EchoConnection::process]:dump to write_buffer failed";
            return;
        }
        if (!_process_done()) {
            LOG(WARNING) << "[EchoConnection::process]:_process_done";
            return;
        }
    }

protected:
    EchoConnection(int dummy) {}

    Connection* _clone() override {
        return new EchoConnection(0);
    }

    bool _clear() override {}

private:
    static EchoConnection _echo_connection;

    EchoConnection() {
        _add_prototype(this);
    }
};

// register prototype to Connection
EchoConnection EchoConnection::_echo_connection;

}

INITIALIZE_EASYLOGGINGPP

static thanos::Server echo_server;

void handle_sigpipe(int snum) {
    return;
}

void handle_termsig(int snum) {
    echo_server.ask_to_quit();
}

int main() {
    el::Configurations conf("../conf/easylog.conf");
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::reconfigureAllLoggers(conf);

    signal(SIGPIPE, handle_sigpipe);
    signal(SIGABRT, handle_termsig);
    signal(SIGINT, handle_termsig);
    signal(SIGTERM, handle_termsig);

    if (!echo_server.init("../conf/thanos.conf")) {
        LOG(FATAL) << "[main]: echo_server init failed";
    }
    if (!echo_server.run()) {
        LOG(FATAL) << "[main]: server run failed";
    }
    if (!echo_server.uninit()) {
        LOG(FATAL) << "[main]: server uninit failed";
    }

    return 0;
}
