// TinyRenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "tgaimage.h"
#include "drawLine.h"
#include "model.h"
#include <iostream>

const std::string FILEPATH = "E:/code/TinyRenderer/obj/diablo3_pose.obj";

int crossProduct2D(const std::vector<int> &point1, const std::vector<int> &point2) {
    /* 2D叉乘 */
    return point1[0] * point2[1] - point1[1] * point2[0];
}

std::vector<int> getVector(const std::vector<int> &p1, const std::vector<int> &p2) {
    /* 根据点求向量 */
    return {p2[0] - p1[0], p2[1] - p1[1]};
}

vec3 barycentric(const std::vector<std::vector<int>> &points, const std::vector<int> &curPoint) {
    /* 计算curPoint在三角形中的重心坐标 */
    vec3 coordinates;
    std::vector<int> A = points[0], B = points[1], C = points[2]; // 三角形的三个点

    /* 获取向量 */
    std::vector<int> AB = getVector(A, B), AC = getVector(A, C);
    std::vector<int> PA = getVector(curPoint, A), PB = getVector(curPoint, B), PC = getVector(curPoint, C);
    int S_ABC = crossProduct2D(AB, AC); //总面积

    // 必须保持 (A,B), (B,C), (C,A) 的循环
    int S_PCA = crossProduct2D(PC, PA);
    int S_APB = crossProduct2D(PA, PB);
    int S_BPC = crossProduct2D(PB, PC);

    // 计算对应的权重，这里注意不能弄乱命名，否则之后的深度z会计算错误
    double alpha = static_cast<double>(S_BPC) / S_ABC;
    double beta = static_cast<double>(S_PCA) / S_ABC;
    double gamma = static_cast<double>(S_APB) / S_ABC;

    // 重心坐标
    coordinates.x = alpha;
    coordinates.y = beta;
    coordinates.z = gamma;

    return coordinates;
}

void rasterization(TGAImage &framebuffer, TGAImage &zbuffer, const std::vector<std::vector<int>> &points, 
                   const std::vector<double> &points_z, TGAColor color=red) {
    int min_x = points[0][0], min_y = points[0][1];
    int max_x = min_x, max_y = min_y;
    for (std::vector<int> point: points) { // 包围盒优化
        min_x = std::min(min_x, point[0]); min_y = std::min(min_y, point[1]);
        max_x = std::max(max_x, point[0]); max_y = std::max(max_y, point[1]);
    }

    for (int c = 0; c < 3; c++) color[c] = std::rand() % 255; // 每块三角形颜色随机填入

    for (int x = min_x; x <= max_x; ++x) {  // 遍历盒内的坐标，进行光栅化
        for (int y = min_y; y <= max_y; ++y) {
            vec3 coordinates = barycentric(points, {x, y}); // 计算重心坐标
            if (coordinates.x < 0 || coordinates.y < 0 || coordinates.z < 0) continue;  // 检查是否在三角形内
            // Z-Buffer处理:目前精度不足待改进
            int za = static_cast<int>((points_z[0] + 1) * 255 / 2);
            int zb = static_cast<int>((points_z[1] + 1) * 255 / 2);
            int zc = static_cast<int>((points_z[2] + 1) * 255 / 2);
            unsigned char z = static_cast<unsigned char>(coordinates.x * za + coordinates.y * zb + coordinates.z * zc);
            if (z <= zbuffer.get(x, y)[0]) continue; // z-buffer处理
            zbuffer.set(x, y, { z }); // 灰度图着色
            framebuffer.set(x, y, color); // 在三角形内，进行颜色绘制（待改进）
        }
    }
}

void analyzeModel() {
    // 解析obj文件
    Model monsterModel(FILEPATH);

    const int height = 800;
    const int width = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE); // 灰度图,内有get(x,y)也可作为深度缓存（但精度不足）

    int faceNum = monsterModel.nfaces();
    for (int i = 0; i < faceNum; ++i) {   // 遍历模型中的所有面
        std::vector<vec4> vertexArr;
        for (int j = 0; j < 3; ++j) {  // 获取面的三个顶点
            vertexArr.push_back(monsterModel.vert(i, j));
        }
        // 进行正交投影: 后续进行封装
        std::vector<std::vector<int>> points;
        std::vector<double> points_z; // 进行深度缓存时要使用
        for (vec4 vertex : vertexArr) {
            std::vector<int> point(2);
            point[0] = static_cast<int>((vertex.x + 1.0) * width / 2.0);
            point[1] = static_cast<int>((vertex.y + 1.0) * height / 2.0);
            points.push_back(point);

            // 处理z
            points_z.push_back(vertex.z);
        }

        // 绘制三角形面
        //drawLineUtils::drawTriangle(framebuffer, points[0], points[1], points[2], { white, white, white });

        // 光栅化逻辑
        rasterization(framebuffer, zbuffer, points, points_z);
    }

    framebuffer.write_tga_file("E:/code/TinyRenderer/images/monster.tga");
    zbuffer.write_tga_file("E:/code/TinyRenderer/images/zbuffer.tga");
}

int main()
{
    analyzeModel();
    return 0;
}

