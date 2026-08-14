#include "function.h"
#include <glm/glm.hpp>
#include <complex>
glm::vec4 complexToColor(const std::complex<float>& val) {
    float mag = std::abs(val);
    float arg = std::arg(val);  // [-π, π]

    // Hue: 映射到 [0,1]
    float H = (arg + M_PI) / (2.0f * M_PI);

    // Value: 用对数模长，并钳制到 [0,1]
    float log_mag = std::log(mag + 1e-10f);  // 避免 log(0)
    float V = 0.5f + 0.5f * std::tanh(log_mag * 0.2f);  // 将 (-∞,+∞) 映射到 (0,1)

    // Saturation: 固定为 1，也可随 mag 微弱变化
    float S = 1.0f;

    // HSV → RGB 转换（标准算法）
    float C = V * S;
    float X = C * (1.0f - std::fabs(std::fmod(H * 6.0f, 2.0f) - 1.0f));
    float m = V - C;
    float r, g, b;
    if (H < 1.0f/6.0f)       { r=C; g=X; b=0; }
    else if (H < 2.0f/6.0f)  { r=X; g=C; b=0; }
    else if (H < 3.0f/6.0f)  { r=0; g=C; b=X; }
    else if (H < 4.0f/6.0f)  { r=0; g=X; b=C; }
    else if (H < 5.0f/6.0f)  { r=X; g=0; b=C; }
    else                      { r=C; g=0; b=X; }
    return glm::vec4(r+m, g+m, b+m, 1.0f);
}
void generatePolarFunctionVertices(
    const std::function<std::complex<float>(const std::complex<float>&)>& fun,
    std::vector<Vertex>& vertices,
    std::vector<uint16_t>& indices,
    float r_min = 0.002f,
    float r_max = 1.5f,
    int radial_steps = 200,
    int angular_base = 120)
{
    // 预计算所有半径（对数分布）
    std::vector<float> radii(radial_steps + 1);
    float log_r_min = std::log(r_min);
    float log_r_max = std::log(r_max);
    float d_log_r = (log_r_max - log_r_min) / radial_steps;
    for (int i = 0; i <= radial_steps; ++i) {
        radii[i] = std::exp(log_r_min + i * d_log_r);
    }

    // 第一遍：计算每个半径层的角度分段数，并预留顶点
    struct RingInfo {
        int start_index;      // 本层第一个顶点在 vertices 中的索引
        int num_angular;      // 本层角度分段数
    };
    std::vector<RingInfo> rings(radial_steps + 1);
    int total_verts = 0;
    for (int ri = 0; ri <= radial_steps; ++ri) {
        float r = radii[ri];
        // 角度分段数与半径成正比，保证最外层达到 angular_base，内层减少但不低于最小值
        int n_ang = std::max(6, (int)(angular_base * (r / r_max)));
        rings[ri].num_angular = n_ang;
        rings[ri].start_index = total_verts;
        total_verts += n_ang;
    }

    vertices.clear();
    vertices.reserve(total_verts);
    indices.clear();
    // 索引数量：每两个相邻层之间形成三角形扇，共 (radial_steps) 个带状区域
    // 每个带状区域：外环 n_out 个顶点，内环 n_in 个顶点，构成 2*(n_out + n_in - 2) 个三角形
    // 近似估算，实际稍后精确计算
    indices.reserve(radial_steps * (angular_base * 2 + 10) * 3);

    // 第二遍：生成所有顶点
    for (int ri = 0; ri <= radial_steps; ++ri) {
        float r = radii[ri];
        int n_ang = rings[ri].num_angular;
        float d_theta = 2.0f * M_PI / n_ang;
        for (int ai = 0; ai < n_ang; ++ai) {
            float theta = ai * d_theta;
            std::complex<float> z(r * std::cos(theta), r * std::sin(theta));
            std::complex<float> val = fun(z);
            Vertex v;
            v.pos = glm::vec4(z.real(), z.imag(), val.real(), val.imag());
            v.color = complexToColor(val);
            vertices.push_back(v);
        }
    }

    // 第三遍：生成索引（连接相邻两层）
    for (int ri = 0; ri < radial_steps; ++ri) {
        int inner_start = rings[ri].start_index;
        int outer_start = rings[ri + 1].start_index;
        int n_inner = rings[ri].num_angular;
        int n_outer = rings[ri + 1].num_angular;

        // 双指针贪心法连接内外环（最短对角线）
        int i = 0;  // 内环指针
        int j = 0;  // 外环指针
        while (i < n_inner && j < n_outer) {
            int i_next = (i + 1) % n_inner;
            int j_next = (j + 1) % n_outer;

            uint16_t i0 = inner_start + i;
            uint16_t i1 = inner_start + i_next;
            uint16_t j0 = outer_start + j;
            uint16_t j1 = outer_start + j_next;

            // 计算两条对角线的平方长度（利用顶点位置）
            auto getPos = [&](uint16_t idx) -> glm::vec2 {
                return glm::vec2(vertices[idx].pos.x, vertices[idx].pos.y);
            };
            glm::vec2 p_i0 = getPos(i0);
            glm::vec2 p_i1 = getPos(i1);
            glm::vec2 p_j0 = getPos(j0);
            glm::vec2 p_j1 = getPos(j1);

            float diag1 = glm::dot(p_i1, p_j0);  // 对角线 (i1, j0)
            float diag2 = glm::dot(p_i0, p_j1);  // 对角线 (i0, j1)

            if (diag1 < diag2) {
                // 三角形 (i0, j0, j1) 和 (i0, j1, i1)
                indices.push_back(i0);
                indices.push_back(j0);
                indices.push_back(j1);
                indices.push_back(i0);
                indices.push_back(j1);
                indices.push_back(i1);
                j++;  // 外环指针前进
            } else {
                // 三角形 (i0, j0, i1) 和 (i1, j0, j1)
                indices.push_back(i0);
                indices.push_back(j0);
                indices.push_back(i1);
                indices.push_back(i1);
                indices.push_back(j0);
                indices.push_back(j1);
                i++;  // 内环指针前进
            }
        }

        // 处理剩余顶点（当一个环先遍历完时）
        while (i < n_inner) {
            int i_next = (i + 1) % n_inner;
            uint16_t i0 = inner_start + i;
            uint16_t i1 = inner_start + i_next;
            uint16_t j_last = outer_start + (n_outer - 1);
            // 退化三角形（面积为零），实际渲染时会被剔除或不可见
            indices.push_back(i0);
            indices.push_back(j_last);
            indices.push_back(i1);
            i++;
        }
        while (j < n_outer) {
            int j_next = (j + 1) % n_outer;
            uint16_t j0 = outer_start + j;
            uint16_t j1 = outer_start + j_next;
            uint16_t i_last = inner_start + (n_inner - 1);
            indices.push_back(i_last);
            indices.push_back(j0);
            indices.push_back(j1);
            j++;
        }
    }
}
void generateFunctionVertices(const std::function<std::complex<float>(const std::complex<float> &)>&fun, std::vector<Vertex>&vertices, std::vector<uint16_t>&indices, float range = 1.0f){
    const float step = STEP(range);
    const float range_min = range * -1.0f;
    const uint32_t COUNT = static_cast<uint32_t>((range - range_min) / step) + 1;
    vertices.reserve(COUNT * COUNT);
    indices.reserve((COUNT - 1) * (COUNT * 2 + 1));                   
    for (uint32_t xi = 0; xi < COUNT; ++xi) {
        float r = range_min + xi * step;                                       
        for (uint32_t yi = 0; yi < COUNT; ++yi) {
            float i_coord = range_min + yi * step;
            
            std::complex<float> z(r, i_coord), val = fun(z);
            Vertex v;
            v.pos = glm::vec4(z.real(), z.imag(), val.real(), val.imag());
            // color encodes argument (hue) and magnitude (value)
            v.color = complexToColor(val);
            vertices.push_back(v);
        }
    }
    for (uint32_t xi = 0; xi < COUNT - 1; ++xi) {
        for (uint32_t yi = 0; yi < COUNT - 1; ++yi) {
            uint32_t tl = xi * COUNT + yi;
            uint32_t tr = xi * COUNT + yi + 1;
            uint32_t bl = (xi + 1) * COUNT + yi;
            uint32_t br = (xi + 1) * COUNT + yi + 1;

            indices.push_back(tl);
            indices.push_back(tr);
            indices.push_back(bl);
 
            indices.push_back(tr);
            indices.push_back(br);
            indices.push_back(bl);
        }
    }
}
std::string Function::generateFunctionVertices(const std::string&funStr, std::vector<Vertex>&vertices, std::vector<uint16_t>&indices, float range){
    mExpression.release();
    if(!mParser.compile(funStr, mExpression)){
        return mParser.error();
    }
    const float step = STEP(range);
    const float range_min = range * -1.0f;
    const uint32_t COUNT = static_cast<uint32_t>((range - range_min) / step) + 1;
    vertices.reserve(COUNT * COUNT);
    indices.reserve((COUNT - 1) * (COUNT * 2 + 1)); // strip + restart
    for (uint32_t xi = 0; xi < COUNT; ++xi) {
        float r = range_min + xi * step;//如果xi是float则不需要这么写
        for (uint32_t yi = 0; yi < COUNT; ++yi) {
            float i_coord = range_min + yi * step;
            z = cmplx::complex_t(r, i_coord);
            cmplx::complex_t val = mExpression.value();
            
            Vertex v;

            v.pos = glm::vec4(z.c_.real(), z.c_.imag(), val.c_.real(), val.c_.imag());
            v.color = v.pos * .5f + .5f;
            vertices.push_back(v);
        }
    }
    for (uint32_t xi = 0; xi < COUNT - 1; ++xi) {
        for (uint32_t yi = 0; yi < COUNT - 1; ++yi) {
            uint32_t tl = xi * COUNT + yi;          // top-left
            uint32_t tr = xi * COUNT + yi + 1;      // top-right
            uint32_t bl = (xi + 1) * COUNT + yi;    // bottom-left
            uint32_t br = (xi + 1) * COUNT + yi + 1;// bottom-right

            // 两个三角形，保持一致的缠绕顺序（这里假设逆时针）
            // 三角形1: tl -> tr -> bl
            indices.push_back(tl);
            indices.push_back(tr);
            indices.push_back(bl);
            // 三角形2: tr -> br -> bl
            indices.push_back(tr);
            indices.push_back(br);
            indices.push_back(bl);
        }
    }
    return "";
}
Function::Function(/* args */){
    mSymbol_table.add_constant("pi",  cmplx::complex_t(M_PI, 0));
    mSymbol_table.add_constant("e",   cmplx::complex_t(M_E, 0));
    mSymbol_table.add_constant("j", cmplx::complex_t(0, 1));
    mSymbol_table.add_constant("i", cmplx::complex_t(0,1));
    mSymbol_table.add_constant("epsilon",  cmplx::complex_t(DBL_EPSILON, 0));
    mSymbol_table.add_constant("inf",      cmplx::complex_t(INFINITY, 0));
    mSymbol_table.add_variable("z", z);
    // mSymbol_table.add_variable("w", w);
    if(!mExpression.register_symbol_table(mSymbol_table)){
        spdlog::error("符号表注册失败");
    }
}

Function::~Function(){
}
void Function::Cleanup(){
    mGeometry.Destroy(*gpu.device);
}

void Function::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Function::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Function::Update(const void *useData){
    std::vector<Vertex>vertices;
    std::vector<uint16_t>indices;
    UseData* parameter = const_cast<UseData*>(static_cast<const UseData*>(useData));
    // generatePolarFunctionVertices(parameter->function.fun, vertices, indices);
    if(parameter->function.function != ""){
        parameter->function.error = generateFunctionVertices(parameter->function.function, vertices, indices, parameter->function.range);
        if(parameter->function.error != "")return;
    }
    else{
        ::generateFunctionVertices(parameter->function.fun, vertices, indices, parameter->function.range);
    }
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}