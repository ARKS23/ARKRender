#include "tgaimage.h"
#include <iostream>
#include <sstream>


class drawLineUtils {
private:
    drawLineUtils() {} // 工具类，不进行实例化

public:
    // 线性插值画线
    static void lerpDrawLine(TGAImage& framebuffer, std::vector<int> stratPoint, std::vector<int> endPoint, float steps, TGAColor color);

    // Bresenham算法画线
    static void BresenhamLine(TGAImage& framebuffer, const std::vector<int> &stratPoint, const std::vector<int> &endPoint, const TGAColor &color);

    // 绘制三角形
    static void drawTriangle(TGAImage& framebuffer, const std::vector<int> &point1, const std::vector<int> &point2, const std::vector<int> &point3, const std::vector<TGAColor> &colorVec);

    // 测试函数
    static void drawLineTest();
};