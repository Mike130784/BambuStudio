// Development-only executable. Exercises the same native runner used by the dialog.
#include "../../src/slic3r/GUI/Parametric/WindowsRenderJob.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <chrono>
using namespace Slic3r::GUI::Parametric;
namespace fs = std::filesystem;
void require(bool value, const char *message) { if (!value) throw std::runtime_error(message); }
void write_pid(const fs::path &path) { std::ofstream(path) << GetCurrentProcessId(); }
DWORD pid(const fs::path &path) { DWORD value=0; std::ifstream(path) >> value; return value; }
int wmain(int argc, wchar_t **argv)
{
    try {
        if (argc > 1 && std::wstring(argv[1]) == L"--check-argument") {
            require(argc == 3 && std::wstring(argv[2]) == L"quotes \" and slash\\end\\", "Argument quoting changed payload");
            return 0;
        }
        if (argc > 1 && std::wstring(argv[1]) == L"--leaf") {
            write_pid(argv[2]); Sleep(300000); return 0;
        }
        if (argc > 1 && std::wstring(argv[1]) == L"--worker") {
            write_pid(argv[2]);
            std::wstring cmd=quote_argument(argv[0])+L" --leaf "+quote_argument(argv[3]);
            STARTUPINFOW si{}; si.cb=sizeof(si); PROCESS_INFORMATION pi{};
            require(CreateProcessW(argv[0],cmd.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&si,&pi),"Cannot start test grandchild");
            CloseHandle(pi.hThread); CloseHandle(pi.hProcess); Sleep(300000); return 0;
        }
        require(argc == 5,"Usage: test-render-job.exe openscad.exe source.scad params.json new-test-dir");
        fs::path dir=fs::absolute(argv[4]); require(fs::create_directories(dir),"Use a fresh test directory");
        auto log=dir/L"render.log", stl=dir/L"result.stl";
        WindowsRenderJob render;
        render.start(argv[1],{L"--export-format",L"binstl",L"-o",stl.wstring(),L"-p",argv[3],L"-P",L"custom",argv[2]},dir.wstring(),log.wstring());
        DWORD code=0; auto began=std::chrono::steady_clock::now();
        while (!render.finished(code)) {
            require(std::chrono::steady_clock::now()-began<std::chrono::seconds(60),"Render timeout"); Sleep(20);
        }
        render.stop(); require(code==0 && fs::file_size(stl)>84,"Real OpenSCAD render failed");
        std::cout << "PASS real OpenSCAD render\n";
        render.start(fs::absolute(argv[0]).wstring(),{L"--check-argument",L"quotes \" and slash\\end\\"},dir.wstring(),(dir/L"arguments.log").wstring());
        while (!render.finished(code)) Sleep(20);
        render.stop(); require(code==0,"Quoted arguments must be preserved");
        std::cout << "PASS quotes and trailing backslash without shell\n";
        for (const auto &entry : {std::pair<const wchar_t*,const char*>(L"syntax.scad","cube([;"),
                                  std::pair<const wchar_t*,const char*>(L"empty.scad","// empty model\n")}) {
            auto broken=dir/entry.first, result=dir/(std::wstring(entry.first)+L".stl");
            std::ofstream(broken)<<entry.second;
            render.start(argv[1],{L"-o",result.wstring(),broken.wstring()},dir.wstring(),(dir/(std::wstring(entry.first)+L".log")).wstring());
            began=std::chrono::steady_clock::now();
            while (!render.finished(code)) { require(std::chrono::steady_clock::now()-began<std::chrono::seconds(60),"Failure test timed out"); Sleep(20); }
            render.stop(); require(code!=0 && !fs::exists(result),"Failed render must not leave usable/stale output");
        }
        std::cout << "PASS syntax error and empty geometry leave no output\n";
        bool rejected=false;
        try { render.start((dir/L"missing.exe").wstring(),{},dir.wstring(),(dir/L"missing.log").wstring()); }
        catch(const std::exception&) { rejected=true; }
        require(rejected,"Missing renderer must fail"); std::cout << "PASS missing renderer\n";
        auto parent_pid=dir/L"parent.pid", child_pid=dir/L"child.pid";
        render.start(fs::absolute(argv[0]).wstring(),{L"--worker",parent_pid.wstring(),child_pid.wstring()},dir.wstring(),(dir/L"cancel.log").wstring());
        for(int i=0;i<250 && (!pid(parent_pid) || !pid(child_pid));++i) Sleep(20);
        HANDLE parent=OpenProcess(SYNCHRONIZE,FALSE,pid(parent_pid)), child=OpenProcess(SYNCHRONIZE,FALSE,pid(child_pid));
        require(parent && child,"Both owned test processes must exist");
        render.stop();
        require(WaitForSingleObject(parent,5000)==WAIT_OBJECT_0 && WaitForSingleObject(child,5000)==WAIT_OBJECT_0,"Cancel must terminate owned process tree");
        CloseHandle(parent); CloseHandle(child); std::cout << "PASS cancel owned process tree\n";
        // The same lifetime guarantee must hold when a dialog/runner is destroyed.
        parent_pid=dir/L"parent-close.pid"; child_pid=dir/L"child-close.pid";
        {
            WindowsRenderJob closing;
            closing.start(fs::absolute(argv[0]).wstring(),{L"--worker",parent_pid.wstring(),child_pid.wstring()},dir.wstring(),(dir/L"close.log").wstring());
            for(int i=0;i<250 && (!pid(parent_pid) || !pid(child_pid));++i) Sleep(20);
            parent=OpenProcess(SYNCHRONIZE,FALSE,pid(parent_pid)); child=OpenProcess(SYNCHRONIZE,FALSE,pid(child_pid));
            require(parent && child,"Close test processes must exist");
        }
        require(WaitForSingleObject(parent,5000)==WAIT_OBJECT_0 && WaitForSingleObject(child,5000)==WAIT_OBJECT_0,"Destruction must terminate owned process tree");
        CloseHandle(parent); CloseHandle(child); std::cout << "PASS close owned process tree\n";
        return 0;
    } catch(const std::exception &e) { std::cerr<<"FAIL "<<e.what()<<'\n'; return 1; }
}
