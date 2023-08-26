#include <csignal>
#include <gperftools/profiler.h>

#include "HylChan.h"
#include "ServerParams.h"

void sigusrHandler(int signum) {
    static std::atomic<bool> isStarted = false;
    if (signum == SIGUSR1) {
        if (!isStarted) {
            isStarted = true;
            ProfilerStart("test.prof");
            std::cout << "profile start" << std::endl;
        }
        else {
            ProfilerStop();
            std::cout << "profile over" << std::endl;
        }
    }
}

int main() {
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR || signal(SIGUSR1, sigusrHandler) == SIG_ERR) {
//        perror("signal");
//        std::cerr << "Failed in register sigpipe handler" << std::endl;
        exit(EXIT_FAILURE);
    }

    params.setEtOn(true);
    params.setThreadCount(4);

    std::unique_ptr<HttpServer> pS = std::make_unique<HylChan>();
    pS->start(40960);

    std::cout << "Server down" << std::endl;

    return 0;
}
