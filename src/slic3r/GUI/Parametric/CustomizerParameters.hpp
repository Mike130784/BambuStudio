// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <algorithm>
#include <cmath>
#include <locale>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Slic3r::GUI::Parametric {
struct CustomizerParameter {
    std::string name, label;
    double initial, minimum, maximum, step;
};
struct CustomizerError : std::runtime_error {
    size_t line;
    explicit CustomizerError(size_t n) : std::runtime_error("Unsupported Customizer header"), line(n) {}
};
inline bool on_parameter_step(double value, double low, double step) {
    if (step == 0) return true;
    const double ticks = (value - low) / step;
    return std::isfinite(ticks) && std::abs(ticks - std::round(ticks)) <= 1e-7;
}
// Independent, deliberately small reader: literal numeric declarations in the
// initial header only. Never executes SCAD or guesses limits for unbounded data.
inline std::vector<CustomizerParameter> read_customizer_parameters(std::string source) {
    if (source.size() > 2 * 1024 * 1024) throw CustomizerError(1);
    if (source.compare(0, 3, "\xEF\xBB\xBF") == 0) source.erase(0, 3);
    const std::string number = R"([+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?[0-9]+)?)";
    const std::regex declaration("^\\s*([A-Za-z_][A-Za-z0-9_]{0,63})\\s*=\\s*(" + number + ")\\s*;\\s*$");
    const std::regex range("^\\s*\\[\\s*(" + number + ")\\s*:\\s*(" + number + ")(?:\\s*:\\s*(" + number + "))?\\s*\\]\\s*$");
    const std::regex group(R"(^\s*/\*\s*\[([^\]\r\n]+)\]\s*\*/\s*$)");
    const std::regex assignment(R"(^\s*\$?[A-Za-z_][A-Za-z0-9_]*\s*=)");
    auto trim = [](const std::string &s) {
        auto first = s.find_first_not_of(" \t\r\n");
        return first == std::string::npos ? std::string() : s.substr(first, s.find_last_not_of(" \t\r\n") - first + 1);
    };
    auto parse = [](const std::string &s, size_t line) {
        std::istringstream stream(s); stream.imbue(std::locale::classic());
        double value; stream >> value;
        if (!stream || !std::isfinite(value) || std::abs(value) > 1e6) throw CustomizerError(line);
        return value;
    };
    std::vector<CustomizerParameter> result;
    std::vector<std::string> names;
    std::istringstream lines(source);
    std::string raw, description;
    bool block = false, hidden = false;
    size_t line = 0;
    while (std::getline(lines, raw)) {
        ++line;
        if (raw.size() > 4096) throw CustomizerError(line);
        std::smatch match;
        if (!block && std::regex_match(raw, match, group)) {
            hidden = trim(match[1]) == "Hidden";
            description.clear();
            continue;
        }
        std::string code, comment;
        bool quoted = false, escaped = false;
        for (size_t i = 0; i < raw.size(); ++i) {
            char c = raw[i], next = i + 1 < raw.size() ? raw[i + 1] : '\0';
            if (block) { if (c == '*' && next == '/') { block = false; ++i; } continue; }
            if (!quoted && c == '/' && next == '*') { block = true; code += ' '; ++i; continue; }
            if (!quoted && c == '/' && next == '/') { comment = trim(raw.substr(i + 2)); break; }
            code += c;
            if (c == '"' && !escaped) quoted = !quoted;
            if (quoted && c == '\\' && !escaped) escaped = true; else escaped = false;
        }
        code = trim(code);
        if (code.empty()) {
            description = comment.size() <= 120 ? comment : std::string();
            continue;
        }
        if (block || quoted) throw CustomizerError(line);
        if (!std::regex_search(code, assignment)) break; // Geometry/module/function starts here.
        // Hidden/internal declarations are not editable. Keep them in the source.
        if (hidden || code[0] == '$') { description.clear(); continue; }
        if (!std::regex_match(code, match, declaration)) throw CustomizerError(line);
        const std::string name = match[1];
        const double initial = parse(match[2], line);
        if (std::find(names.begin(), names.end(), name) != names.end()) throw CustomizerError(line);
        names.push_back(name);
        if (!std::regex_match(comment, match, range)) throw CustomizerError(line);
        double low = parse(match[1], line), high = parse(match[2], line), step = 0;
        if (match[3].matched) { step = high; high = parse(match[3], line); if (step <= 0) throw CustomizerError(line); }
        if (low > high || initial < low || initial > high || !on_parameter_step(initial, low, step)) throw CustomizerError(line);
        result.push_back({name, description.empty() ? name : description, initial, low, high, step});
        if (result.size() > 32) throw CustomizerError(line);
        description.clear();
    }
    if (block || result.empty()) throw CustomizerError(line ? line : 1);
    return result;
}
}
