/**
 * @file logging.cpp
 */
#include "logging.h"
#include <atomic>
#include <cctype>
#include <iostream>
#include <mutex>
#include <unordered_set>

namespace OFIQ_LIB::logging {

static std::atomic<LogLevel> g_level{LogLevel::Off};
static std::unordered_set<std::string> g_categories; // lowercased
static std::mutex g_mutex;

static inline std::string toLower(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

void setLevel(LogLevel level) { g_level.store(level, std::memory_order_relaxed); }

LogLevel getLevel() { return g_level.load(std::memory_order_relaxed); }

void setCategories(const std::vector<std::string>& cats) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_categories.clear();
    for (auto c : cats) g_categories.insert(toLower(std::move(c)));
}

bool parseLevel(const std::string& s, LogLevel& out) {
    auto l = toLower(s);
    if (l == "off" || l == "none") { out = LogLevel::Off; return true; }
    if (l == "error" || l == "err") { out = LogLevel::Error; return true; }
    if (l == "warn" || l == "warning") { out = LogLevel::Warn; return true; }
    if (l == "info") { out = LogLevel::Info; return true; }
    if (l == "debug" || l == "dbg") { out = LogLevel::Debug; return true; }
    return false;
}

void log(LogLevel level, std::string_view msg, std::string_view category) {
    // Level filter
    if (static_cast<int>(level) > static_cast<int>(g_level.load(std::memory_order_relaxed))) return;

    // Category filter (if configured)
    if (!g_categories.empty()) {
        if (category.empty()) return; // category required when filter set
        std::string lc = toLower(std::string(category));
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_categories.find(lc) == g_categories.end()) return;
    }

    // Output stream by level
    std::ostream* os = &std::cout;
    if (level == LogLevel::Error || level == LogLevel::Warn) os = &std::cerr;

    if (!category.empty()) (*os) << "[" << category << "] ";
    (*os) << msg;
    if (msg.empty() || msg.back() != '\n') (*os) << '\n';
}

} // namespace OFIQ_LIB::logging

