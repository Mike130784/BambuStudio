// SPDX-License-Identifier: AGPL-3.0-or-later
#include "ParametricEditor.hpp"
#include "../Plater.hpp"
#include "../PartPlate.hpp"
#include "../GUI_App.hpp"
#include "../GUI_Utils.hpp"
#include "../Widgets/Button.hpp"
#include "../Widgets/TextInput.hpp"
#include "../Widgets/Label.hpp"
#include "libslic3r/AppConfig.hpp"
#include "libslic3r/Utils.hpp"
#include "libslic3r/TriangleMesh.hpp"
#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/statline.h>
#include <wx/gauge.h>
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
#include "CustomizerParameters.hpp"

namespace Slic3r::GUI {
#ifdef _WIN32
namespace {
namespace fs = boost::filesystem;
using Json = nlohmann::json;

wxString ui(const char *english, const char *dutch)
{
    return wxGetApp().current_language_code_safe().StartsWith("nl") ? wxString::FromUTF8(dutch)
                                                                  : wxGetTranslation(wxString::FromUTF8(english));
}
std::string words(const char *english, const char *dutch) { return std::string(ui(english, dutch).ToUTF8()); }

std::string read_bounded(const fs::path &path, size_t limit)
{
    if (!fs::is_regular_file(path) || fs::file_size(path) > limit)
        throw std::runtime_error(words("Missing file or file exceeds the prototype size limit.", "Bestand ontbreekt of is te groot voor deze editor."));
    boost::nowide::ifstream stream(path.string(), std::ios::binary);
    if (!stream) throw std::runtime_error(words("Cannot open local file.", "Het lokale bestand kon niet worden geopend."));
    std::string data(limit + 1, '\0');
    stream.read(data.data(), data.size());
    data.resize(static_cast<size_t>(stream.gcount()));
    if (stream.bad() || data.size() > limit) throw std::runtime_error(words("Cannot read local file.", "Het lokale bestand kon niet worden gelezen."));
    return data;
}
void write_local(const fs::path &path, const std::string &text)
{
    boost::nowide::ofstream out(path.string(), std::ios::binary | std::ios::trunc);
    out.write(text.data(), text.size());
    out.close();
    if (!out) throw std::runtime_error(words("Cannot write private render input.", "De lokale renderbestanden konden niet worden opgeslagen."));
}
std::wstring wide(const fs::path &path) { return wxString::FromUTF8(path.string()).ToStdWstring(); }
fs::path path_of(const wxString &text) { return fs::path(std::string(text.ToUTF8())); }

class Editor final : public DPIDialog {
    enum class Feedback { Neutral, Working, Ready, Error };
    struct Field { std::string name; double minimum, maximum, step; wxTextCtrl *value; TextInput *control; };
    TextInput *m_renderer, *m_source;
    Button *m_generate, *m_place, *m_cancel, *m_close, *m_open_source, *m_open_renderer;
    wxStaticText *m_status, *m_state_title, *m_model_note, *m_empty, *m_settings_hint;
    wxGauge *m_progress;
    StaticBox *m_result_panel;
    wxBoxSizer *m_rows;
    wxScrolledWindow *m_parameters;
    wxTimer m_timer;
    std::vector<Field> m_fields;
    Parametric::WindowsRenderJob m_job;
    fs::path m_source_path, m_schema_path, m_directory, m_result;
    std::string m_source_bytes, m_schema_bytes, m_mesh_bytes;
    std::chrono::steady_clock::time_point m_started;
    bool m_running = false, m_loading = false, m_has_schema = false;
    Feedback m_feedback = Feedback::Neutral;

    wxString renderer_path() const { return m_renderer->GetTextCtrl()->GetValue(); }
    wxString source_path() const { return m_source->GetTextCtrl()->GetValue(); }

    void remember_path(const char *key, const wxString &value) {
        auto config = wxGetApp().app_config;
        if (!config) return;
        try {
            config->set("custom_parametric_editor", key, std::string(value.ToUTF8()));
            config->save();
            m_settings_hint->SetLabel(ui("Paths are remembered on this computer.", "Paden worden op deze computer onthouden."));
        } catch (const std::exception&) {
            m_settings_hint->SetLabel(ui("Could not remember the path. You can still use it this session.", "Pad kon niet worden onthouden. Je kunt het deze keer wel gebruiken."));
        }
        m_settings_hint->Wrap(FromDIP(650));
    }

    void choose_file(bool renderer) {
        const auto current = renderer ? renderer_path() : source_path();
        wxFileName initial(current);
        wxFileDialog picker(this, renderer ? ui("Choose OpenSCAD", "Kies OpenSCAD") : ui("Open a parametric model", "Open een parametrisch model"),
            initial.GetPath(), initial.GetFullName(), renderer ? "OpenSCAD (*.exe;*.com)|*.exe;*.com" : "OpenSCAD (*.scad)|*.scad",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (picker.ShowModal() != wxID_OK) return;
        auto input = renderer ? m_renderer : m_source;
        input->GetTextCtrl()->ChangeValue(picker.GetPath());
        input->SetToolTip(picker.GetPath());
        invalidate();
        if (renderer) {
            remember_path("renderer_path", picker.GetPath());
            message(words("OpenSCAD selected. Generate a new result.", "OpenSCAD gekozen. Genereer een nieuw resultaat."));
        } else load_source();
    }

    Button *button(wxWindow *parent, const wxString &text, bool primary = false) {
        auto result = new Button(parent, text);
        result->SetMinSize(FromDIP(wxSize(110, 34)));
        result->SetCornerRadius(FromDIP(6));
        if (primary) {
            result->SetBackgroundColor(StateColor(std::make_pair(ThemeColor::Grey300, int(StateColor::Disabled)),
                std::make_pair(ThemeColor::BrandGreenPressed, int(StateColor::Pressed)),
                std::make_pair(ThemeColor::BrandGreenHovered, int(StateColor::Hovered)),
                std::make_pair(ThemeColor::BrandGreen, int(StateColor::Normal))));
            result->SetBorderColor(StateColor(std::make_pair(ThemeColor::Grey300, int(StateColor::Disabled)),
                std::make_pair(ThemeColor::BrandGreen, int(StateColor::Normal))));
            result->SetTextColor(StateColor(std::make_pair(ThemeColor::TextDisabled, int(StateColor::Disabled)),
                std::make_pair(ThemeColor::WhiteFixed, int(StateColor::Normal))));
        } else result->SetBackgroundColor(StateColor::createButtonStyleGray());
        return result;
    }

    TextInput *input(wxWindow *parent, const wxString &value, const wxString &unit = wxEmptyString, bool path = false) {
        auto result = new TextInput(parent, value, wxEmptyString, wxEmptyString, wxDefaultPosition,
            FromDIP(wxSize(path ? 430 : 180, 34)), path ? wxTE_READONLY : 0, unit);
        result->SetMinSize(FromDIP(wxSize(path ? 300 : 160, 34)));
        result->SetCornerRadius(FromDIP(6));
        return result;
    }

    void apply_theme() {
        SetBackgroundColour(StateColor::darkModeColorFor(ThemeColor::White));
        m_parameters->SetBackgroundColour(GetBackgroundColour());
        wxGetApp().UpdateDlgDarkUI(this);
        m_state_title->SetForegroundColour(StateColor::darkModeColorFor(m_feedback == Feedback::Ready ? ThemeColor::BrandGreen :
            m_feedback == Feedback::Error ? ThemeColor::Danger : ThemeColor::TextPrimary));
        Refresh();
    }

    void on_dpi_changed(const wxRect&) override {
        for (auto control : {m_generate, m_place, m_cancel, m_close, m_open_source, m_open_renderer}) control->Rescale();
        m_renderer->Rescale(); m_source->Rescale();
        for (const auto &field : m_fields) field.control->Rescale();
        SetMinSize(FromDIP(wxSize(740, 650))); Layout(); Refresh();
    }
    void on_sys_color_changed() override { apply_theme(); }

    void invalidate() { m_place->Disable(); m_mesh_bytes.clear(); }
    void message(const std::string &text, Feedback feedback = Feedback::Neutral) {
        m_feedback = feedback;
        m_status->SetLabel(wxString::FromUTF8(text)); m_status->Wrap(FromDIP(650));
        m_state_title->SetLabel(feedback == Feedback::Ready ? ui("Ready to place", "Klaar om te plaatsen") :
            feedback == Feedback::Working ? ui("Generating model", "Model wordt gegenereerd") :
            feedback == Feedback::Error ? ui("Check the model or input", "Controleer het model of de invoer") :
            ui("Generate a new result", "Genereer een nieuw resultaat"));
        m_state_title->SetForegroundColour(StateColor::darkModeColorFor(feedback == Feedback::Ready ? ThemeColor::BrandGreen :
            feedback == Feedback::Error ? ThemeColor::Danger : ThemeColor::TextPrimary));
        Layout();
    }
    void busy(bool active) {
        m_running = active;
        m_renderer->Enable(!active); m_source->Enable(!active);
        m_open_source->Enable(!active); m_open_renderer->Enable(!active);
        m_generate->Enable(!active && !m_fields.empty()); m_cancel->Enable(active);
        m_cancel->Show(active); m_place->Show(!active);
        for (auto &field : m_fields) field.control->Enable(!active);
        m_progress->Show(active);
        if (active) m_progress->Pulse();
        Layout();
    }
    void fail(const std::exception &error) {
        m_timer.Stop(); m_job.stop(); busy(false); invalidate();
        if (dynamic_cast<const fs::filesystem_error*>(&error))
            message(words("A local file is unavailable. Check the selected paths and try again.", "Een lokaal bestand is niet bereikbaar. Controleer de gekozen paden en probeer opnieuw."), Feedback::Error);
        else if (std::string(error.what()).rfind("Cannot ", 0) == 0)
            message(words("Could not start or control OpenSCAD. Check the selected application and try again.", "OpenSCAD kon niet worden gestart of aangestuurd. Controleer het gekozen programma en probeer opnieuw."), Feedback::Error);
        else message(error.what(), Feedback::Error);
    }
    bool schema_unchanged() const {
        return m_has_schema ? read_bounded(m_schema_path, 65536) == m_schema_bytes : !fs::exists(m_schema_path);
    }
    void load_source(bool remember = true) {
        m_loading = true;
        invalidate(); m_fields.clear(); m_rows->Clear(true);
        try {
            m_source_path = fs::absolute(path_of(source_path()));
            if (m_source_path.extension() != ".scad") throw std::runtime_error(words("Choose a local .scad file.", "Kies een lokaal .scad-bestand."));
            m_schema_path = fs::path(m_source_path.string() + ".json");
            m_source_bytes = read_bounded(m_source_path, 2 * 1024 * 1024);
            // Deliberately conservative prototype guard, not a full SCAD parser.
            // False positives (including comments) are rejected, never silently evaluated.
            if (std::regex_search(m_source_bytes, std::regex(R"(\b(include|use|import|surface)\b)")))
                throw std::runtime_error(words("Prototype accepts standalone SCAD only; dependencies are not supported yet.", "Dit model gebruikt externe bestanden. Deze eerste versie ondersteunt alleen zelfstandige SCAD-modellen."));
            // An existing sidecar always wins; invalid schemas never silently fall back.
            m_has_schema = fs::exists(m_schema_path);
            m_schema_bytes.clear();
            Json schema;
            if (m_has_schema) {
                m_schema_bytes = read_bounded(m_schema_path, 65536);
                schema = Json::parse(m_schema_bytes);
            } else {
                auto parameters = Json::array();
                for (const auto &parameter : Parametric::read_customizer_parameters(m_source_bytes))
                    parameters.push_back({{"name", parameter.name}, {"label", parameter.label}, {"type", "number"},
                        {"default", parameter.initial}, {"min", parameter.minimum}, {"max", parameter.maximum}, {"step", parameter.step}});
                schema = Json{{"version", 1}, {"parameters", parameters}};
            }
            if (schema.at("version") != 1 || !schema.at("parameters").is_array() ||
                schema.at("parameters").empty() || schema.at("parameters").size() > 32)
                throw std::runtime_error(words("Schema requires version 1 and 1-32 numeric parameters.", "Het schema moet versie 1 en 1 tot 32 getalparameters bevatten."));
            std::vector<std::string> names;
            for (const auto &item : schema.at("parameters")) {
                std::string name = item.at("name").get<std::string>();
                if (item.at("type") != "number" || !std::regex_match(name, std::regex("[A-Za-z_][A-Za-z0-9_]{0,63}")) ||
                    std::find(names.begin(), names.end(), name) != names.end())
                    throw std::runtime_error(words("Schema parameter name/type is invalid or duplicated.", "Een parameternaam of -type is ongeldig, of komt dubbel voor."));
                double low = item.at("min").get<double>(), high = item.at("max").get<double>();
                double initial = item.at("default").get<double>();
                double step = item.value("step", 0.0);
                if (!std::isfinite(low) || !std::isfinite(high) || !std::isfinite(initial) || low < -1e6 || high > 1e6 ||
                    low > high || initial < low || initial > high || !std::isfinite(step) || step < 0 ||
                    !Parametric::on_parameter_step(initial, low, step))
                    throw std::runtime_error(words("Schema contains an invalid numeric range/default.", "Een parameter heeft een ongeldig bereik of een ongeldige standaardwaarde."));
                names.push_back(name);
                const auto display = item.value("label", name);
                const auto unit = item.value("unit", std::string());
                if (display.size() > 120 || unit.size() > 24)
                    throw std::runtime_error(words("Parameter label or unit is too long.", "De parameternaam of eenheid is te lang."));
                auto row = new wxBoxSizer(wxHORIZONTAL);
                auto labels = new wxBoxSizer(wxVERTICAL);
                auto label = new wxStaticText(m_parameters, wxID_ANY, wxString::FromUTF8(display));
                label->SetFont(Label::Body_14);
                label->Wrap(FromDIP(360));
                labels->Add(label);
                auto range = new wxStaticText(m_parameters, wxID_ANY, wxString::FromUTF8(item.at("min").dump() + " - " + item.at("max").dump()));
                if (step > 0) range->SetLabel(range->GetLabel() + ui("; step ", "; stap ") + wxString::FromUTF8(Json(step).dump()));
                range->SetFont(Label::Body_12);
                range->SetForegroundColour(StateColor::darkModeColorFor(ThemeColor::TextMuted));
                labels->Add(range, 0, wxTOP, FromDIP(2));
                row->Add(labels, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(16));
                auto control = input(m_parameters, wxString::FromUTF8(item.at("default").dump()));
                auto value = control->GetTextCtrl();
                value->SetName(wxString::FromUTF8(display));
                row->Add(control, 0, wxALIGN_CENTER_VERTICAL);
                if (!unit.empty()) {
                    auto unit_label = new wxStaticText(m_parameters, wxID_ANY, wxString::FromUTF8(unit));
                    unit_label->SetFont(Label::Body_12);
                    unit_label->SetForegroundColour(StateColor::darkModeColorFor(ThemeColor::TextMuted));
                    unit_label->SetMinSize(FromDIP(wxSize(34, -1)));
                    row->Add(unit_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(8));
                }
                m_rows->Add(row, 0, wxEXPAND | wxBOTTOM, FromDIP(14));
                m_fields.push_back({name, low, high, step, value, control});
                value->Bind(wxEVT_TEXT, [this](wxCommandEvent&) { if (!m_loading) { invalidate(); message(words("Parameters changed. Generate a new result.", "Parameters aangepast. Genereer een nieuw resultaat.")); } });
            }
            if (remember) remember_path("model_path", source_path());
            m_model_note->SetLabel(wxString::Format(ui("%zu parameters - use a dot for decimals", "%zu parameters - gebruik een punt voor decimalen"), m_fields.size()) + (m_has_schema ? " (JSON)" : " (Customizer)"));
            message(words("Adjust the values and generate your model.", "Pas de waarden aan en genereer je model."));
        } catch (const Parametric::CustomizerError &error) {
            m_fields.clear(); m_rows->Clear(true);
            message(std::string(wxString::Format(ui(
                "Unsupported Customizer header at line %zu. Use numeric literals with // [min:max] or // [min:step:max], or supply a .scad.json schema.",
                "Customizer-kop niet ondersteund op regel %zu. Gebruik getallen met // [min:max] of // [min:stap:max], of kies een model met .scad.json-schema."), error.line).ToUTF8()), Feedback::Error);
            m_model_note->SetLabel(ui("No supported parameters loaded", "Geen ondersteunde parameters geladen"));
        } catch (const std::exception &error) {
            m_fields.clear(); m_rows->Clear(true);
            message(dynamic_cast<const Json::exception*>(&error) ? words("The parameter schema is invalid. Check the .scad.json file.", "Het parameterschema is ongeldig. Controleer het .scad.json-bestand.") : error.what(), Feedback::Error);
            m_model_note->SetLabel(ui("No parameters loaded", "Geen parameters geladen"));
        }
        m_empty->Show(m_fields.empty());
        m_loading = false; busy(false); m_parameters->FitInside(); apply_theme(); Layout();
    }
    void generate() {
        invalidate();
        try {
            if (fs::absolute(path_of(source_path())) != m_source_path ||
                read_bounded(m_source_path, 2 * 1024 * 1024) != m_source_bytes ||
                !schema_unchanged())
                throw std::runtime_error(words("Source/schema changed. Select the model again before rendering.", "Het model of schema is gewijzigd. Open het model opnieuw voordat je genereert."));
            Json values = Json::object();
            for (const auto &field : m_fields) {
                auto text = std::string(field.value->GetValue().ToUTF8());
                std::istringstream input(text); input.imbue(std::locale::classic());
                double value; input >> value;
                if (!input || !(input >> std::ws).eof() || !std::isfinite(value) || value < field.minimum || value > field.maximum ||
                    !Parametric::on_parameter_step(value, field.minimum, field.step))
                    throw std::runtime_error(words("A parameter is not a finite number within its schema range.", "Een waarde past niet bij het aangegeven bereik of de stapgrootte."));
                // OpenSCAD Customizer parameter-set values are strings, including numbers.
                values[field.name] = Json(value).dump();
            }
            auto exe = fs::absolute(path_of(renderer_path()));
            if (!fs::is_regular_file(exe)) throw std::runtime_error(words("Select your local OpenSCAD executable first.", "Kies eerst het OpenSCAD-programma op je computer."));
            auto root = fs::path(data_dir()) / "parametric-cache";
            fs::create_directories(root);
            m_directory = root / fs::unique_path("render-%%%%%%%%-%%%%%%%%");
            if (!fs::create_directory(m_directory)) throw std::runtime_error(words("Cannot create unique render directory.", "Er kon geen nieuwe lokale rendermap worden gemaakt."));
            auto source = m_directory / "model.scad", params = m_directory / "parameters.json";
            m_result = m_directory / "result.stl";
            write_local(source, m_source_bytes);
            write_local(params, Json{{"fileFormatVersion", "1"}, {"parameterSets", {{"custom", values}}}}.dump(2));
            m_job.start(wide(exe), {L"--export-format", L"binstl", L"-o", wide(m_result), L"-p", wide(params),
                L"-P", L"custom", wide(source)}, wide(m_directory), wide(m_directory / "renderer.log"));
            m_started = std::chrono::steady_clock::now(); busy(true); m_timer.Start(150);
            message(words("This may take a moment. You can cancel without changing the plate.", "Dit kan even duren. Je kunt annuleren zonder de printplaat te wijzigen."), Feedback::Working);
        } catch (const std::exception &error) { fail(error); }
    }
    void poll() {
        try {
            m_progress->Pulse();
            auto log = m_directory / "renderer.log";
            if (std::chrono::steady_clock::now() - m_started > std::chrono::seconds(60) ||
                (fs::exists(log) && fs::file_size(log) > 10 * 1024 * 1024))
                throw std::runtime_error(words("Renderer exceeded time/log limit. No model was placed.", "Genereren duurde te lang of gaf te veel meldingen. Er is geen model geplaatst."));
            DWORD code = 0;
            if (!m_job.finished(code)) return;
            m_timer.Stop(); m_job.stop(); busy(false);
            if (code != 0) throw std::runtime_error(words("OpenSCAD failed. Diagnostics remain in the local render cache.", "OpenSCAD kon dit model niet genereren. Controleer de bron en de gekozen parameters."));
            m_mesh_bytes = read_bounded(m_result, 20 * 1024 * 1024);
            TriangleMesh mesh;
            if (!mesh.ReadSTLFile(m_result.string().c_str()) || mesh.its.indices.empty())
                throw std::runtime_error(words("OpenSCAD did not produce a readable non-empty mesh.", "OpenSCAD heeft geen bruikbaar model met geometrie gemaakt."));
            auto size = mesh.bounding_box().size();
            if (!size.allFinite() || (size.array() <= 0).any() || (size.array() > 1000).any() ||
                !std::isfinite(mesh.volume()) || mesh.volume() <= 0)
                throw std::runtime_error(words("Result has invalid or oversized geometry (prototype limit: 1000 mm).", "Het resultaat is ongeldig of groter dan de limiet van 1000 mm per richting."));
            m_place->Enable();
            message(std::string(wxString::Format("%.2f x %.2f x %.2f mm", size.x(), size.y(), size.z()).ToUTF8()) +
                    words(". Add this model with Place on plate.", ". Voeg dit model toe met Op printplaat plaatsen."), Feedback::Ready);
        } catch (const std::exception &error) { fail(error); }
    }
public:
    Editor(wxWindow *parent) : DPIDialog(parent, wxID_ANY, ui("Parametric model", "Parametrisch model"),
        wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER), m_timer(this)
    {
        SetFont(Label::Body_14);
        auto root_layout = new wxBoxSizer(wxVERTICAL);
        auto layout = new wxBoxSizer(wxVERTICAL);
        root_layout->Add(layout, 1, wxEXPAND | wxALL, FromDIP(20));
        const int pad = FromDIP(6), gap = FromDIP(12);
        auto title = new wxStaticText(this, wxID_ANY, ui("Parametric model", "Parametrisch model"));
        title->SetFont(Label::Head_20);
        layout->Add(title, 0, wxLEFT | wxRIGHT | wxTOP, pad);
        auto subtitle = new wxStaticText(this, wxID_ANY, ui(
            "Adjust dimensions, generate locally and add the result to your plate.",
            "Pas maten aan, genereer lokaal en voeg het resultaat toe aan je printplaat."));
        subtitle->SetFont(Label::Body_13);
        subtitle->Wrap(FromDIP(680));
        layout->Add(subtitle, 0, wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, pad);
        layout->Add(new wxStaticLine(this), 0, wxEXPAND);

        auto section = [this, layout, pad](const wxString &text) {
            auto label = new wxStaticText(this, wxID_ANY, text);
            label->SetFont(Label::Head_14);
            layout->AddSpacer(FromDIP(12));
            layout->Add(label, 0, wxLEFT | wxRIGHT, pad);
        };
        section(ui("Model", "Model"));
        auto model_row = new wxBoxSizer(wxHORIZONTAL);
        m_source = input(this, wxEmptyString, wxEmptyString, true);
        m_source->GetTextCtrl()->SetName(ui("SCAD model path", "Pad naar SCAD-model"));
        m_open_source = button(this, ui("Open model...", "Model openen..."));
        model_row->Add(m_source, 1, wxEXPAND | wxRIGHT, gap);
        model_row->Add(m_open_source, 0, wxALIGN_CENTER_VERTICAL);
        layout->Add(model_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

        section(ui("Parameters", "Parameters"));
        m_model_note = new wxStaticText(this, wxID_ANY, ui("Choose a model to see its parameters.", "Kies een model om de parameters te zien."));
        m_model_note->SetFont(Label::Body_12);
        layout->Add(m_model_note, 0, wxLEFT | wxRIGHT | wxTOP, pad);
        m_empty = new wxStaticText(this, wxID_ANY, ui("Your model's adjustable dimensions will appear here.", "Hier verschijnen de instelbare maten van je model."));
        m_empty->SetFont(Label::Body_13);
        layout->Add(m_empty, 0, wxLEFT | wxRIGHT | wxTOP, pad);
        m_parameters = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(640, 150)), wxVSCROLL);
        m_parameters->SetScrollRate(0, FromDIP(12));
        m_rows = new wxBoxSizer(wxVERTICAL); m_parameters->SetSizer(m_rows);
        layout->Add(m_parameters, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

        m_result_panel = new StaticBox(this);
        m_result_panel->SetBackgroundColor(ThemeColor::Grey200);
        m_result_panel->SetBorderColor(ThemeColor::Grey350);
        m_result_panel->SetCornerRadius(FromDIP(8));
        auto result_layout = new wxBoxSizer(wxVERTICAL);
        m_state_title = new wxStaticText(m_result_panel, wxID_ANY, ui("Choose a model", "Kies een model"));
        m_state_title->SetFont(Label::Head_14);
        result_layout->Add(m_state_title, 0, wxLEFT | wxRIGHT | wxTOP, FromDIP(14));
        m_status = new wxStaticText(m_result_panel, wxID_ANY, ui("Open a local SCAD model to get started.", "Open een lokaal SCAD-model om te beginnen."));
        m_status->SetFont(Label::Body_13);
        m_status->SetMinSize(FromDIP(wxSize(620, 38)));
        result_layout->Add(m_status, 0, wxEXPAND | wxALL, FromDIP(14));
        m_progress = new wxGauge(m_result_panel, wxID_ANY, 100, wxDefaultPosition, FromDIP(wxSize(-1, 4)));
        result_layout->Add(m_progress, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(14));
        m_result_panel->SetSizer(result_layout);
        layout->Add(m_result_panel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

        section(ui("OpenSCAD", "OpenSCAD"));
        auto renderer_row = new wxBoxSizer(wxHORIZONTAL);
        m_renderer = input(this, wxEmptyString, wxEmptyString, true);
        m_renderer->GetTextCtrl()->SetName(ui("OpenSCAD application path", "Pad naar OpenSCAD"));
        m_open_renderer = button(this, ui("Choose...", "Kiezen..."));
        renderer_row->Add(m_renderer, 1, wxEXPAND | wxRIGHT, gap);
        renderer_row->Add(m_open_renderer, 0, wxALIGN_CENTER_VERTICAL);
        layout->Add(renderer_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
        m_settings_hint = new wxStaticText(this, wxID_ANY, ui("Choose OpenSCAD once. Paths are remembered locally.", "Kies OpenSCAD eenmalig. Paden worden lokaal onthouden."));
        m_settings_hint->SetFont(Label::Body_12);
        layout->Add(m_settings_hint, 0, wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, pad);
        layout->Add(new wxStaticLine(this), 0, wxEXPAND);

        auto buttons = new wxBoxSizer(wxHORIZONTAL);
        m_generate = button(this, ui("Generate model", "Model genereren"));
        m_cancel = button(this, ui("Cancel rendering", "Genereren stoppen"));
        m_place = button(this, ui("Place on plate", "Op printplaat plaatsen"), true);
        m_place->SetMinSize(FromDIP(wxSize(180, 34)));
        m_close = button(this, ui("Close", "Sluiten"));
        m_close->SetId(wxID_CANCEL);
        buttons->Add(m_generate, 0, wxRIGHT, gap);
        buttons->Add(m_cancel, 0, wxRIGHT, gap);
        buttons->AddStretchSpacer();
        buttons->Add(m_close, 0, wxRIGHT, gap);
        buttons->Add(m_place);
        layout->Add(buttons, 0, wxEXPAND | wxALL, pad);
        SetSizer(root_layout);
        SetMinSize(FromDIP(wxSize(740, 650)));
        SetSize(FromDIP(wxSize(800, 740)));
        CentreOnParent();
        m_place->Disable(); busy(false);

        m_open_source->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { choose_file(false); });
        m_open_renderer->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { choose_file(true); });
        m_generate->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { generate(); });
        m_close->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { EndModal(wxID_CANCEL); });
        m_cancel->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            m_timer.Stop(); m_job.stop(); busy(false); invalidate();
            message(words("Cancelled. Your plate is unchanged.", "Gestopt. Je printplaat is ongewijzigd."));
        });
        Bind(wxEVT_TIMER, [this](wxTimerEvent&) { if (m_running) poll(); });
        m_place->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            try {
                if (m_running || m_mesh_bytes.empty() || fs::absolute(path_of(source_path())) != m_source_path ||
                    read_bounded(m_result, 20 * 1024 * 1024) != m_mesh_bytes ||
                    read_bounded(m_source_path, 2 * 1024 * 1024) != m_source_bytes || !schema_unchanged())
                    throw std::runtime_error(words("Source or result changed. Generate a new result first.", "Model of resultaat gewijzigd. Genereer eerst een nieuw resultaat."));
                EndModal(wxID_OK);
            } catch (const std::exception &error) { fail(error); }
        });
        // Restore paths only. Parameters use schema defaults and geometry is never restored or run automatically.
        if (auto config = wxGetApp().app_config) {
            const auto renderer = wxString::FromUTF8(config->get("custom_parametric_editor", "renderer_path"));
            const auto model = wxString::FromUTF8(config->get("custom_parametric_editor", "model_path"));
            m_renderer->GetTextCtrl()->ChangeValue(renderer); m_renderer->SetToolTip(renderer);
            m_source->GetTextCtrl()->ChangeValue(model); m_source->SetToolTip(model);
            if (!renderer.empty()) m_settings_hint->SetLabel(ui("Paths are remembered on this computer.", "Paden worden op deze computer onthouden."));
            if (!model.empty()) {
                try {
                    auto root = fs::absolute(path_of(model)).root_path();
                    if (!model.StartsWith("\\\\") && !model.StartsWith("//") && GetDriveTypeW(wide(root).c_str()) == DRIVE_FIXED) load_source(false);
                    else message(words("Path remembered. Open the model again to load a network or removable file.", "Pad onthouden. Open het model opnieuw om een netwerkbestand of verwisselbare schijf te laden."));
                } catch (const std::exception &error) { message(error.what(), Feedback::Error); }
            }
        }
        apply_theme(); Layout();
    }
    ~Editor() override { m_timer.Stop(); m_job.stop(); }
    std::string result() const { return m_result.string(); }
};
}
#endif

wxString parametric_editor_menu_label()
{
    return wxGetApp().current_language_code_safe().StartsWith("nl") ? wxString("Parametrisch model...") : wxString("Parametric model...");
}

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
