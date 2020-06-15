#include <cest/cest.h>

#include <chromos/logging/LoggerInConsole.h>

using namespace cest;


describe("chromos-logging", []() {
    it("allows logging to console", []() {
        LoggerInConsole logger;
        logger.info("info message");
        logger.warn("warning message");
        logger.error("error message");
    });
});
