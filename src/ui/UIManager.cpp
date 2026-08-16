#include "ui/UIManager.hpp"
#include "ui/ThemeManager.hpp"
#include "ui/WorkspaceManager.hpp"
#include "ui/CommandPalette.hpp"
#include "ui/fonts/IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

namespace mathstudio::ui
{

    UIManager &UIManager::instance()
    {
        static UIManager inst;
        return inst;
    }

    bool UIManager::init(SDL_Window *window, SDL_Renderer *renderer)
    {
        m_window = window;
        m_renderer = renderer;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.FontGlobalScale = 1.30f;

        // ── Load & Merge FontAwesome 6 Vector Icons ─────────────────────
        io.Fonts->AddFontDefault();

        static const ImWchar icons_ranges[] = {0x2000, 0xf8ff, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = 13.0f;

        if (!io.Fonts->AddFontFromFileTTF("src/ui/fonts/fa-solid-900.ttf", 13.0f, &icons_config, icons_ranges))
        {
            io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", 13.0f, &icons_config, icons_ranges);
        }

        // Apply default Dark Obsidian theme
        ThemeManager::instance().applyTheme(ThemePreset::DarkObsidian);

        if (!ImGui_ImplSDL2_InitForSDLRenderer(window, renderer))
        {
            return false;
        }
        if (!ImGui_ImplSDLRenderer2_Init(renderer))
        {
            return false;
        }

        m_initialized = true;
        return true;
    }

    void UIManager::processEvent(const SDL_Event *event)
    {
        if (!m_initialized || !event)
            return;

        ImGui_ImplSDL2_ProcessEvent(event);

        // Ctrl+P shortcut for Command Palette
        if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.sym == SDLK_p && (event->key.keysym.mod & KMOD_CTRL))
            {
                CommandPalette::instance().open();
            }
        }
    }

    void UIManager::beginFrame()
    {
        if (!m_initialized)
            return;

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void UIManager::setupDockspace()
    {
        if (!m_initialized)
            return;

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags hostWindowFlags = 0;
        hostWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        hostWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        hostWindowFlags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("MathStudioDockSpace", nullptr, hostWindowFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspaceID = ImGui::GetID("MathStudioDockSpaceNode");
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

        if (m_lockLayout)
        {
            // Lock docking targets and prevent accidental panel moving/splitting
            dockspaceFlags |= ImGuiDockNodeFlags_NoDockingOverCentralNode;
            dockspaceFlags |= ImGuiDockNodeFlags_NoDockingSplit;
            dockspaceFlags |= ImGuiDockNodeFlags_NoDockingOverOther;
            dockspaceFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
        }

        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

        static bool firstRun = true;
        if (firstRun || ImGui::DockBuilderGetNode(dockspaceID) == nullptr)
        {
            firstRun = false;
            WorkspaceManager::instance().applyPreset(WorkspacePreset::Default);
        }

        ImGui::End();

        // Render Top Header Main Menu Bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit", "Alt+F4"))
                {
                    SDL_Event quitEv;
                    quitEv.type = SDL_QUIT;
                    SDL_PushEvent(&quitEv);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem(ICON_FA_LOCK " Lock Panel Positions", nullptr, &m_lockLayout);
                ImGui::Separator();

                if (ImGui::BeginMenu("Panels"))
                {
                    if (ImGui::MenuItem("Show All Panels"))
                    {
                        for (auto &panel : m_panels)
                        {
                            if (panel)
                                panel->isOpen = true;
                        }
                    }
                    ImGui::Separator();
                    for (auto &panel : m_panels)
                    {
                        if (panel)
                        {
                            ImGui::MenuItem(panel->getTitle(), nullptr, &panel->isOpen);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Themes"))
                {
                    if (ImGui::MenuItem("Dark Obsidian"))
                        ThemeManager::instance().applyTheme(ThemePreset::DarkObsidian);
                    if (ImGui::MenuItem("Light Clean"))
                        ThemeManager::instance().applyTheme(ThemePreset::LightClean);
                    if (ImGui::MenuItem("Dracula"))
                        ThemeManager::instance().applyTheme(ThemePreset::Dracula);
                    if (ImGui::MenuItem("Solarized Dark"))
                        ThemeManager::instance().applyTheme(ThemePreset::SolarizedDark);
                    if (ImGui::MenuItem("VS Code Dark"))
                        ThemeManager::instance().applyTheme(ThemePreset::VSCodeDark);
                    if (ImGui::MenuItem("MATLAB Classic"))
                        ThemeManager::instance().applyTheme(ThemePreset::MatlabClassic);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Workspaces"))
                {
                    if (ImGui::MenuItem("Default"))
                        WorkspaceManager::instance().applyPreset(WorkspacePreset::Default);
                    if (ImGui::MenuItem("Scientific"))
                        WorkspaceManager::instance().applyPreset(WorkspacePreset::Scientific);
                    if (ImGui::MenuItem("DSP"))
                        WorkspaceManager::instance().applyPreset(WorkspacePreset::DSP);
                    if (ImGui::MenuItem("Calculus"))
                        WorkspaceManager::instance().applyPreset(WorkspacePreset::Calculus);
                    if (ImGui::MenuItem("Minimal"))
                        WorkspaceManager::instance().applyPreset(WorkspacePreset::Minimal);
                    ImGui::Separator();
                    if (ImGui::MenuItem("Reset Layout"))
                        WorkspaceManager::instance().applyPreset(WorkspacePreset::ResetLayout);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Interface Scale"))
                {
                    ImGuiIO &io = ImGui::GetIO();
                    if (ImGui::MenuItem("100% (Normal)", nullptr, io.FontGlobalScale == 1.0f))
                        io.FontGlobalScale = 1.0f;
                    if (ImGui::MenuItem("125% (Medium)", nullptr, io.FontGlobalScale == 1.25f))
                        io.FontGlobalScale = 1.25f;
                    if (ImGui::MenuItem("140% (Large)", nullptr, io.FontGlobalScale == 1.40f))
                        io.FontGlobalScale = 1.40f;
                    if (ImGui::MenuItem("175% (Extra Large)", nullptr, io.FontGlobalScale == 1.75f))
                        io.FontGlobalScale = 1.75f;
                    if (ImGui::MenuItem("200% (High DPI)", nullptr, io.FontGlobalScale == 2.0f))
                        io.FontGlobalScale = 2.0f;
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Command Palette", "Ctrl+P"))
                {
                    CommandPalette::instance().open();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void UIManager::render(core::EvaluationContext &context)
    {
        if (!m_initialized)
            return;

        // Draw all registered panels
        for (auto &panel : m_panels)
        {
            if (panel && panel->isOpen)
            {
                panel->draw(context);
            }
        }

        // Render Command Palette overlay if active
        CommandPalette::instance().render(context);

        // Final ImGui render pass
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
    }

    void UIManager::shutdown()
    {
        if (!m_initialized)
            return;

        m_panels.clear();
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();

        m_initialized = false;
    }

} // namespace mathstudio::ui
