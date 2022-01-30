#include "GettingStartedWindow.h"

#include <imgui.h>
#include "Fonts/IconsFontAwesome5.h"

#include "GlobalSettings.h"
#include "StyleRepository.h"
#include "AlienImGui.h"

_GettingStartedWindow::_GettingStartedWindow()
    : _AlienWindow("Getting started", "windows.getting started", true)
{
    _showAfterStartup = _on;
}

_GettingStartedWindow::~_GettingStartedWindow()
{
    _on = _showAfterStartup;
}

void _GettingStartedWindow::processIntern()
{
    ImGui::PushFont(StyleRepository::getInstance().getMediumFont());
    ImGui::PushStyleColor(ImGuiCol_Text, (ImU32)Const::HeadlineColor);
    ImGui::Text("What is Artificial Life Environment (ALiEn)?");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    AlienImGui::Separator();

    if (ImGui::BeginChild("##", ImVec2(0, ImGui::GetContentRegionAvail().y - 50), false, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x);
        ImGui::PushStyleColor(ImGuiCol_Text, (ImU32)Const::HeadlineColor);
        ImGui::Text("Abstract");
        ImGui::PopStyleColor();

        ImGui::Text("ALiEn is an artificial life simulation program based on a specialized physics and rendering "
                    "engine with extensive world building features. It is a high-performance simulation tool in "
                    "the sense that it not only uses OpenGL for rendering, but also utilizes the CUDA API to run "
                    "simulations on thousands of GPU threads.");

        ImGui::Text("Each simulated body (named as cell clusters) consists of a network of connected particles (named as "
                    "building blocks or cells) that can be enriched with higher-level functions, ranging from pure information "
                    "processing capabilities to physical equipment such as sensors, actuators, weapons, constructors, etc. "
                    "To orchestrate the execution, a token concept from graph theory is utilized. A token has a state and "
                    "is located on a cell. After each time step the token can jump to adjacent cells and triggers the "
                    "execution of cell functions. "
                    "In this way, a cell cluster can implement an arbitrarily complex set of behaviors and operates as an "
                    "agent or machine in a shared environment.");

        AlienImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImU32)Const::HeadlineColor);
        ImGui::Text("First steps");
        ImGui::PopStyleColor();

        ImGui::Text("The easiest way to get to know the alien simulator is to load and run an existing simulation "
                    "file. You find various demos in examples/simulations/* demonstrating capabilities of the "
                    "engine ranging from physics examples, self-deploying structures, replicators to small "
                    "ecosystems. To this end, go to Simulation " ICON_FA_ARROW_RIGHT
                    " Open and select a file. However, for starters, you can use the simple evolution example "
                    "already loaded.");

        ImGui::Text("At the beginning it is recommended to get familiar with the windows for temporal and spatial "
                    "controls. The handling should be intuitive and possible without deeper knowledge.");
        ImGui::Text(ICON_FA_CARET_RIGHT);
        ImGui::SameLine();
        ImGui::Text("In the temporal control window, a simulation can be started or paused. The execution speed "
                    "may be regulated if necessary. In addition, it is possible to calculate and revert single time steps as "
                    "well as snapshots of a simulation to which one can return at any time without having "
                    "to reload the simulation from a file.");
        ImGui::Text(ICON_FA_CARET_RIGHT);
        ImGui::SameLine();
        ImGui::Text("The spatial control window combines zoom information and settings on the one hand, and "
                    "scaling functions on the other hand. A quite useful feature in the dialog for "
                    "scaling/resizing is the option 'Scale content'. If activated, periodic spatial copies of "
                    "the original world can be made.");
        ImGui::Text("There are basically two modes of how the user can operate in the view where the simulation is "
                    "shown: a navigation mode and an action mode. You can switch between these two modes using the "
                    "buttons at the bottom left of the screen.");
        ImGui::Text(ICON_FA_CARET_RIGHT);
        ImGui::SameLine();
        ImGui::Text("The navigation mode is enabled by default and allows you to zoom in (while holding the left mouse "
                    "button) and zoom out (while holding the right mouse button) continuously. By holding the middle mouse "
                    "button and moving the mouse, you can move the visualized section of the world.");
        ImGui::Text(ICON_FA_CARET_RIGHT);
        ImGui::SameLine();
        ImGui::Text("In action mode, it is possible to apply forces to bodies in a running simulation or edit "
                    "them in a paused simulation. Please try this out. It can make a lot of fun!");

        ImGui::Text("To be able to experiment with existing simulations, it is important to know and change the "
                    "simulation parameters. This can be accomplished in the window 'Simulation parameters'. For example, "
                    "the radiation intensity can be increased or the friction can be adjusted. Explanations to the "
                    "individual parameters can be found in the tooltip next to them.");

        AlienImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImU32)Const::HeadlineColor);
        ImGui::Text("Further steps");
        ImGui::PopStyleColor();

        ImGui::Text("To build custom worlds with desired environment "
                    "structures and agents, the included graph editor can be used in the simulator. Documentation "
                    "with detailed tutorials can be found on the website at\nalien-project.org/documentation. ");

        ImGui::PopTextWrapPos();
    }
    ImGui::EndChild();

    AlienImGui::Separator();
    AlienImGui::ToggleButton("Show after startup", _showAfterStartup);
}
 