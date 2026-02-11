#include <gtest/gtest.h>
#include "Logging/Logger.h"
#include "Logging/LoggerMacros.h"
#include <spdlog/sinks/base_sink.h>
#include <spdlog/fmt/fmt.h>

using namespace Utils::Logging;

// A thread-safe sink for testing that captures log output globally or per instance
template<typename Mutex>
class TestSink : public spdlog::sinks::base_sink<Mutex> {
public:
    std::string log_contents;
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        log_contents += fmt::to_string(formatted);
    }
    void flush_() override {}
};

using TestSink_mt = TestSink<std::mutex>;

class LoggerTest : public ::testing::Test {
   protected:
    static std::shared_ptr<LoggerConfig> createTestConfig() {
        auto c = std::make_shared<LoggerConfig>();
        c->globalLogLevel = LogLevel::INFO;
        c->filename = "test_log.txt";
        return c;
    }

    LoggerTest() : config(createTestConfig()), m_logger("TestLogger", config), testSink(std::make_shared<TestSink_mt>()) {
        m_logger.clearSinks();
        m_logger.addSink(testSink);
        testSink->set_level(spdlog::level::trace); 
    }

    std::shared_ptr<LoggerConfig> config;
    Logger m_logger;
    std::shared_ptr<TestSink_mt> testSink;
};

TEST_F(LoggerTest, MacrosExecute) {
    LOG_D("Debug message");
    LOG_I("Info message");
    LOG_W("Warning message");
    LOG_E("Error message");
    LOG_C("Critical message");

    EXPECT_FALSE(testSink->log_contents.find("Debug message") != std::string::npos);
    EXPECT_TRUE(testSink->log_contents.find("Info message") != std::string::npos);
    EXPECT_TRUE(testSink->log_contents.find("Warning message") != std::string::npos);
    EXPECT_TRUE(testSink->log_contents.find("Error message") != std::string::npos);
    EXPECT_TRUE(testSink->log_contents.find("Critical message") != std::string::npos);
}

TEST_F(LoggerTest, FormattingWorks) {
    LOG_I("String: {}, Int: {}, Float: {:.2f}", "test", 42, 3.14159);
    
    EXPECT_TRUE(testSink->log_contents.find("String: test, Int: 42, Float: 3.14") != std::string::npos);
}

TEST_F(LoggerTest, ConfigUpdateChangesLevel) {
    LOG_D("Debug message");
    EXPECT_FALSE(testSink->log_contents.find("Debug message") != std::string::npos);

    auto newConfig = std::make_shared<LoggerConfig>();
    newConfig->globalLogLevel = LogLevel::DEBUG;
    m_logger.onUpdate(newConfig);

    LOG_D("Debug message");
    EXPECT_TRUE(testSink->log_contents.find("Debug message") != std::string::npos);
}

TEST_F(LoggerTest, NamedLoggerOverride) {
    LOG_D("Debug message");
    EXPECT_FALSE(testSink->log_contents.find("Debug message") != std::string::npos);

    config->loggersLogLevels[m_logger.getName()] = LogLevel::DEBUG;
    m_logger.onUpdate(config);
    
    LOG_D("Debug message");
    EXPECT_TRUE(testSink->log_contents.find("Debug message") != std::string::npos);
}

TEST_F(LoggerTest, FlushDoesNotThrow) {
    EXPECT_NO_THROW(m_logger.flush());
}

TEST_F(LoggerTest, LogLevelFiltering) {
    auto newConfig = std::make_shared<LoggerConfig>();
    newConfig->globalLogLevel = LogLevel::INFO;
    m_logger.onUpdate(newConfig);

    LOG_D("Debug message that should be skipped");
    LOG_I("Info message that should appear");
    m_logger.flush();

    EXPECT_FALSE(testSink->log_contents.find("Debug message") != std::string::npos);
    EXPECT_TRUE(testSink->log_contents.find("Info message") != std::string::npos);
}
