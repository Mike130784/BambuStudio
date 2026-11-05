// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../src/slic3r/GUI/Parametric/CustomizerParameters.hpp"
#include <fstream>
#include <iostream>
#include <limits>
using namespace Slic3r::GUI::Parametric;
int main(int argc, char **argv) {
    int checks = 0;
    auto check = [&](bool ok) { ++checks; if (!ok) throw std::runtime_error("Test failed " + std::to_string(checks)); };
    auto reject = [&](const std::string &s) { bool failed=false; try { read_customizer_parameters(s); } catch (const CustomizerError &e) { failed=e.line>0; } check(failed); };
    try {
        auto p=read_customizer_parameters("\xEF\xBB\xBF/* [Maten] */\r\n// Width\r\nw = 20; // [1:1:200]\r\n/* [Hidden] */\nx = w / 2;\nmodule foo() { local = 4; }\n");
        check(p.size()==1 && p[0].name=="w" && p[0].label=="Width" && p[0].step==1);
        p=read_customizer_parameters("/* multiline\nw = 2; // [1:3]\n*/\n/* [Hidden] */\nh = 2;\n/* [Visible] */\nx = -0.5; // [-1:0.1:1]\ncube(1);\n");
        check(p.size()==1 && p[0].initial==-0.5);
        check(read_customizer_parameters("n = 1e1; // [1:2e1]\ncube(n);").size()==1);
        reject(""); reject("n = true; // [0:1]\ncube(1);");
        reject("n = 1 + 2; // [0:10]\ncube(n);");
        reject("n = 2;\ncube(n);"); reject("n = 2; // [1,2,3]\ncube(n);");
        reject("n = 2; // [3:1]\ncube(n);"); reject("n = 2; // [0:0:3]\ncube(n);");
        reject("n = 2; // [0:-1:3]\ncube(n);"); reject("n = 2.5; // [0:1:3]\ncube(n);");
        reject("n = 1e999; // [0:10]\ncube(n);"); reject("n = 2; // [0:1e7]\ncube(n);");
        reject("n = 2; // [0:3]\nn = 1; // [0:3]\ncube(n);");
        reject("n = 2; // [0:3]\n/* unterminated");
        reject("s = \"fake // [0:3]\"; // [0:3]\ncube(1);");
        reject(std::string(4097,' ') + "\nn = 1; // [0:2]");
        std::string many; for(int i=0;i<33;++i) many += "n"+std::to_string(i)+" = 1; // [0:2]\n"; reject(many);
        check(on_parameter_step(1.3, 1, .1)); check(!on_parameter_step(1.35,1,.1));
        check(!on_parameter_step(std::numeric_limits<double>::infinity(),0,1));
        for (int i=1;i<argc;++i) {
            std::ifstream f(argv[i],std::ios::binary); if(!f) throw std::runtime_error("Cannot open fixture");
            std::string s((std::istreambuf_iterator<char>(f)),{}); p=read_customizer_parameters(s); check(p.size()==3);
            std::cout << "fixture " << i << ": " << p.size() << " parameters PASS\n";
        }
        std::cout << checks << " checks PASS\n"; return 0;
    } catch(const std::exception &e) { std::cerr << e.what() << '\n'; return 1; }
}
