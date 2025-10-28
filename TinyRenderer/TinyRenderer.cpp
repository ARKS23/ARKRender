// TinyRenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "tgaimage.h"
#include "drawLine.h"
#include "model.h"
#include <iostream>

const std::string FILEPATH = "E:/code/TinyRenderer/obj/diablo3_pose.obj";

void analyzeModel() {
    // 解析obj文件
    Model monsterModel(FILEPATH);

    const int height = 800;
    const int width = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int faceNum = monsterModel.nfaces();
    for (int i = 0; i < faceNum; ++i) {   // 遍历模型中的所有面
        std::vector<vec4> vertexArr;
        for (int j = 0; j < 3; ++j) {  // 获取面的三个顶点
            vertexArr.push_back(monsterModel.vert(i, j));
        }
        // 进行正交投影
        std::vector<std::vector<int>> points;
        for (vec4 vertex : vertexArr) {
            std::vector<int> point(2);
            point[0] = static_cast<int>((vertex.x + 1.0) * width / 2.0);
            point[1] = static_cast<int>((vertex.y + 1.0) * height / 2.0);
            points.push_back(point);
        }
        // 绘制线条
        BresenhamLine(framebuffer, points[0], points[1], red);
        BresenhamLine(framebuffer, points[0], points[2], red);
        BresenhamLine(framebuffer, points[1], points[2], red);
    }

    framebuffer.write_tga_file("E:/code/TinyRenderer/images/monster.tga");
}

int main()
{
    analyzeModel();
    return 0;
}

