/**
 * @file logging.h
 */
#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace OFIQ_LIB::logging {

enum class LogLevel { Off = 0, Error, Warn, Info, Debug };

// Configure logging level
void setLevel(LogLevel level);
LogLevel getLevel();

// Configure allowed categories; empty = allow all
void setCategories(const std::vector<std::string>& cats);

// Parse level from string (case-insensitive); returns false if unknown
bool parseLevel(const std::string& s, LogLevel& out);

// Log a message with optional category (nullptr or empty = uncategorized)
void log(LogLevel level, std::string_view msg, std::string_view category = {});

} // namespace OFIQ_LIB::logging

