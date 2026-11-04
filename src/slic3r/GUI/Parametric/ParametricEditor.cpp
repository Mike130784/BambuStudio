// SPDX-License-Identifier: AGPL-3.0-or-later
#include "ParametricEditor.hpp"
#include "../Plater.hpp"
#include "../PartPlate.hpp"
#include "libslic3r/Utils.hpp"
#include "libslic3r/TriangleMesh.hpp"
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/timer.h>
#include <wx/msgdlg.h>
#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <nlohmann/json.hpp>
#include <cmath>
#include <regex>
#include <sstream>
#include <locale>
#include <chrono>
#include "WindowsRenderJob.hpp"

namespace Slic3r::GUI {
#ifdef _WIN32
namespace {
namespace fs = boost::filesystem;
using Json = nlohmann::json;

std::string read_bounded(const fs::path &path, size_t limit)
{
    if (!fs::is_regular_file(path) || fs::file_size(path) > limit)
        throw std::runtime_error("Missing file or file exceeds the prototype size limit.");
    boost::nowide::ifstream stream(path.string(), std::ios::binary);
    if (!stream) throw std::runtime_error("Cannot open local file.");
    std::string data(limit + 1, '\0');
    stream.read(data.data(), data.size());
    data.resize(static_cast<size_t>(stream.gcount()));
    if (stream.bad() || data.size() > limit) throw std::runtime_error("Cannot read local file.");
    return data;
}
void write_local(const fs::path &path, const std::string &text)
{
    boost::nowide::ofstream out(path.string(), std::ios::binary | std::ios::trunc);
    out.write(text.data(), text.size());
    out.close();
    if (!out) throw std::runtime_error("Cannot write private render input.");
}
std::wstring wide(const fs::path &path) { return wxString::FromUTF8(path.string()).ToStdWstring(); }
fs::path path_of(const wxString &text) { return fs::path(std::string(text.ToUTF8())); }

class Editor final : public wxDialog {
    struct Field { std::string name; double minimum, maximum; wxTextCtrl *value; };
    wxFilePickerCtrl *m_renderer, *m_source;
    wxButton *m_generate, *m_place, *m_cancel;
    wxStaticText *m_status;
    wxBoxSizer *m_rows;
    wxScrolledWindow *m_parameters;
    wxTimer m_timer;
    std::vector<Field> m_fields;
    Parametric::WindowsRenderJob m_job;
    fs::path m_source_path, m_schema_path, m_directory, m_result;
    std::string m_source_bytes, m_schema_bytes, m_mesh_bytes;
    std::chrono::steady_clock::time_point m_started;
    bool m_running = false, m_loading = false;

    void invalidate() { m_place->Disable(); m_mesh_bytes.clear(); }
    void message(const std::string &text) { m_status->SetLabel(wxString::FromUTF8(text)); m_status->Wrap(620); Layout(); }
    void busy(bool active) {
        m_running = active;
        m_renderer->Enable(!active); m_source->Enable(!active);
        m_generate->Enable(!active && !m_fields.empty()); m_cancel->Enable(active);
        for (auto &field : m_fields) field.value->Enable(!active);
    }
    void fail(const std::exception &error) {
        m_timer.Stop(); m_job.stop(); busy(false); invalidate(); message(error.what());
    }
    void load_source() {
        invalidate(); m_fields.clear(); m_rows->Clear(true);
        m_loading = true;
        try {
            m_source_path = fs::absolute(path_of(m_source->GetPath()));
            if (m_source_path.extension() != ".scad") throw std::runtime_error("Choose a local .scad file.");
            m_schema_path = fs::path(m_source_path.string() + ".json");
            m_source_bytes = read_bounded(m_source_path, 2 * 1024 * 1024);
            // Deliberately conservative prototype guard, not a full SCAD parser.
            // False positives (including comments) are rejected, never silently evaluated.
            if (std::regex_search(m_source_bytes, std::regex(R"(\b(include|use|import|surface)\b)")))
                throw std::runtime_error("Prototype accepts standalone SCAD only; dependencies are not supported yet.");
            m_schema_bytes = read_bounded(m_schema_path, 65536);
            Json schema = Json::parse(m_schema_bytes);
            if (schema.at("version") != 1 || !schema.at("parameters").is_array() ||
                schema.at("parameters").empty() || schema.at("parameters").size() > 32)
                throw std::runtime_error("Schema requires version 1 and 1-32 numeric parameters.");
            std::vector<std::string> names;
            for (const auto &item : schema.at("parameters")) {
                std::string name = item.at("name").get<std::string>();
                if (item.at("type") != "number" || !std::regex_match(name, std::regex("[A-Za-z_][A-Za-z0-9_]{0,63}")) ||
                    std::find(names.begin(), names.end(), name) != names.end())
                    throw std::runtime_error("Schema parameter name/type is invalid or duplicated.");
                double low = item.at("min").get<double>(), high = item.at("max").get<double>();
                double initial = item.at("default").get<double>();
                if (!std::isfinite(low) || !std::isfinite(high) || !std::isfinite(initial) || low < -1e6 || high > 1e6 ||
                    low > high || initial < low || initial > high)
                    throw std::runtime_error("Schema contains an invalid numeric range/default.");
                names.push_back(name);
                auto row = new wxBoxSizer(wxHORIZONTAL);
                const auto label = name + " [" + item.at("min").dump() + ", " + item.at("max").dump() + "]";
                row->Add(new wxStaticText(m_parameters, wxID_ANY, wxString::FromUTF8(label)), 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
                auto value = new wxTextCtrl(m_parameters, wxID_ANY, wxString::FromUTF8(item.at("default").dump()));
                row->Add(value, 1);
                m_rows->Add(row, 0, wxEXPAND | wxBOTTOM, 5);
                m_fields.push_back({name, low, high, value});
                value->Bind(wxEVT_TEXT, [this](wxCommandEvent&) { if (!m_loading) { invalidate(); message("Parameters changed. Generate again."); } });
            }
            message("Standalone model loaded. Numeric prototype; decimal separator is a dot.");
        } catch (const std::exception &error) {
            m_fields.clear(); m_rows->Clear(true); message(error.what());
        }
        m_loading = false; busy(false); m_parameters->FitInside(); Layout(); Fit();
    }
    void generate() {
        invalidate();
        try {
            if (fs::absolute(path_of(m_source->GetPath())) != m_source_path ||
                read_bounded(m_source_path, 2 * 1024 * 1024) != m_source_bytes ||
                read_bounded(m_schema_path, 65536) != m_schema_bytes)
                throw std::runtime_error("Source/schema changed. Select the model again before rendering.");
            Json values = Json::object();
            for (const auto &field : m_fields) {
                auto text = std::string(field.value->GetValue().ToUTF8());
                std::istringstream input(text); input.imbue(std::locale::classic());
                double value; input >> value;
                if (!input || !(input >> std::ws).eof() || !std::isfinite(value) || value < field.minimum || value > field.maximum)
                    throw std::runtime_error("A parameter is not a finite number within its schema range.");
                // OpenSCAD Customizer parameter-set values are strings, including numbers.
                values[field.name] = Json(value).dump();
            }
            auto exe = fs::absolute(path_of(m_renderer->GetPath()));
            if (!fs::is_regular_file(exe)) throw std::runtime_error("Select your local OpenSCAD executable first.");
            auto root = fs::path(data_dir()) / "parametric-cache";
            fs::create_directories(root);
            m_directory = root / fs::unique_path("render-%%%%%%%%-%%%%%%%%");
            if (!fs::create_directory(m_directory)) throw std::runtime_error("Cannot create unique render directory.");
            auto source = m_directory / "model.scad", params = m_directory / "parameters.json";
            m_result = m_directory / "result.stl";
            write_local(source, m_source_bytes);
            write_local(params, Json{{"fileFormatVersion", "1"}, {"parameterSets", {{"custom", values}}}}.dump(2));
            m_job.start(wide(exe), {L"--export-format", L"binstl", L"-o", wide(m_result), L"-p", wide(params),
                L"-P", L"custom", wide(source)}, wide(m_directory), wide(m_directory / "renderer.log"));
            m_started = std::chrono::steady_clock::now(); busy(true); m_timer.Start(150);
            message("Generating locally. Timeout: 60 seconds. Cancel leaves the plate unchanged.");
        } catch (const std::exception &error) { fail(error); }
    }
    void poll() {
        try {
            auto log = m_directory / "renderer.log";
            if (std::chrono::steady_clock::now() - m_started > std::chrono::seconds(60) ||
                (fs::exists(log) && fs::file_size(log) > 10 * 1024 * 1024))
                throw std::runtime_error("Renderer exceeded time/log limit. No model was placed.");
            DWORD code = 0;
            if (!m_job.finished(code)) return;
            m_timer.Stop(); m_job.stop(); busy(false);
            if (code != 0) throw std::runtime_error("OpenSCAD failed. Diagnostics remain in the local render cache.");
            m_mesh_bytes = read_bounded(m_result, 20 * 1024 * 1024);
            TriangleMesh mesh;
            if (!mesh.ReadSTLFile(m_result.string().c_str()) || mesh.its.indices.empty())
                throw std::runtime_error("OpenSCAD did not produce a readable non-empty mesh.");
            auto size = mesh.bounding_box().size();
            if (!size.allFinite() || (size.array() <= 0).any() || (size.array() > 1000).any() ||
                !std::isfinite(mesh.volume()) || mesh.volume() <= 0)
                throw std::runtime_error("Result has invalid or oversized geometry (prototype limit: 1000 mm).");
            m_place->Enable();
            message("Ready: " + std::to_string(size.x()) + " x " + std::to_string(size.y()) + " x " +
                    std::to_string(size.z()) + " mm. Use Place to add one model.");
        } catch (const std::exception &error) { fail(error); }
    }
public:
    Editor(wxWindow *parent) : wxDialog(parent, wxID_ANY, "Local parametric editor (prototype)",
        wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER), m_timer(this)
    {
        auto layout = new wxBoxSizer(wxVERTICAL);
        layout->Add(new wxStaticText(this, wxID_ANY,
            "Open trusted local SCAD only. Rendering is local, with your normal file access.\n"
            "This first prototype uses standalone models plus a .scad.json numeric schema."), 0, wxALL, 10);
        layout->Add(new wxStaticText(this, wxID_ANY, "OpenSCAD executable (official local installation)"), 0, wxLEFT, 10);
        m_renderer = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, "Select local OpenSCAD",
            "OpenSCAD (*.exe;*.com)|*.exe;*.com", wxDefaultPosition, wxSize(620, -1), wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_USE_TEXTCTRL);
        layout->Add(m_renderer, 0, wxEXPAND | wxALL, 10);
        layout->Add(new wxStaticText(this, wxID_ANY, "Standalone model (schema: same filename plus .json)"), 0, wxLEFT, 10);
        m_source = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, "Select standalone SCAD", "SCAD (*.scad)|*.scad",
            wxDefaultPosition, wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_USE_TEXTCTRL);
        layout->Add(m_source, 0, wxEXPAND | wxALL, 10);
        m_parameters = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(620, 160), wxVSCROLL);
        m_parameters->SetScrollRate(0, 10);
        m_rows = new wxBoxSizer(wxVERTICAL); m_parameters->SetSizer(m_rows);
        layout->Add(m_parameters, 0, wxEXPAND | wxALL, 10);
        m_status = new wxStaticText(this, wxID_ANY, "Select OpenSCAD and a local model.");
        layout->Add(m_status, 0, wxEXPAND | wxALL, 10);
        auto buttons = new wxBoxSizer(wxHORIZONTAL);
        m_generate = new wxButton(this, wxID_ANY, "Generate locally");
        m_cancel = new wxButton(this, wxID_ANY, "Cancel rendering");
        m_place = new wxButton(this, wxID_ANY, "Place on plate");
        auto close = new wxButton(this, wxID_CANCEL, "Close");
        for (auto button : {m_generate, m_cancel, m_place, close}) buttons->Add(button, 0, wxRIGHT, 6);
        layout->Add(buttons, 0, wxALL, 10); SetSizerAndFit(layout); CentreOnParent();
        m_place->Disable(); busy(false);
        m_source->Bind(wxEVT_FILEPICKER_CHANGED, [this](wxFileDirPickerEvent&) { load_source(); });
        m_renderer->Bind(wxEVT_FILEPICKER_CHANGED, [this](wxFileDirPickerEvent&) { invalidate(); message("Renderer changed. Generate again."); });
        m_generate->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { generate(); });
        m_cancel->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { m_timer.Stop(); m_job.stop(); busy(false); invalidate(); message("Cancelled. Plate unchanged."); });
        Bind(wxEVT_TIMER, [this](wxTimerEvent&) { if (m_running) poll(); });
        m_place->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            try {
                if (m_running || m_mesh_bytes.empty() || fs::absolute(path_of(m_source->GetPath())) != m_source_path ||
                    read_bounded(m_result, 20 * 1024 * 1024) != m_mesh_bytes ||
                    read_bounded(m_source_path, 2 * 1024 * 1024) != m_source_bytes || read_bounded(m_schema_path, 65536) != m_schema_bytes)
                    throw std::runtime_error("Source or result changed. Generate a new result first.");
                EndModal(wxID_OK);
            } catch (const std::exception &error) { fail(error); }
        });
    }
    ~Editor() override { m_timer.Stop(); m_job.stop(); }
    std::string result() const { return m_result.string(); }
};
}
#endif

void show_parametric_editor(wxWindow *parent, Plater &plater)
{
#ifdef _WIN32
    const auto snapshot = plater.get_active_snapshot_time();
    const auto model_id = plater.model().id();
    const auto plate = plater.get_partplate_list().get_curr_plate_index();
    Editor editor(parent);
    if (editor.ShowModal() != wxID_OK) return;
    if (!plater.can_add_model() || model_id != plater.model().id() || snapshot != plater.get_active_snapshot_time() ||
        plate != plater.get_partplate_list().get_curr_plate_index()) {
        wxMessageBox("Project or plate changed. Open the editor again before placing a model.", "Parametric editor", wxOK | wxICON_INFORMATION, parent);
        return;
    }
    Plater::TakeSnapshot undo(&plater, "Add parametric model");
    plater.load_files(std::vector<std::string>{editor.result()}, LoadStrategy::LoadModel, false);
#else
    wxMessageBox("The first local renderer prototype currently supports Windows only.", "Parametric editor", wxOK, parent);
#endif
}
}
