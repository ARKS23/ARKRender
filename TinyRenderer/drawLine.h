#include "tgaimage.h"
#include <iostream>
#include <sstream>

void drawLine(TGAImage& framebuffer, std::vector<int> stratPoint, std::vector<int> endPoint, float steps = 0.01, TGAColor color = white) {
    // 参数方程进行线的绘制，控制步长为stpes （重心坐标线形插值）
    float xa = stratPoint[0], ya = stratPoint[1];
    float xb = endPoint[0], yb = endPoint[1];
    for (float t = 0.0; t < 1; t += steps) {
        float xt = (1 - t) * xa + t * xb;
        float yt = (1 - t) * ya + t * yb;

        framebuffer.set(xt, yt, color);
    }
}

void drawTriangle(TGAImage& framebuffer, std::vector<int> point1, std::vector<int> point2, std::vector<int> point3) {
    // 绘制三角形
    drawLine(framebuffer, point1, point2, 0.01, green);
    drawLine(framebuffer, point1, point3, 0.01, blue);
    drawLine(framebuffer, point2, point3, 0.01, red);
}

void BresenhamLine(TGAImage& framebuffer, std::vector<int> stratPoint, std::vector<int> endPoint, TGAColor color = white) {
    // 通过Bresenham算法绘制线段
    int ax = stratPoint[0], ay = stratPoint[1];
    int bx = endPoint[0], by = endPoint[1];
    // 判断“陡峭”情况, 如果陡峭则进行x, y的交换
    bool isSteep = std::abs(bx - ax) < std::abs(by - ay);
    if (isSteep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    // “向后”的线段处理：交换起点和终点
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    // y的前进方向
    int sy = (by > ay) ? 1 : -1;

    int y = ay;
    int ierror = 0; // 决策变量改用整形运算
    for (int x = ax; x <= bx; ++x) {
        if (isSteep) { // 如果是“陡峭”的，之前交换过x，y，记得转置回来
            framebuffer.set(y, x, color);
        }
        else {
            framebuffer.set(x, y, color);
        }

        ierror += 2 * std::abs(by - ay); // 与浮点数运算error斜率累计0.5是同理，整数运算更快
        if (ierror > bx - ax) {
            y += sy;
            ierror -= 2 * std::abs(bx - ax);
        }
    }
}

void drawObjLine(TGAImage& framebuffer, std::string objPath, TGAColor color = white) {
    std::fstream file(objPath);

    if (!file.is_open()) {
        std::cerr << "错误：无法打开文件 " << objPath << std::endl;
    }

    std::string line;
    // 逐行读取
    while (std::getline(file, line)) {

    }
}

void drawLineTest() {
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    // 画线测试
    //drawLine(framebuffer, {0, 0}, {224, 224}, 0.001);

    // 绘制三角形测试
    // std::vector<int> point1({7, 3}), point2({12, 37}), point3({62, 53});
    // drawTriangle(framebuffer, point1, point2, point3);

    // // 随机绘制测试
    // std::srand(std::time({}));
    // std::vector<TGAColor> colorVec({white, green, red, yellow, blue}); 
    // for (int i=0; i<(1<<24); i++) {
    //     int ax = rand()%width, ay = rand()%height;
    //     int bx = rand()%width, by = rand()%height;
    //     drawLine(framebuffer, {ax, ay}, {bx, by}, 0.01, colorVec[rand() % colorVec.size()]);
    // }

    // 画线算法测试
    BresenhamLine(framebuffer, { 0, 0 }, { 63, 62 }, green);


    framebuffer.write_tga_file("framebuffer.tga");
}