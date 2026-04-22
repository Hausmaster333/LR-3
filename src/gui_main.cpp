#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "deque/segment_deque.h"
#include "gui/deque_visualizer.h"
#include "gui/benchmarks.h"
#include <GLFW/glfw3.h>
#include <iostream>

MutableSegmentedDeque<int> g_deque;
int g_input_value = 0;
static double g_deque_push_front_times[6] = {0};
static double g_array_push_front_times[6] = {0};
static double g_list_push_front_times[6] = {0};

static double g_deque_get_times[6] = {0};
static double g_array_get_times[6] = {0};
static double g_list_get_times[6] = {0};

static double g_mem_deque[6] = {0};
static double g_mem_array[6] = {0};
static double g_mem_list[6] = {0};

static bool g_benchmarks_done = false;

int g_map_choice = 0;
const char* g_map_funcs[] = {"x * 2", "x squared", "-x"};

int map_double(const int& x) { return x * 2; }
int map_square(const int& x) { return x * x; }
int map_negate(const int& x) { return -x; }

int main() {
    // 1. Инициализируем GLFW (оконная система)
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return 1;
    }

    // 2. Создаём окно 1280x720
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Segmented Deque", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync

    // 3. Инициализируем ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(2.0f);  // увеличить виджеты
    io.FontGlobalScale = 2.0f;              // увеличить шрифт
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // 4. Главный цикл
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Начинаем кадр
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Deque menu
        ImGui::Begin("Segmented Deque");

        ImGui::InputInt("Value", &g_input_value);

        if (ImGui::Button("Push Front")) {
            g_deque.push_front(g_input_value);
        }
        ImGui::SameLine();
        if (ImGui::Button("Push Back")) {
            g_deque.push_back(g_input_value);
        }

        if (ImGui::Button("Pop Front")) {
            if (g_deque.get_count() > 0) {
                int val;
                g_deque.pop_front(&val);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Pop Back")) {
            if (g_deque.get_count() > 0) {
                int val;
                g_deque.pop_back(&val);
            }
        }

        if (ImGui::Button("Reset")) {
            g_deque.reset_deque();
        }

        ImGui::Separator();
        ImGui::Text("Count: %d", g_deque.get_count());

        // Отображение элементов
        ImGui::Text("Elements:");
        for (int i = 0; i < g_deque.get_count(); i++) {
            ImGui::SameLine();
            ImGui::Text("%d", g_deque.get(i));
        }

        ImGui::Text("Internal structure:");
        render_deque(g_deque);

        ImGui::Separator();

        ImGui::Text("Map");

        // Combo (dropdown) — выбор функции
        ImGui::Combo("Function##map", &g_map_choice, g_map_funcs, 3);

        if (ImGui::Button("Apply Map")) {
            int (*func)(const int&) = nullptr;
            if (g_map_choice == 0) func = map_double;
            else if (g_map_choice == 1) func = map_square;
            else if (g_map_choice == 2) func = map_negate;
            
            Sequence<int>* result = g_deque.map(func);
            
            // Заменяем дек на результат
            g_deque = *dynamic_cast<MutableSegmentedDeque<int>*>(result);
            delete result;
        }

        ImGui::End();

        // ======== Benchmarks menu
        ImGui::Begin("Benchmarks");

        if (ImGui::Button("Run Benchmarks")) {
            run_benchmarks(g_deque_push_front_times, g_array_push_front_times, g_list_push_front_times,
                            g_deque_get_times, g_array_get_times, g_list_get_times,
                            g_mem_deque, g_mem_array, g_mem_list);
            g_benchmarks_done = true;
        }

        if (g_benchmarks_done) {
            static double sizes_d[] = {1000, 2000, 5000, 10000, 20000, 50000, 100000};

            ImGui::BeginChild("PushFrontBenchmark", ImVec2(0, 600), true);
            if (ImPlot::BeginPlot("Push Front Performance")) {
                ImPlot::SetupAxes("Elements count", "Time (ms)");
                ImPlot::PlotLine("SegmentDeque", sizes_d, g_deque_push_front_times, 7);
                ImPlot::PlotLine("ArraySequence", sizes_d, g_array_push_front_times, 7);
                ImPlot::PlotLine("ListSequence", sizes_d, g_list_push_front_times, 7);
                ImPlot::EndPlot();
            }
    

            if (ImGui::BeginTable("push_front_bench_table", 4, ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Size");
                ImGui::TableSetupColumn("Deque (ms)");
                ImGui::TableSetupColumn("Array (ms)");
                ImGui::TableSetupColumn("List (ms)");
                ImGui::TableHeadersRow();
                
                for (int i = 0; i < 7; i++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%.0f", sizes_d[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_deque_push_front_times[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_array_push_front_times[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_list_push_front_times[i]);
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            ImGui::BeginChild("GetBenchmark", ImVec2(0, 600), true);
            if (ImPlot::BeginPlot("Get(index) Performance")) {
                ImPlot::SetupAxes("Elements count", "Time (ms)");
                ImPlot::PlotLine("SegmentDeque", sizes_d, g_deque_get_times, 7);
                ImPlot::PlotLine("ArraySequence", sizes_d, g_array_get_times, 7);
                ImPlot::PlotLine("ListSequence", sizes_d, g_list_get_times, 7);
                ImPlot::EndPlot();
            }
    

            if (ImGui::BeginTable("get_bench_table", 4, ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Size");
                ImGui::TableSetupColumn("Deque (ms)");
                ImGui::TableSetupColumn("Array (ms)");
                ImGui::TableSetupColumn("List (ms)");
                ImGui::TableHeadersRow();
                
                for (int i = 0; i < 7; i++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%.0f", sizes_d[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_deque_get_times[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_array_get_times[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_list_get_times[i]);
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();

            ImGui::BeginChild("MemoryBenchmark", ImVec2(0, 600), true);
            if (ImPlot::BeginPlot("Memory Performance")) {
                ImPlot::SetupAxes("Elements count", "Memory (KB)");
                ImPlot::PlotLine("SegmentDeque", sizes_d, g_mem_deque, 7);
                ImPlot::PlotLine("ArraySequence", sizes_d, g_mem_array, 7);
                ImPlot::PlotLine("ListSequence", sizes_d, g_mem_list, 7);
                ImPlot::EndPlot();
            }
    

            if (ImGui::BeginTable("memory_bench_table", 4, ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Size");
                ImGui::TableSetupColumn("Deque (KB)");
                ImGui::TableSetupColumn("Array (KB)");
                ImGui::TableSetupColumn("List (KB)");
                ImGui::TableHeadersRow();
                
                for (int i = 0; i < 7; i++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%.0f", sizes_d[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_mem_deque[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_mem_array[i]);
                    ImGui::TableNextColumn(); ImGui::Text("%.3f", g_mem_list[i]);
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
        }

        ImGui::End();
        // Рендеринг
        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // 5. Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}