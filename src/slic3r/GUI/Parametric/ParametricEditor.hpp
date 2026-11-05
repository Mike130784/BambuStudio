// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <wx/string.h>
class wxWindow;
namespace Slic3r::GUI {
class Plater;
void show_parametric_editor(wxWindow *parent, Plater &plater);
wxString parametric_editor_menu_label();
}
