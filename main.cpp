#include <array>
#include <cstring>
#include <thread>
#include <chrono>
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#ifdef WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif
#include "Camera.hpp"
#include "imgui_impl/glfw.h"
#include "vulkan/renderer.hpp"
#include "imgui_impl/vulkan.hpp"
#ifndef NDEBUG
#include "geometry/test.h"
#endif
#include "geometry/grid.h"
#include "geometry/hopf.h"
#include "geometry/cube.h"
#include "geometry/function.h"
#include "geometry/cylinder.h"
#include "geometry/pipeline.h"
#include "geometry/tesseract.h"
#include "geometry/hexadeca.h"
#include "geometry/icositetra.h"
#include "geometry/pentatope.h"
#include "geometry/spherinder.h"
#include "geometry/kleinBottle.h"
#include "geometry/sphereCone.h"
#include "geometry/hypersphere.h"
#include "geometry/projectivePlane.h"
#define RESOURCE_PATH "./resource/"
struct ImGuiPlaneInput{
    float angle;
    std::string splane;
    mglm::plane plane;
    ImGuiPlaneInput(){
    }
    ImGuiPlaneInput(const std::string&splane){
        this->splane = splane;
    }
};
struct MVP_UBO{
    float view[5][5];
    float model[5][5];
    float projection[5][5];
};
struct ImGuiInput{
    struct{
        bool font = false;
        bool hopf = false;
        bool cube = false;
        bool grid4d = false;
        bool grid3d = false;
        bool sphere = false;
        bool update = false;
        bool remake = false;
        bool function = false;
        bool pipeline = false;
        bool cylinder = false;
        bool tesseract = true;
        bool hexadeca = false;
        bool pentatope = false;
        bool spherinder = false;
        bool sphereCone = false;
        bool icositetra = false;
        bool kleinBottle = false;
        bool hypersphere = false;
        bool realProjectivePlane = false;
#ifndef NDEBUG
        bool test = false;
#endif
        void UnSelect(){
            memset(this, 0, sizeof(*this));
        }
        void SelectCube(){
            UnSelect();
            update = true;
            remake = true;
            cube = true;
        }
        void SelectCylinder(){
            UnSelect();
            update = true;
            remake = true;
            cylinder = true;
        }
        void SelectFont(){
            UnSelect();
            font = true;
            update = true;
            remake = true;
        }
        void SelectFunction(){
            UnSelect();
            update = true;
            remake = true;
            function = true;
        }
        void SelectGrid(bool three = false){
            UnSelect();
            if(three)grid3d = true;
            else grid4d = true;
            update = true;
            remake = true;
        }
        void SelectHopf(){
            UnSelect();
            hopf = true;
            update = true;
            remake = true;
        }
        void SelectHexadeca(){
            UnSelect();
            update = true;
            remake = true;
            hexadeca = true;
        }
        void SelectHypersphere(){
            UnSelect();
            update = true;
            remake = true;
            hypersphere = true;
        }
        void SelectIcositetra(){
            UnSelect();
            update = true;
            remake = true;
            icositetra = true;
        }
        void SelectKleinBottle(){
            UnSelect();
            update = true;
            remake = true;
            kleinBottle = true;
        }
        void SelectPipeline(){
            UnSelect();
            update = true;
            remake = true;
            pipeline = true;
        }
        void SelectPentatope(){
            UnSelect();
            update = true;
            remake = true;
            pentatope = true;
        }
        void SelectRealProjectionPlane(){
            UnSelect();
            update = true;
            remake = true;
            realProjectivePlane = true;
        }
        void SelectSphere(){
            UnSelect();
            sphere = true;
            update = true;
            remake = true;
        }
        void SelectSphereCone(){
            UnSelect();
            update = true;
            remake = true;
            sphereCone = true;
        }
        void SelectSpherinder(){
            UnSelect();
            update = true;
            remake = true;
            spherinder = true;
        }
        void SelectTesseract(){
            UnSelect();
            update = true;
            remake = true;
            tesseract = true;
        }
#ifndef NDEBUG
        void SeletctTest(){
            UnSelect();
            update = true;
            remake = true;
            test = true;
        }
#endif
    }geometry;
    struct{
        bool custom = false;
    }rotateMode;
    struct{
        bool stop = true;
        mglm::plane plane;
        bool randomPlane = false;
    }rotate_animation;
    bool fill = true;
#ifndef USE_PERSPECTIVE
    float scale = 2.0f;
#endif
    UseData parameter;
    bool ortho = false;
    bool mutliView = false;
    bool doubleRotate = false;

    double fps;
    double targetFrameTime = 1.0 / 60.0;
};
struct PushConstant{
    glm::mat4 model;
    glm::mat4 projection;
};
struct GraphicsPipeline{
    //整个程序只需要一个
    vk::PipelineCache cache;
    vk::PipelineLayout layout;
    vk::Pipeline fill, wireframe;
};
//目前没法用
#ifdef ENABE_DEPTH_TEST
struct DepthTest{
    vulkan::Image depth;
    vk::DescriptorSet set;
    vk::DescriptorSetLayout setLayout;
};
#endif
vulkan::Pool g_VulkanPool;
vulkan::Queue g_VulkanQueue;
vulkan::Device g_VulkanDevice;
vulkan::Renderer g_VulkanRenderer;

uint32_t g_WindowWidth, g_WindowHeight;

Camera g_Camera(3);
vulkan::Buffer g_MvpUbo;
GraphicsPipeline g_Pipeline;

std::vector<vk::DescriptorSet>g_Set;
std::vector<vk::DescriptorSetLayout>g_CameraSetLayout(2);

Geometry *g_Geometry;

std::shared_ptr<spdlog::logger>g_Logger = spdlog::basic_logger_mt("logger", "logs/global.logs.txt");

// PickingTexture g_PickingTexture;

vulkan::ImGui g_VulkanImGui;

std::array<ImGuiPlaneInput, 2>g_Plane;

ImGuiInput g_ImGuiInput;

std::string g_FunctionItem[] = {
    "sin", "cos", "tan", "cot", "sec", "csc",
    "sinh", "cosh", "tanh", "coth", "sech", "csch",
    "arcsin", "arccos", "arctan", "arccot", "arcsec", "arccsc",
    "arsinh", "arcosh", "artanh", "arcoth", "arsech", "arcsch",
    "exp", "log", "log10", "log2", "sqrt",
    "z^2", "z^3",
    // 以下是新增的有趣复变函数
    "1/z", "1/z^2", "z + 1/z", "z^2 - 1", "e^z", "sin(z)/z",
    "log(z)", "Gamma", "Riemann Zeta", "J0", "J1", "sin(1/z)",
    "exp(1/z)", "z^(1/2)", "z^(1/3)", "z^i", "z^z", "sinh(1/z)",
    "tan(z/2)", "Weierstrass P", "Airy Ai", "Dawson", "Fresnel C"
};

#ifdef ENABE_DEPTH_TEST
DepthTest g_DepthTest;
#endif
std::array g_CameraPos = { mglm::vec4(0, 0, 0, 3), mglm::vec4(3, 0, 0, 0), mglm::vec4(0, 3, 0, 0), mglm::vec4(0, 0, 3, 0) };
const char *GetPlaneString(const mglm::plane&plane){
    std::string strPlane;
    if(mglm::abs(plane.u) == mglm::vec4(1, 0, 0, 0)){
        strPlane += "X";
    }
    if(mglm::abs(plane.u) == mglm::vec4(0, 1, 0, 0)){
        strPlane += "Y";
    }
    if(mglm::abs(plane.u) == mglm::vec4(0, 0, 1, 0)){
        strPlane += "Z";
    }
    if(mglm::abs(plane.u) == mglm::vec4(0, 0, 0, 1)){
        strPlane += "W";
    }
    if(mglm::abs(plane.v) == mglm::vec4(1, 0, 0, 0)){
        strPlane += "X";
    }
    if(mglm::abs(plane.v) == mglm::vec4(0, 1, 0, 0)){
        strPlane += "Y";
    }
    if(mglm::abs(plane.v) == mglm::vec4(0, 0, 1, 0)){
        strPlane += "Z";
    }
    if(mglm::abs(plane.v) == mglm::vec4(0, 0, 0, 1)){
        strPlane += "W";
    }
    return strPlane.c_str();
}
void SetPlane(const std::string&splane){
    if(splane == "XY"){
        g_Plane[0].plane = mglm::planes::xy;
        g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
    }
    else if(splane == "XZ"){
        g_Plane[0].plane = mglm::planes::xz;
        g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
    }
    else if(splane == "YZ"){
        g_Plane[0].plane = mglm::planes::yz;
        g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
    }
    else if(splane == "XW"){
        g_Plane[0].plane = mglm::planes::xw;
        g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
    }
    else if(splane == "YW"){
        g_Plane[0].plane = mglm::planes::yw;
        g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
    }
    else if(splane == "ZW"){
        g_Plane[0].plane = mglm::planes::zw;
        g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
    }
}
void UpdateUniform(const vulkan::Device&device){
    mglm::mat5 projection;
    if(g_ImGuiInput.ortho){
        projection = mglm::ortho(-1, 1, -1, 1, -1, 1, -1, 1);
    }
    else{
        projection = mglm::perspective(glm::radians(45.0f), glm::radians(45.0f), glm::radians(45.0f), .1f, 100.0f);
    }
    const mglm::mat5 model = mglm::rotate(mglm::mat5(1.0f), glm::radians(g_Plane[0].angle), g_Plane[0].plane, glm::radians(g_Plane[1].angle), g_Plane[1].plane);
    for (size_t i = 0; i < 4; i++){
        mglm::vec4 up = mglm::vec4(0, 1, 0, 0);
        if(mglm::dot(g_CameraPos[i], up) > 1e-6f){
            up = mglm::getOrthogonal(mglm::normalize(g_CameraPos[i]));
        }
        const mglm::mat5 view = mglm::lookAt(g_CameraPos[i], mglm::vec4(0), up);
        MVP_UBO ubo;
        memcpy(ubo.view, view.data, sizeof(float) * 25);
        memcpy(ubo.model, model.data, sizeof(float) * 25);
        memcpy(ubo.projection, projection.data, sizeof(float) * 25);

        g_MvpUbo.UpdateData(device, &ubo, i * g_MvpUbo.GetSize());        
    }
}
void ShowGeometry(){
    const std::array fdCurrentItems = { "超立方体", "正五胞体", "正十六胞体", "正二十四胞体", "四维网格", "球柱体", "球锥体", "超球", "hopf环面", "克莱因瓶", "实射影平面", "贝塞尔管道", "四维字", "复变函数",
#ifndef NDEBUG
        "图元测试"
#endif
    };
    static std::string fdGeometry = fdCurrentItems[0];
    if(ImGui::BeginCombo("四维", fdGeometry.c_str())){
        for (auto currentGeometry = fdCurrentItems.begin(); currentGeometry != fdCurrentItems.end(); ++currentGeometry){
            bool is_selected = fdGeometry == *currentGeometry;
            if (ImGui::Selectable(*currentGeometry, is_selected)){
                fdGeometry = *currentGeometry;
                if(fdGeometry == "超立方体"){
                    g_ImGuiInput.geometry.SelectTesseract();
                }
                else if(fdGeometry == "超球"){
                    g_ImGuiInput.geometry.SelectHypersphere();
                }
                else if(fdGeometry == "正五胞体"){
                    g_ImGuiInput.geometry.SelectPentatope();
                }
                else if(fdGeometry == "正十六胞体"){
                    g_ImGuiInput.geometry.SelectHexadeca();
                }
                else if(fdGeometry == "正二十四胞体"){
                    g_ImGuiInput.geometry.SelectIcositetra();
                }
                else if(fdGeometry == "hopf环面"){
                    g_ImGuiInput.geometry.SelectHopf();
                }
                else if(fdGeometry == "克莱因瓶"){
                    g_ImGuiInput.geometry.SelectKleinBottle();
                }
                else if(fdGeometry == "球柱体"){
                    g_ImGuiInput.geometry.SelectSpherinder();
                }
                else if(fdGeometry == "球锥体"){
                    g_ImGuiInput.geometry.SelectSphereCone();
                }
                else if(fdGeometry == "四维字"){
                    g_ImGuiInput.geometry.SelectFont();
                }
                else if(fdGeometry == "贝塞尔管道"){
                    g_ImGuiInput.geometry.SelectPipeline();
                }
                else if(fdGeometry == "四维网格"){
                    g_ImGuiInput.fill = false;
                    g_ImGuiInput.geometry.SelectGrid();
                }
                else if(fdGeometry == "实射影平面"){
                    g_ImGuiInput.geometry.SelectRealProjectionPlane();
                }
                else if(fdGeometry == "复变函数"){
                    g_ImGuiInput.ortho = true;
                    g_ImGuiInput.geometry.SelectFunction();
                }
#ifndef NDEBUG
                else if(fdGeometry == "图元测试"){
                    g_ImGuiInput.geometry.SeletctTest();
                }
#endif
                break;
            }
        }
        ImGui::EndCombo();
    }
    const std::array tdCurrentItems = { "立方体", "球", "圆柱体", "三维网格" };
    static std::string tdGeometry = tdCurrentItems[0];
    if(ImGui::BeginCombo("三维", tdGeometry.c_str())){
        for (auto currentGeometry = tdCurrentItems.begin(); currentGeometry != tdCurrentItems.end(); ++currentGeometry){
            bool is_selected = tdGeometry == *currentGeometry;
            if (ImGui::Selectable(*currentGeometry, is_selected)){
                tdGeometry = *currentGeometry;
                if(tdGeometry == "球"){
                    g_ImGuiInput.geometry.SelectSphere();
                }
                else if(tdGeometry == "立方体"){
                    g_ImGuiInput.geometry.SelectCube();
                }
                else if(tdGeometry == "圆柱体"){
                    g_ImGuiInput.geometry.SelectCylinder();
                }
                else if(tdGeometry == "三维网格"){
                    g_ImGuiInput.fill = false;
                    g_ImGuiInput.geometry.SelectGrid(true);
                }
                break;
            }
        }
        ImGui::EndCombo();
    }
}
void ShowPlaneCombo(const char *lable){
    const std::vector<std::string> currentItems = { "XY", "XZ", "YZ", "XW", "YW", "ZW" };
    static std::string plane = currentItems[0];
    if(ImGui::BeginCombo(lable, plane.c_str())){
        for (auto currentPlane = currentItems.begin(); currentPlane != currentItems.end(); ++currentPlane){
            bool is_selected = plane == *currentPlane;
            if (ImGui::Selectable(currentPlane->c_str(), is_selected)){
                plane = *currentPlane;
                UpdateUniform(g_VulkanDevice);
                break;
            }
        }
        ImGui::EndCombo();
    }
    SetPlane(plane);
}
void ShowRotate(){
    ImGui::BeginDisabled(!g_ImGuiInput.rotate_animation.stop);
    if(g_ImGuiInput.rotateMode.custom){
        static float planeVec[][4] = { {g_Plane[0].plane.u.x, g_Plane[0].plane.u.y, g_Plane[0].plane.u.z, g_Plane[0].plane.u.w}, {g_Plane[0].plane.v.x, g_Plane[0].plane.v.y, g_Plane[0].plane.v.z, g_Plane[0].plane.v.w} };
        if(!g_ImGuiInput.rotate_animation.stop){
            planeVec[0][0] = g_Plane[0].plane.u.x;
            planeVec[0][1] = g_Plane[0].plane.u.y;
            planeVec[0][2] = g_Plane[0].plane.u.z;
            planeVec[0][3] = g_Plane[0].plane.u.w;

            planeVec[1][0] = g_Plane[1].plane.v.x;
            planeVec[1][1] = g_Plane[1].plane.v.y;
            planeVec[1][2] = g_Plane[1].plane.v.z;
            planeVec[1][3] = g_Plane[1].plane.v.w;            
        }
        ImGui::Text("平面一");
        if(ImGui::InputFloat4("u", planeVec[0])){
            g_Plane[0].plane.u = mglm::vec4(planeVec[0][0], planeVec[0][1], planeVec[0][2], planeVec[0][3]);
        }
        ImGui::SameLine();
        if(ImGui::Button("获取v")){
            g_Plane[0].plane.v = mglm::getOrthogonal(g_Plane[0].plane.u);
            planeVec[1][0] = g_Plane[0].plane.v.x;
            planeVec[1][1] = g_Plane[0].plane.v.y;
            planeVec[1][2] = g_Plane[0].plane.v.z;
            planeVec[1][3] = g_Plane[0].plane.v.w;
        }
        if(ImGui::InputFloat4("v", planeVec[1])){
            g_Plane[0].plane.v = mglm::vec4(planeVec[1][0], planeVec[1][1], planeVec[1][2], planeVec[1][3]);
        }
        ImGui::SameLine();
        if(ImGui::Button("获取u")){
            g_Plane[0].plane.u = mglm::getOrthogonal(g_Plane[0].plane.v);
            planeVec[0][0] = g_Plane[1].plane.v.x;
            planeVec[0][1] = g_Plane[1].plane.v.y;
            planeVec[0][2] = g_Plane[1].plane.v.z;
            planeVec[0][3] = g_Plane[1].plane.v.w;
        }
        ImGui::Text("两个向量的点乘:%f", mglm::dot(g_Plane[0].plane.u, g_Plane[0].plane.v));
        ImGui::Text("平面二:\nu:%f, %f, %f, %f\nv:%f, %f, %f, %f", g_Plane[1].plane.u.x, g_Plane[1].plane.u.y, g_Plane[1].plane.u.z, g_Plane[1].plane.u.w, g_Plane[1].plane.v.x, g_Plane[1].plane.v.y, g_Plane[1].plane.v.z, g_Plane[1].plane.v.w);
        if(ImGui::Button("获取平面二")){
            g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
        }
    }
    else{
        ShowPlaneCombo("平面一");
        ImGui::SameLine();
        ImGui::Text("平面二:%s", GetPlaneString(g_Plane[1].plane));
    }
    ImGui::EndDisabled();
    ImGui::Checkbox("双旋转", &g_ImGuiInput.doubleRotate);
    ImGui::BeginDisabled(!g_ImGuiInput.rotate_animation.stop);
    if(ImGui::SliderFloat("平面一角度", &g_Plane[0].angle, 0, 360)){
        if(g_ImGuiInput.doubleRotate){
            g_Plane[1].angle = g_Plane[0].angle;
        }
        UpdateUniform(g_VulkanDevice);
    }
    if(ImGui::SliderFloat("平面二角度", &g_Plane[1].angle, 0, 360)){
        if(g_ImGuiInput.doubleRotate){
            g_Plane[0].angle = g_Plane[1].angle;
        }
        UpdateUniform(g_VulkanDevice);
    }
    ImGui::EndDisabled();
}
//因为我们只启动一次线程, 直到线程退出才会启动新线程, 所以需要全局变量
void RotateAnimation(){
    float angle = 0;
    mglm::plane plane;
    constexpr auto targetFrameDuration = std::chrono::milliseconds(25);
    while (!g_ImGuiInput.rotate_animation.stop){
        auto frameStart = std::chrono::steady_clock::now();
        g_Plane[0].angle = angle;
        if(g_ImGuiInput.doubleRotate)g_Plane[1].angle = angle;
        if(g_ImGuiInput.rotate_animation.randomPlane){
            g_ImGuiInput.rotate_animation.randomPlane = false;
            do{
                plane.u = mglm::vec4(rand() % 2, rand() % 2, rand() % 2, rand() % 2);                
            } while (mglm::length(plane.u) < 1e-6f);
            plane.v = mglm::getOrthogonal(plane.u);
        }
        if(g_ImGuiInput.rotate_animation.plane.u == mglm::vec4(0) && g_ImGuiInput.rotate_animation.plane.v == mglm::vec4(0)){
            g_Plane[0].plane = plane;
            g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
        }
        else{
            g_Plane[0].plane = g_ImGuiInput.rotate_animation.plane;
            g_Plane[1].plane = mglm::getOrthogonalPlane(g_Plane[0].plane);
        }
        angle = ((uint32_t)angle + 1) % 360;
        UpdateUniform(g_VulkanDevice);
        auto elapsed = std::chrono::steady_clock::now() - frameStart;
        auto remaining = targetFrameDuration - elapsed;
        if (remaining > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(remaining);
        }
    }
}
std::function<std::complex<float>(const std::complex<float>&)>GetFunction(const std::string& function){
    constexpr std::complex<float> I(0.0f, 1.0f);
    
    // 三角函数
    if(function == "sin"){
        return [](const std::complex<float>& z){ return std::sin(z); };
    }
    else if(function == "cos"){
        return [](const std::complex<float>& z){ return std::cos(z); };
    }
    else if(function == "tan"){
        return [](const std::complex<float>& z){ return std::tan(z); };
    }
    else if(function == "cot"){
        return [](const std::complex<float>& z){ return 1.0f / std::tan(z); };
    }
    else if(function == "sec"){
        return [](const std::complex<float>& z){ return 1.0f / std::cos(z); };
    }
    else if(function == "csc"){
        return [](const std::complex<float>& z){ return 1.0f / std::sin(z); };
    }
    
    // 双曲函数
    else if(function == "sinh"){
        return [](const std::complex<float>& z){ return std::sinh(z); };
    }
    else if(function == "cosh"){
        return [](const std::complex<float>& z){ return std::cosh(z); };
    }
    else if(function == "tanh"){
        return [](const std::complex<float>& z){ return std::tanh(z); };
    }
    else if(function == "coth"){
        return [](const std::complex<float>& z){ return 1.0f / std::tanh(z); };
    }
    else if(function == "sech"){
        return [](const std::complex<float>& z){ return 1.0f / std::cosh(z); };
    }
    else if(function == "csch"){
        return [](const std::complex<float>& z){ return 1.0f / std::sinh(z); };
    }
    
    // 反三角函数
    else if(function == "arcsin"){
        return [I](const std::complex<float>& z){ return -I * std::log(I*z + std::sqrt(1.0f - z*z)); };
    }
    else if(function == "arccos"){
        return [I](const std::complex<float>& z){ return -I * std::log(z + I*std::sqrt(1.0f - z*z)); };
    }
    else if(function == "arctan"){
        return [I](const std::complex<float>& z){ return 0.5f*I * std::log((I + z)/(I - z)); };
    }
    else if(function == "arccot"){
        return [I](const std::complex<float>& z){ return 0.5f*I * std::log((z - I)/(z + I)); };
    }
    else if(function == "arcsec"){
        return [I](const std::complex<float>& z){ return -I * std::log(1.0f/z + I*std::sqrt(1.0f - 1.0f/(z*z))); };
    }
    else if(function == "arccsc"){
        return [I](const std::complex<float>& z){ return -I * std::log(I/z + std::sqrt(1.0f - 1.0f/(z*z))); };
    }
    
    // 反双曲函数
    else if(function == "arsinh"){
        return [](const std::complex<float>& z){ return std::asinh(z); };
    }
    else if(function == "arcosh"){
        return [](const std::complex<float>& z){ return std::acosh(z); };
    }
    else if(function == "artanh"){
        return [](const std::complex<float>& z){ return std::atanh(z); };
    }
    else if(function == "arcoth"){
        return [](const std::complex<float>& z){ return 0.5f * std::log((z + 1.0f)/(z - 1.0f)); };
    }
    else if(function == "arsech"){
        return [](const std::complex<float>& z){ return std::log((1.0f + std::sqrt(1.0f - z*z))/z); };
    }
    else if(function == "arcsch"){
        return [](const std::complex<float>& z){ return std::log((1.0f + std::sqrt(1.0f + z*z))/z); };
    }
    
    // 基本函数
    else if(function == "exp"){
        return [](const std::complex<float>& z){ return std::exp(z); };
    }
    else if(function == "log"){
        return [](const std::complex<float>& z){ return std::log(z); };
    }
    else if(function == "log10"){
        return [](const std::complex<float>& z){ return std::log10(z); };
    }
    else if(function == "log2"){
        return [](const std::complex<float>& z){ return std::log(z) / std::log(2.0f); };
    }
    else if(function == "sqrt"){
        return [](const std::complex<float>& z){ return std::sqrt(z); };
    }
    
    // 幂函数
    else if(function == "z^2"){
        return [](const std::complex<float>& z){ return z*z; };
    }
    else if(function == "z^3"){
        return [](const std::complex<float>& z){ return z*z*z; };
    }
    
    // 以下是新增的有趣复变函数
    else if(function == "1/z"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-10f) return std::complex<float>(1e10f, 0.0f); // 处理奇点
            return 1.0f/z; 
        };
    }
    else if(function == "1/z^2"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-10f) return std::complex<float>(1e10f, 0.0f);
            return 1.0f/(z*z); 
        };
    }
    else if(function == "z + 1/z"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-10f) return std::complex<float>(1e10f, 0.0f);
            return z + 1.0f/z; 
        };
    }
    else if(function == "z^2 - 1"){
        return [](const std::complex<float>& z){ return z*z - 1.0f; };
    }
    else if(function == "e^z"){
        return [](const std::complex<float>& z){ return std::exp(z); };
    }
    else if(function == "sin(z)/z"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-6f) return std::complex<float>(1.0f, 0.0f); // sinc(0) = 1
            return std::sin(z)/z; 
        };
    }
    else if(function == "log(z)"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-10f) return std::complex<float>(-1e10f, 0.0f);
            return std::log(z); 
        };
    }
    else if(function == "Gamma"){
        // Lanczos 近似，适用于实部和虚部较小的复数
        return [](const std::complex<float>& z){
            if(z.real() <= 0.0f && std::abs(std::fmod(z.real(), 1.0f)) < 1e-6f) 
                return std::complex<float>(1e10f, 0.0f);
            
            // Lanczos 常数 g = 6.0
            static constexpr float g = 6.0f;
            
            // Lanczos 系数，来自 Godfrey 的 "An Atlas of Functions"
            static constexpr float p[7] = {
                1.000000000190015f,
                76.18009172947146f,
                -86.50532032941677f,
                24.01409824083091f,
                -1.231739572450155f,
                0.1208650973866179e-2f,
                -0.5395239384953e-5f
            };
            
            std::complex<float> x = z;
            std::complex<float> tmp = x + g - 0.5f;  // g + 0.5
            tmp = (x - 0.5f) * std::log(tmp) - tmp;
            
            std::complex<float> ser = p[0];
            for(int i = 1; i < 7; ++i) {
                ser += p[i] / (x + static_cast<float>(i));
            }
            
            return std::exp(tmp) * glm::root_pi<float>() * ser / x;
        };
    }
    else if(function == "Riemann Zeta"){
        // 简化的 Riemann Zeta 函数近似(仅用于可视化，精度有限)
        return [](const std::complex<float>& z){
            int n = 30;
            std::complex<float> sum = 0.0f;
            for(int k=1; k<=n; k++){
                sum += 1.0f / std::pow(std::complex<float>(float(k), 0.0f), z);
            }
            return sum;
        };
    }
    else if(function == "J0"){
        // 零阶贝塞尔函数近似
        return [](const std::complex<float>& z){
            std::complex<float> sum = 0.0f;
            std::complex<float> term = 1.0f;
            for(int n=1; n<=20; n++){
                term *= -z*z/(4.0f*n*n);
                sum += term;
            }
            return 1.0f + sum;
        };
    }
    else if(function == "J1"){
        // 一阶贝塞尔函数近似
        return [](const std::complex<float>& z){
            if(std::abs(z) < 1e-6f) return std::complex<float>(0.0f, 0.0f);
            std::complex<float> sum = 0.0f;
            std::complex<float> term = z/2.0f;
            for(int n=1; n<=20; n++){
                term *= -z*z/(4.0f*n*(n+1));
                sum += term;
            }
            return z/2.0f + sum;
        };
    }
    else if(function == "sin(1/z)"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-10f) return std::complex<float>(0.0f, 0.0f);
            return std::sin(1.0f/z); 
        };
    }
    else if(function == "exp(1/z)"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-10f) return std::complex<float>(0.0f, 0.0f);
            return std::exp(1.0f/z); 
        };
    }
    else if(function == "z^(1/2)"){
        return [](const std::complex<float>& z){ return std::sqrt(z); };
    }
    else if(function == "z^(1/3)"){
        return [](const std::complex<float>& z){ return std::pow(z, 1.0f/3.0f); };
    }
    else if(function == "z^i"){
        return [I](const std::complex<float>& z){ return std::pow(z, I); };
    }
    else if(function == "z^z"){
        return [](const std::complex<float>& z){ return std::pow(z, z); };
    }
    else if(function == "sinh(1/z)"){
        return [](const std::complex<float>& z){ 
            if(std::abs(z) < 1e-10f) return std::complex<float>(0.0f, 0.0f);
            return std::sinh(1.0f/z); 
        };
    }
    else if(function == "tan(z/2)"){
        return [](const std::complex<float>& z){ return std::tan(z/2.0f); };
    }
    else if(function == "Weierstrass P"){
        // 简化的 Weierstrass ℘ 函数近似
        return [](const std::complex<float>& z){
            if(std::abs(z) < 1e-6f) return std::complex<float>(1e10f, 0.0f);
            std::complex<float> sum = 1.0f/(z*z);
            for(int n=-3; n<=3; n++){
                for(int m=-3; m<=3; m++){
                    if(n==0 && m==0) continue;
                    std::complex<float> w = std::complex<float>(2.0f*n, 2.0f*m);
                    sum += 1.0f/((z-w)*(z-w)) - 1.0f/(w*w);
                }
            }
            return sum;
        };
    }
    else if(function == "Airy Ai"){
        // Airy Ai 函数近似
        return [](const std::complex<float>& z){
            std::complex<float> term = 1.0f;
            std::complex<float> sum = term;
            for(int n=1; n<=20; n++){
                term *= z*z*z/(3.0f*n*(3.0f*n-1.0f));
                sum += term;
            }
            return 0.355028053887817f * sum; // 1/(3^(2/3)*Γ(2/3))
        };
    }
    else if(function == "Dawson"){
        // Dawson 积分函数近似
        return [](const std::complex<float>& z){
            if(std::abs(z) < 1e-6f) return z;
            std::complex<float> sum = 0.0f;
            std::complex<float> term = 1.0f;
            for(int n=0; n<20; n++){
                term *= -2.0f*z*z/(2.0f*n+1.0f);
                sum += term;
            }
            return std::exp(-z*z) * sum;
        };
    }
    else if(function == "Fresnel C"){
        // Fresnel 余弦积分近似
        return [](const std::complex<float>& z){
            std::complex<float> sum = 0.0f;
            std::complex<float> term = z;
            for(int n=0; n<20; n++){
                term *= -glm::pi<float>() *z*z*z*z/(4.0f*(2.0f*n+1.0f)*(n+1.0f)*(2.0f*n+3.0f));
                sum += term;
            }
            return sum;
        };
    }
    
    return [](const std::complex<float>& z){ return z; };
}

void UpdateImGui(vk::CommandBuffer command){
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("视图")){
            ImGui::MenuItem("多视图", nullptr, &g_ImGuiInput.mutliView);
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("旋转")){
            ImGui::MenuItem("自定义", nullptr, &g_ImGuiInput.rotateMode.custom);
            if(ImGui::BeginMenu("动画")){
                if(ImGui::MenuItem("停止", nullptr, &g_ImGuiInput.rotate_animation.stop)){
                    g_ImGuiInput.rotate_animation.plane = mglm::plane();
                }
                if(ImGui::MenuItem("XY", nullptr, g_ImGuiInput.rotate_animation.plane == mglm::planes::xy)){
                    g_ImGuiInput.rotate_animation.plane = mglm::planes::xy;
                    g_ImGuiInput.rotateMode.custom = false;
                    if(g_ImGuiInput.rotate_animation.stop){
                        g_ImGuiInput.rotate_animation.stop = false;
                        std::thread thread(RotateAnimation);
                        thread.detach();
                    }
                }
                if(ImGui::MenuItem("XZ", nullptr, g_ImGuiInput.rotate_animation.plane == mglm::planes::xz)){
                    g_ImGuiInput.rotate_animation.plane = mglm::planes::xz;
                    g_ImGuiInput.rotateMode.custom = false;
                    if(g_ImGuiInput.rotate_animation.stop){
                        g_ImGuiInput.rotate_animation.stop = false;
                        std::thread thread(RotateAnimation);
                        thread.detach();
                    }
                }
                if(ImGui::MenuItem("YZ", nullptr, g_ImGuiInput.rotate_animation.plane == mglm::planes::yz)){
                    g_ImGuiInput.rotate_animation.plane = mglm::planes::yz;
                    g_ImGuiInput.rotateMode.custom = false;
                    if(g_ImGuiInput.rotate_animation.stop){
                        g_ImGuiInput.rotate_animation.stop = false;
                        std::thread thread(RotateAnimation);
                        thread.detach();
                    }
                }
                if(ImGui::MenuItem("XW", nullptr, g_ImGuiInput.rotate_animation.plane == mglm::planes::xw)){
                    g_ImGuiInput.rotate_animation.plane = mglm::planes::xw;
                    g_ImGuiInput.rotateMode.custom = false;
                    if(g_ImGuiInput.rotate_animation.stop){
                        g_ImGuiInput.rotate_animation.stop = false;
                        std::thread thread(RotateAnimation);
                        thread.detach();
                    }
                }
                if(ImGui::MenuItem("YW", nullptr, g_ImGuiInput.rotate_animation.plane == mglm::planes::yw)){
                    g_ImGuiInput.rotate_animation.plane = mglm::planes::yw;
                    g_ImGuiInput.rotateMode.custom = false;
                    if(g_ImGuiInput.rotate_animation.stop){
                        g_ImGuiInput.rotate_animation.stop = false;
                        std::thread thread(RotateAnimation);
                        thread.detach();
                    }
                }
                if(ImGui::MenuItem("ZW", nullptr, g_ImGuiInput.rotate_animation.plane == mglm::planes::zw)){
                    g_ImGuiInput.rotate_animation.plane = mglm::planes::zw;
                    g_ImGuiInput.rotateMode.custom = false;
                    if(g_ImGuiInput.rotate_animation.stop){
                        g_ImGuiInput.rotate_animation.stop = false;
                        std::thread thread(RotateAnimation);
                        thread.detach();
                    }
                }
                if(ImGui::MenuItem("随机")){
                    g_ImGuiInput.rotateMode.custom = true;
                    g_ImGuiInput.rotate_animation.randomPlane = true;
                    g_ImGuiInput.rotate_animation.plane = mglm::plane();
                    if(g_ImGuiInput.rotate_animation.stop){
                        g_ImGuiInput.rotate_animation.stop = false;
                        std::thread thread(RotateAnimation);
                        thread.detach();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if(ImGui::Begin("四维几何")){
        ImGui::Text("fps:%f", g_ImGuiInput.fps);
        ImGui::BeginDisabled(g_ImGuiInput.geometry.grid3d || g_ImGuiInput.geometry.grid4d);
        if(ImGui::Checkbox("填充", &g_ImGuiInput.fill)){
            UpdateUniform(g_VulkanDevice);
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(g_ImGuiInput.geometry.function);
        if(ImGui::Checkbox("正交投影", &g_ImGuiInput.ortho)){
            UpdateUniform(g_VulkanDevice);
        }
        ImGui::EndDisabled();
        static float cameraPos[][4] = {
            {g_CameraPos[0].x, g_CameraPos[0].y, g_CameraPos[0].z, g_CameraPos[0].w },
            {g_CameraPos[1].x, g_CameraPos[1].y, g_CameraPos[1].z, g_CameraPos[1].w },
            {g_CameraPos[2].x, g_CameraPos[2].y, g_CameraPos[2].z, g_CameraPos[2].w },
            {g_CameraPos[3].x, g_CameraPos[3].y, g_CameraPos[3].z, g_CameraPos[3].w }
        };
        for (size_t i = 0; i < (g_ImGuiInput.mutliView?4:1); i++){
            char lable[0xff] = {0};
            sprintf(lable, "四维摄像机%d", i);
            if(ImGui::InputFloat4(lable, cameraPos[i])){
                g_CameraPos[i] = mglm::vec4(cameraPos[i][0], cameraPos[i][1], cameraPos[i][2], cameraPos[i][3]);
                UpdateUniform(g_VulkanDevice);
            }
        }
#ifndef USE_PERSPECTIVE
        if(ImGui::InputFloat("缩放", &g_ImGuiInput.scale)){
            UpdateUniform(g_VulkanDevice);
        }
#endif
        ImGui::SeparatorText("四维旋转");
        ShowRotate();
        ImGui::SeparatorText("几何");
        ShowGeometry();
        ImGui::SeparatorText("几何参数");
        if(g_ImGuiInput.geometry.kleinBottle){
            if(ImGui::SliderInt("扭转次数", &g_ImGuiInput.parameter.kleinbottle.twistLoops, 0, 5)){
                g_ImGuiInput.geometry.update = true;
            }
        }
        if(g_ImGuiInput.geometry.function){
            static std::string currentFunctionItem = g_FunctionItem[2];
            if(ImGui::SliderFloat("range", &g_ImGuiInput.parameter.function.range, 1, 5)){
                g_ImGuiInput.geometry.update = true;
            }
            static char functionString[0xff];
            ImGui::InputText("复变函数 ", functionString, 0xff);
            ImGui::Text("%s", g_ImGuiInput.parameter.function.error.c_str());
            if(ImGui::Button("确定")){
                g_ImGuiInput.geometry.update = true;
                g_ImGuiInput.parameter.function.function = functionString;
            }
            if(ImGui::BeginCombo("复变函数", currentFunctionItem.c_str())){
                for (uint32_t i = 0; i < sizeof(g_FunctionItem) / sizeof(std::string); ++i){
                    bool is_selected = currentFunctionItem == g_FunctionItem[i];
                    if (ImGui::Selectable(g_FunctionItem[i].c_str(), is_selected)){
                        g_ImGuiInput.geometry.update = true;
                        currentFunctionItem = g_FunctionItem[i];
                        g_ImGuiInput.parameter.function.function = "";
                        g_ImGuiInput.parameter.function.fun = GetFunction(currentFunctionItem);
                        break;
                    }
                }
                ImGui::EndCombo();
            } 
        }
        if(g_ImGuiInput.geometry.hopf){
            if(ImGui::Checkbox("clifford环面", &g_ImGuiInput.parameter.hopf.clifford)){
                g_ImGuiInput.parameter.hopf.torusAspect = CLIFFORD_R;
                g_ImGuiInput.geometry.update = true;
            }
            if(!g_ImGuiInput.parameter.hopf.clifford){
                if(ImGui::SliderFloat("主圆半径", &g_ImGuiInput.parameter.hopf.torusAspect, 0, 1)){
                    g_ImGuiInput.geometry.update = true;
                }
            }
            // if(ImGui::SliderInt("lobeCount", &g_ImGuiInput.parameter.hopf.lobeCount, 0, 5)){
            //     g_ImGuiInput.geometry.update = true;
            // }
            // if(ImGui::SliderFloat("时间", &g_ImGuiInput.parameter.hopf.time, 0, 1)){
            //     g_ImGuiInput.geometry.update = true;
            // }
        }
        if(g_ImGuiInput.geometry.pipeline || g_ImGuiInput.geometry.font){
            if(ImGui::InputFloat("半径", &g_ImGuiInput.parameter.bezier.radius)){
                g_Geometry->Update(&g_ImGuiInput.parameter);
            }
            for (size_t i = 0; i < 4; i++){
                char label[0xff];
                sprintf(label, "点%d", i);
                if(ImGui::InputFloat4(label, g_ImGuiInput.parameter.bezier.point[i].data())){
                    g_Geometry->Update(&g_ImGuiInput.parameter);
                }
            }
        }
    }
    ImGui::End();
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool isMinimized = (draw_data->DisplaySize.x <=.0f || draw_data->DisplaySize.y <= .0f);
    if(!isMinimized)g_VulkanImGui.RenderDrawData(command, draw_data);
}
glm::uvec2 calculateViewport(uint32_t windowWidth, uint32_t windowHeight, vk::Extent2D&exent){
    const float targetAspect = 1.0f;
    
    glm::uvec2 offset = glm::uvec2(0);
    exent.width = windowWidth;
    exent.height = windowHeight;
    const float windowAspect = (float)windowWidth / (float)windowHeight;

    if (windowAspect > targetAspect) {
        exent.height = windowHeight;
        exent.width = static_cast<int>(windowHeight * targetAspect);
        offset.x = (windowWidth - exent.width) / 2;
    } else {
        exent.width = windowWidth;
        exent.height = static_cast<int>(windowWidth / targetAspect);
        offset.y = (windowHeight - exent.height) / 2;
    }
    return offset;
}
void updateViewport(vk::CommandBuffer command, uint32_t windowWidth, uint32_t windowHeight) {
    vk::Extent2D extent;
    auto offset = calculateViewport(windowWidth, windowHeight, extent);
    command.setScissor(0, vulkan::pipeline::initializers::scissor(extent.width, extent.height, offset.x, offset.y));
    command.setViewport(0, vulkan::pipeline::initializers::viewport(extent.width, extent.height, offset.x, offset.y));
}
void Draw(vk::CommandBuffer command, const vk::Pipeline *pipeline, uint32_t count){
    PushConstant pc;
#ifdef USE_PERSPECTIVE
    pc.model = glm::mat4(1.0f);
    pc.projection = glm::perspective(glm::radians(45.0f), 1.0f, .1f, 100.0f);
    pc.projection[1][1] *= -1;
#else
    pc.model = glm::scale(glm::mat4(1.0f), glm::vec3(g_ImGuiInput.scale));
    const float distance = mglm::distance(g_CameraPos[0], mglm::vec4(0));
    pc.projection = glm::ortho(-distance, distance, -distance, distance, -distance * 10, distance * 10);
#endif
    if(g_ImGuiInput.fill){
        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline[0]);
    }
    else{
        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline[1]);
    }
    command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, g_Pipeline.layout, 0, g_Set[0], {});
#ifdef ENABE_DEPTH_TEST
    command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, g_Pipeline.layout, 2, g_DepthTest.set, {});
#endif
    vkCmdPushConstants(command, g_Pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &pc);
    uint32_t dynamicOffsets = 0;
    if(g_ImGuiInput.mutliView){
        vk::Extent2D extent;
        calculateViewport(g_WindowWidth, g_WindowHeight, extent);
        for(uint32_t i = 0; i < 4; ++i){
            dynamicOffsets = i * g_MvpUbo.GetSize();
            const uint32_t width = extent.width / 2;
            const uint32_t height = extent.height / 2;
            const uint32_t offsetX = (i % 2) * extent.width / 2;
            const uint32_t offsetY = (i / 2) * extent.height / 2;
            command.setScissor(0, vulkan::pipeline::initializers::scissor(width, height, offsetX, offsetY));
            command.setViewport(0, vulkan::pipeline::initializers::viewport(width, height, offsetX, offsetY));
            command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, g_Pipeline.layout, 1, g_Set[1], dynamicOffsets);

            if(g_ImGuiInput.fill){
                g_Geometry->Draw(command, g_Pipeline.layout);
            }
            else{
                g_Geometry->DrawWireframe(command, g_Pipeline.layout);
            }
        }
    }
    else{
        updateViewport(command, g_WindowWidth, g_WindowHeight);
        command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, g_Pipeline.layout, 1, g_Set[1], dynamicOffsets);
        if(g_ImGuiInput.fill){
            g_Geometry->Draw(command, g_Pipeline.layout);
        }
        else{
            g_Geometry->DrawWireframe(command, g_Pipeline.layout);
        }
    }
}
#ifdef ENABE_DEPTH_TEST
void ClearDepthMap(vk::CommandBuffer command){
    vk::ClearColorValue clearColor = {};
    clearColor.float32[0] = 1.0f;

    vk::ImageSubresourceRange range{};
    range.aspectMask = vk::ImageAspectFlagBits::eColor;
    range.levelCount = 1;
    range.layerCount = 1;
    g_DepthTest.depth.SetLayout(command, vk::ImageLayout::eGeneral, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferWrite);
    command.clearColorImage(g_DepthTest.depth.GetImage(), vk::ImageLayout::eGeneral, clearColor, range);
}
#endif
void RecordCommand(vk::CommandBuffer command, vulkan::Image&color, vulkan::Image&depth){
    vulkan::framework::BeginCommands(command, vk::CommandBufferUsageFlagBits::eSimultaneousUse);
#ifdef ENABE_DEPTH_TEST
    ClearDepthMap(command);
#endif
    vulkan::framework::BeginRendering(command, {&color}, {depth}, {g_WindowWidth, g_WindowHeight});

    std::array pipeline = { g_Pipeline.fill,  g_Pipeline.wireframe };
    Draw(command, pipeline.data(), pipeline.size());

    UpdateImGui(command);

    vulkan::framework::EndRendering(command, color);
    command.end();
}
void RecordCommand(vk::CommandBuffer command, vk::Framebuffer frame, vk::RenderPass renderPass){
    vulkan::framework::BeginCommands(command, vk::CommandBufferUsageFlagBits::eSimultaneousUse);
#ifdef ENABE_DEPTH_TEST
    ClearDepthMap(command);
#endif
    vulkan::framework::BeginRenderPass(command, frame, renderPass, {g_WindowWidth, g_WindowHeight});

    std::array pipeline = { g_Pipeline.fill,  g_Pipeline.wireframe };
    Draw(command, pipeline.data(), pipeline.size());

    UpdateImGui(command);

    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}

void keybutton(GLFWwindow *window, int key, int scancode, int action, int mods){

}
void mousescroll(GLFWwindow *window, double xoffset, double yoffset){
#ifdef USE_PERSPECTIVE
    g_Camera.ProcessMouseScroll(yoffset);
    g_Camera.UpdateUniform(g_VulkanDevice);
#endif
}
glm::vec2 g_MousePos;
void mousecursor(GLFWwindow *window, double xpos, double ypos){
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glm::vec2 current(xpos, ypos);
        glm::vec2 delta = current - g_MousePos;
        g_MousePos = current;
        g_Camera.ProcessMouseMovement(delta.x, delta.y);
        g_Camera.UpdateUniform(g_VulkanDevice);
    }
}
void mousebutton(GLFWwindow *window,int button,int action,int mods){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if(action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT){
        g_MousePos = glm::vec2(xpos, ypos);
    }
}

void DestroyGraphicsPipeline(vk::Device device){
    vkDestroyPipeline(device, g_Pipeline.fill, VK_NULL_HANDLE);
    vkDestroyPipeline(device, g_Pipeline.wireframe, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(device, g_Pipeline.layout, VK_NULL_HANDLE);
}
void CreatePipelineLayout(vk::Device device){
    std::array setLayout = { g_CameraSetLayout[0], g_CameraSetLayout[1]
#ifdef ENABE_DEPTH_TEST
        ,g_DepthTest.setLayout
#endif
    };
    auto info = vulkan::pipeline::initializers::pipelineLayoutCreateInfo(setLayout.size(), 1);
    auto pushConstantRange = vulkan::pipeline::initializers::pushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(PushConstant), 0);
    info.pSetLayouts = setLayout.data();
    info.pPushConstantRanges = &pushConstantRange;
    g_Pipeline.layout = device.createPipelineLayout(info);
}
void CreateGraphicsPipeline(vk::Device device, VkPipelineLayout layout){
    vk::PipelineVertexInputStateCreateInfo inputState{};
    std::array<vk::PipelineShaderStageCreateInfo,2> shaderStages;
    auto bindingDescriptions = Vertex::inputBindingDescription(0);
    auto attributeDescriptions = Vertex::inputAttributeDescriptions(0);
    vk::PipelineViewportStateCreateInfo viewportState = vulkan::pipeline::initializers::pipelineViewportStateCreateInfo(1, 1);
    vk::GraphicsPipelineCreateInfo info = vulkan::pipeline::initializers::pipelineCreateInfo(layout, g_VulkanRenderer.GetRenderPass());
    const vk::ColorComponentFlags colorWriteMask = vk::ColorComponentFlagBits::eR| vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendAttachmentState blendAttachmentState = vulkan::pipeline::initializers::pipelineColorBlendAttachmentState(colorWriteMask, vk::False);
    vk::PipelineColorBlendStateCreateInfo colorBlendState = vulkan::pipeline::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
    vk::PipelineMultisampleStateCreateInfo multisampleState = vulkan::pipeline::initializers::pipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);
    vk::PipelineDepthStencilStateCreateInfo depthStencilState = vulkan::pipeline::initializers::pipelineDepthStencilStateCreateInfo(vk::True, vk::True, vk::CompareOp::eLessOrEqual);
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = vulkan::pipeline::initializers::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, vk::False);
    vk::PipelineRasterizationStateCreateInfo rasterizationState = vulkan::pipeline::initializers::pipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);
    std::array<vk::DynamicState, 2> dynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState = vulkan::pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()));
    // vk::Rect2D scissors = vulkan::pipeline::initializers::scissor(g_WindowWidth, g_WindowHeight);
    // vk::Viewport viewport = vulkan::pipeline::initializers::viewport(g_WindowWidth, g_WindowHeight);
    // viewportState.pScissors = &scissors;
    // viewportState.pViewports = &viewport;

    inputState.sType =  vk::StructureType::ePipelineVertexInputStateCreateInfo;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.pVertexBindingDescriptions = &bindingDescriptions;
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
    inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();
    shaderStages[0] = vulkan::pipeline::tools::loadShader(device, RESOURCE_PATH"shaders/base.vert.spv", vk::ShaderStageFlagBits::eVertex);
    shaderStages[1] = vulkan::pipeline::tools::loadShader(device, RESOURCE_PATH"shaders/base.frag.spv", vk::ShaderStageFlagBits::eFragment);
    vk::PipelineRenderingCreateInfo renderingInfo;
    if(g_VulkanDevice.IsEnableDynamicRendering()){
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachmentFormats = &g_VulkanRenderer.GetSurfaceFormat();
        renderingInfo.depthAttachmentFormat = g_VulkanRenderer.GetDepthImage().GetFormat();
        info.pNext = &renderingInfo;    
    }
    info.pStages = shaderStages.data();
    info.stageCount = shaderStages.size();

    info.pVertexInputState = &inputState;
    info.pDynamicState = &dynamicState;
    info.pViewportState = &viewportState;
    info.pColorBlendState = &colorBlendState;
    info.pMultisampleState = &multisampleState;
    info.pDepthStencilState = &depthStencilState;
    info.pInputAssemblyState = &inputAssemblyState;
    info.pRasterizationState = &rasterizationState;
    auto result = device.createGraphicsPipeline(g_Pipeline.cache, info);
    if(result.result == vk::Result::eSuccess){        
        g_Pipeline.fill = result.value;
    }
    inputAssemblyState.topology = vk::PrimitiveTopology::eLineList;
    result = device.createGraphicsPipeline(g_Pipeline.cache, info);
    if(result.result == vk::Result::eSuccess){
        g_Pipeline.wireframe = result.value;
    }
    device.destroyShaderModule(shaderStages[0].module);
    device.destroyShaderModule(shaderStages[1].module);
}
void CreateMVPUBO(const vulkan::Device&device){
    auto physicalDeviceProperties = device.GetPhysicalDeviceProperties();
    const int32_t minUniformBufferOffset = ALIGN(sizeof(MVP_UBO), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    g_MvpUbo.Create(device, minUniformBufferOffset, 4, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu, true);
    UpdateUniform(device);
    std::vector bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex)
    };
    vulkan::framework::UpdateDescriptorSets(device, bindings, {g_MvpUbo}, {}, g_Set[1]);
}
#ifdef ENABE_DEPTH_TEST
void CreateDepthTestResource(const vulkan::Device&device){
    vk::Extent3D size;
    size.width = size.height = size.depth = 256;
    g_DepthTest.depth.Create(device, size, vk::ImageUsageFlagBits::eStorage|vk::ImageUsageFlagBits::eTransferDst, vk::Format::eR32Sfloat);
    g_DepthTest.depth.CreateView(device, vk::ImageViewType::e3D);

    std::vector bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eFragment)
    };
    g_DepthTest.set = g_VulkanPool.AllocateDescriptorSets(device, {g_DepthTest.setLayout})[0];
    vulkan::framework::UpdateDescriptorSets(device, bindings, {}, {g_DepthTest.depth}, g_DepthTest.set);
}
void DestroyDepthTestResource(const vulkan::Device&device){
    g_DepthTest.depth.Destroy(device);
    device.GetDevice().destroyDescriptorSetLayout(g_DepthTest.setLayout);
}
#endif
void DestoryMVPUBO(const vulkan::Device&device){
    g_MvpUbo.Destroy(device);
}
void SetupDescriptorSetLayout(vk::Device device){
    std::array bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
    };
    g_CameraSetLayout[0] = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings));
    std::array mvp_bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex)
    };
    g_CameraSetLayout[1] = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, mvp_bindings));
#ifdef ENABE_DEPTH_TEST
    std::array depth_test_bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eFragment)
    };
    g_DepthTest.setLayout = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, depth_test_bindings));
#endif
}
void Setup(GLFWwindow *window){
    glfwSetKeyCallback(window, keybutton);
    glfwSetScrollCallback(window, mousescroll);
    glfwSetCursorPosCallback(window, mousecursor);
    glfwSetMouseButtonCallback(window, mousebutton);

    SetupDescriptorSetLayout(g_VulkanDevice);
    
    g_Pipeline.cache = vulkan::pipeline::tools::createPipelineCache(g_VulkanDevice, "GraphicsPipelineCache");

    g_Camera.CreateUniform(g_VulkanDevice);
    g_Camera.UpdateUniform(g_VulkanDevice);

    g_Set = g_VulkanPool.AllocateDescriptorSets(g_VulkanDevice, g_CameraSetLayout);

    CreateMVPUBO(g_VulkanDevice);
#ifdef ENABE_DEPTH_TEST
    CreateDepthTestResource(g_VulkanDevice);
#endif
    std::vector bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
    };
    vulkan::framework::UpdateDescriptorSets(g_VulkanDevice, bindings, { g_Camera.GetUniform() }, {}, g_Set[0]);

    g_Plane[0].plane = mglm::planes::xy;
    g_Plane[1].plane = mglm::planes::zw;

    g_ImGuiInput.parameter.hopf.torusAspect = CLIFFORD_R;
    g_ImGuiInput.parameter.function.fun = GetFunction("tan");

    g_Geometry = new Tesseract;
    g_Geometry->Setup(g_VulkanDevice, g_VulkanQueue.graphics, g_VulkanPool);
    g_Geometry->Update();

    CreatePipelineLayout(g_VulkanDevice);
    CreateGraphicsPipeline(g_VulkanDevice, g_Pipeline.layout);

    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&io = ImGui::GetIO();
    io.ConfigNavCursorVisibleAlways = true;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    g_VulkanImGui.Setup(g_VulkanDevice, g_VulkanPool);
    g_VulkanImGui.CreateFont(RESOURCE_PATH"fonts/SourceHanSerifCN-Bold.otf", g_VulkanQueue.graphics, g_VulkanPool);
    g_VulkanImGui.CreatePipeline(g_VulkanRenderer.GetSurfaceFormat(), g_VulkanRenderer.GetDepthImage().GetFormat(), g_VulkanRenderer.GetRenderPass(), g_Pipeline.cache);
}

void Cleanup(const vulkan::Device&device){
    vk::Device dev = device;    
    g_VulkanImGui.Cleanup();
    DestoryMVPUBO(device);
    ImGui_ImplGlfw_Shutdown();
#ifdef ENABE_DEPTH_TEST
    DestroyDepthTestResource(device);
#endif
    for (size_t i = 0; i < g_CameraSetLayout.size(); i++){
        dev.destroyDescriptorSetLayout(g_CameraSetLayout[i]);
    }
    vulkan::pipeline::tools::destroyPipelineCache(device, "GraphicsPipelineCache", g_Pipeline.cache);
    g_Geometry->Cleanup();
    DestroyGraphicsPipeline(device);
    g_Camera.DestroyUniform(device);
    // g_PickingTexture.Cleanup(device);
}
void RecreateSwapchain(void *userData){
    GLFWwindow* window = (GLFWwindow*)userData;
    int32_t width, height;
    do{
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }while(width <= 0 || height <= 0);
    g_VulkanDevice.waitIdle();
    g_WindowWidth = width;
    g_WindowHeight = height;
    //想不调用vkDeviceWaitIdle重建交换链也行, 在重新创建交换链时, VkSwapchainCreateInfoKHR的结构体中oldSwapChain设置为原来的交换链
    g_VulkanRenderer.CleanupSwapchain();
    g_VulkanRenderer.CreateSwapchain(g_WindowWidth, g_WindowHeight);
    g_VulkanRenderer.CreateDepthImage();
    if(!g_VulkanDevice.IsEnableDynamicRendering()){
        g_VulkanRenderer.CreateFrameBuffer();
    }
}
void display(GLFWwindow* window){
    g_VulkanDevice.waitIdle();
    if(g_ImGuiInput.geometry.remake){
        g_ImGuiInput.geometry.remake = false;
        g_Geometry->Cleanup();
        delete g_Geometry;
        if(g_ImGuiInput.geometry.tesseract){
            g_Geometry = new Tesseract;
        }
        else if(g_ImGuiInput.geometry.cube){
            g_Geometry = new Cube;
        }
        else if(g_ImGuiInput.geometry.sphere){
            g_Geometry = new Sphere;
        }
        else if(g_ImGuiInput.geometry.pentatope){
            g_Geometry = new Pentatope;
        }
        else if(g_ImGuiInput.geometry.hexadeca){
            g_Geometry = new Hexadeca;
        }
        else if(g_ImGuiInput.geometry.icositetra){
            g_Geometry = new Icositetra;
        }
        else if(g_ImGuiInput.geometry.hopf){
            g_Geometry = new Hopf;
        }
        else if(g_ImGuiInput.geometry.font){
            g_Geometry = new Font;
        }
        else if(g_ImGuiInput.geometry.cylinder){
            g_Geometry = new Cylinder;
        }
        else if(g_ImGuiInput.geometry.pipeline){
            g_Geometry = new Pipeline;
        }
        else if(g_ImGuiInput.geometry.sphereCone){
            g_Geometry = new SphereCone;
        }
        else if(g_ImGuiInput.geometry.kleinBottle){
            g_Geometry = new KleinBottle;
        }
        else if(g_ImGuiInput.geometry.grid3d){
            g_Geometry = new Grid3D;
        }
        else if(g_ImGuiInput.geometry.grid4d){
            g_Geometry = new Grid4D;
        }
        else if(g_ImGuiInput.geometry.spherinder){   
            g_Geometry = new Spherinder;
        }
        else if(g_ImGuiInput.geometry.hypersphere){
            g_Geometry = new Hypersphere;
        }
        else if(g_ImGuiInput.geometry.realProjectivePlane){
            g_Geometry = new ProjectivePlane;
        }
        else if(g_ImGuiInput.geometry.function){
            g_Geometry = new Function;
        }
#ifndef NDEBUG
        else if(g_ImGuiInput.geometry.test){
            g_Geometry = new GeometryTest;
        }
#endif
        g_Geometry->Setup(g_VulkanDevice, g_VulkanQueue.graphics, g_VulkanPool);
    }
    if(g_ImGuiInput.geometry.update){
        g_ImGuiInput.geometry.update = false;
        g_Geometry->Update(&g_ImGuiInput.parameter);
        UpdateUniform(g_VulkanDevice);
    }
    if(g_VulkanDevice.IsEnableDynamicRendering()){
        g_VulkanRenderer.DynamicRendering(g_VulkanDevice, g_VulkanQueue, RecordCommand, RecreateSwapchain, window);
    }
    else{
        g_VulkanRenderer.Render(g_VulkanDevice, g_VulkanQueue, RecordCommand, RecreateSwapchain, window);
    }
}
bool SelectPhysicalDevice(vk::PhysicalDevice physicalDevice){
    vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
    return physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu;
}
void SetupVulkan(GLFWwindow *window){
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
#ifndef NDEBUG
    g_VulkanDevice.EnableValidation();
#endif
    g_VulkanDevice.CreateInstance(extensions);
    if(!g_VulkanDevice.SelectPhysicalDevice(SelectPhysicalDevice)){
        spdlog::error("No suitable graphics card!");
        glfwSetWindowShouldClose(window, true);
        return;
    }
    g_VulkanDevice.EnableDynamicRendering();
#ifdef ENABLE_DEPTH_TEST
    g_VulkanDevice.EnableFragmentStoresAndAtomics();
#endif
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(g_VulkanDevice.GetInstance(), window, nullptr, &surface);
    g_VulkanRenderer.SetSurface(surface);
    g_VulkanDevice.CreateDevice(g_VulkanRenderer.GetSurface());
    g_VulkanDevice.CreateAllocator();
    g_VulkanPool.CreatePool(g_VulkanDevice, 5, vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    g_VulkanRenderer.Setup(g_VulkanDevice, g_WindowWidth, g_WindowHeight, g_VulkanPool);
    g_VulkanQueue.CreateQueue(g_VulkanDevice, g_VulkanRenderer.GetSurface());
    const char *deviceType;
    vk::PhysicalDeviceProperties physicalDeviceProperties = g_VulkanDevice.GetPhysicalDeviceProperties();
    switch (physicalDeviceProperties.deviceType){
    case vk::PhysicalDeviceType::eCpu:
        deviceType = "CPU";
        break;
    case vk::PhysicalDeviceType::eDiscreteGpu:
        deviceType = "DISCRETE GPU";
        break;
    case vk::PhysicalDeviceType::eIntegratedGpu:
        deviceType = "INTEGRATED GPU";
        break;
    case vk::PhysicalDeviceType::eVirtualGpu:
        deviceType = "VIRTUAL GPU";
        break;
    default:
        deviceType = "OTHER";
        break;
    }
	printf("gpu name:%s, gpu type:%s\n", physicalDeviceProperties.deviceName, deviceType);
    spdlog::info("API version: {}.{}.{} (raw 0x{:08X})", VK_API_VERSION_MAJOR(physicalDeviceProperties.apiVersion), VK_API_VERSION_MINOR(physicalDeviceProperties.apiVersion), VK_API_VERSION_PATCH(physicalDeviceProperties.apiVersion),physicalDeviceProperties.apiVersion);
    spdlog::info("device type:{}, device name:{}", deviceType, physicalDeviceProperties.deviceName.data());
}
void CleanupVulkan(){
    g_VulkanPool.Cleanup(g_VulkanDevice);

    g_VulkanRenderer.Cleanup();

    g_VulkanDevice.Cleanup();
}
glm::uvec2 GetScreenSize(){
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    return glm::uvec2(mode->width, mode->height);
}
double calculateFPS(double updateInterval = 1.0) {
    static int frameCount = 0;
    static double accumulatedTime = 0.0;
    static double lastFPS = 0.0;
    static auto lastTime = std::chrono::high_resolution_clock::now();

    auto now = std::chrono::high_resolution_clock::now();
    double delta = std::chrono::duration<double>(now - lastTime).count();
    lastTime = now;

    frameCount++;
    accumulatedTime += delta;

    if (accumulatedTime >= updateInterval) {
        lastFPS = frameCount / accumulatedTime;
        frameCount = 0;
        accumulatedTime = 0.0;
    }

    return lastFPS;
}
#ifdef _WIN32
LONG WINAPI MyExceptionFilter(EXCEPTION_POINTERS *ep) {
    HANDLE hFile = CreateFileA("crash.dmp", GENERIC_WRITE, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId          = GetCurrentThreadId();
        mdei.ExceptionPointers = ep;
        mdei.ClientPointers    = FALSE;
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                          hFile, MiniDumpWithFullMemory, &mdei, NULL, NULL);
        CloseHandle(hFile);
    }
    return EXCEPTION_EXECUTE_HANDLER; // 让程序终止
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
#ifdef WIN32
    SetUnhandledExceptionFilter(MyExceptionFilter);
#endif
    g_Logger->flush_on(spdlog::level::err);
#ifdef NDEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::debug);
#endif // NDEBUG
    spdlog::set_default_logger(g_Logger);
    if (GLFW_FALSE == glfwInit()) {
        const char* desc = nullptr;
        int err = glfwGetError(&desc);
        printf("glfwInit failed, code=%d, desc = %s\n", err, desc ? desc : "(null)");
        spdlog::error("glfwInit failed, code={:#x}, desc ={}", err, desc ? desc : "(null)");
        return -1;
    }
    auto screenSize = GetScreenSize();
    g_WindowWidth = std::min(screenSize.x, screenSize.y) * .8;
    g_WindowHeight = g_WindowWidth;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "demo", NULL, NULL);
    // Setup Vulkan
    if (!glfwVulkanSupported()){
        const char* desc = nullptr;
        int err = glfwGetError(&desc);
        printf("GLFW: Vulkan Not Supported, Error Code=%d, desc = %s\n", err, desc ? desc : "(null)");
        spdlog::error("GLFW: Vulkan Not Supported, Error Code={:#x}, desc = {}", err, desc ? desc : "(null)");
        return -1;
    }
    SetupVulkan(window);
    Setup(window);

    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime < g_ImGuiInput.targetFrameTime) {
            double sleepTime = (g_ImGuiInput.targetFrameTime - elapsedTime) * 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTime));
        }

        previousTime = std::chrono::high_resolution_clock::now();
        glfwPollEvents();

        // proccessInput(window);

        display(window);

        g_ImGuiInput.fps = calculateFPS();
    }
	glfwTerminate();

    g_VulkanDevice.waitIdle();
    Cleanup(g_VulkanDevice);
    CleanupVulkan();
    spdlog::shutdown();
    return 0;
}
