#include "../../src/slic3r/GUI/Connect/ConnectHandoff.hpp"
#include <iostream>

int main()
{
    using namespace Slic3r::GUI::ConnectHandoff;
    int checks = 0;
    auto check = [&](bool condition) {
        if (!condition) throw std::runtime_error("URI contract test failed");
        ++checks;
    };
    check(import_uri("/tmp/cube.gcode.3mf", "Cube") ==
        "bambu-connect://import-file?path=%2Ftmp%2Fcube.gcode.3mf&name=Cube&version=1.0.0");
    check(import_uri("C:\\Prints\\test & + #?.gcode.3mf", "Cube & version=9") ==
        "bambu-connect://import-file?path=C%3A%5CPrints%5Ctest%20%26%20%2B%20%23%3F.gcode.3mf&name=Cube%20%26%20version%3D9&version=1.0.0");
    check(encode_component("\xC3\xBC \xE6\xA8\xA1\xE5\x9E\x8B") == "%C3%BC%20%E6%A8%A1%E5%9E%8B");
    check(encode_component("\"\r\n%20") == "%22%0D%0A%2520");
    check(encode_component("AZaz09-_.!~*'()") == "AZaz09-_.!~*'()");
    check(import_uri("D:/plate.gcode.3mf", "Cube").find("path=D%3A%2Fplate.gcode.3mf") != std::string::npos);
    for (const auto& path : {std::string(), std::string("relative.gcode.3mf"),
            std::string("C:relative.gcode.3mf"), std::string("C:\\bad\0other", 12)}) {
        bool rejected = false;
        try { import_uri(path, "Cube"); } catch (const std::invalid_argument&) { rejected = true; }
        check(rejected);
    }
    bool rejected = false;
    try { import_uri("C:/plate.gcode.3mf", ""); } catch (const std::invalid_argument&) { rejected = true; }
    check(rejected);
    std::cout << checks << " PASS: public URI contract, Unicode, reserved characters, invalid paths\n";
}
