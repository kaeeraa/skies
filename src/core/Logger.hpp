#pragma once
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

class Logger {
  Logger()
  {
    auto formatter = boost::log::parse_formatter<char>(
      "[%TimeStamp%] [%Severity%] %Message%");

    boost::log::add_console_log(
      std::cout,
      boost::log::keywords::format = formatter);

    boost::log::add_common_attributes();
    boost::log::core::get()->set_filter(
      boost::log::trivial::severity >= boost::log::trivial::trace);
  };

  public:
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  // singleton
  inline static Logger& instance()
  {
    static Logger instance;
    return instance;
  }

  void trace(const std::string_view msg) const { BOOST_LOG_TRIVIAL(trace) << msg; }
  void debug(const std::string_view msg) const { BOOST_LOG_TRIVIAL(debug) << msg; }
  void info(const std::string_view msg) const { BOOST_LOG_TRIVIAL(info) << msg; }
  void warn(const std::string_view msg) const { BOOST_LOG_TRIVIAL(warning) << msg; }
  void error(const std::string_view msg) const { BOOST_LOG_TRIVIAL(error) << msg; }
  void fatal(const std::string_view msg) const { BOOST_LOG_TRIVIAL(fatal) << msg; }
};