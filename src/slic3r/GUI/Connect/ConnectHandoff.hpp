#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace Slic3r::GUI::ConnectHandoff {

// Bambu's public import-file contract uses JavaScript encodeURIComponent over
// UTF-8 bytes. This function builds data only and never invokes a command shell.
inline std::string encode_component(std::string_view value)
{
    static constexpr char hex[] = "0123456789ABCDEF";
    std::string encoded;
    for (unsigned char c : value) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
            c == '!' || c == '~' || c == '*' || c == '\'' || c == '(' || c == ')') {
            encoded += static_cast<char>(c);
        } else {
            encoded += '%';
            encoded += hex[c >> 4];
            encoded += hex[c & 15];
        }
    }
    return encoded;
}

inline std::string import_uri(std::string_view absolute_path, std::string_view display_name)
{
    const bool drive_path = absolute_path.size() >= 3 &&
        ((absolute_path[0] >= 'A' && absolute_path[0] <= 'Z') ||
         (absolute_path[0] >= 'a' && absolute_path[0] <= 'z')) &&
        absolute_path[1] == ':' && (absolute_path[2] == '\\' || absolute_path[2] == '/');
    if ((!drive_path && (absolute_path.empty() || absolute_path[0] != '/')) ||
        display_name.empty() || absolute_path.find('\0') != std::string_view::npos ||
        display_name.find('\0') != std::string_view::npos)
        throw std::invalid_argument("An absolute local path and display name are required");
    return "bambu-connect://import-file?path=" + encode_component(absolute_path) +
        "&name=" + encode_component(display_name) + "&version=1.0.0";
}

} // namespace Slic3r::GUI::ConnectHandoff
