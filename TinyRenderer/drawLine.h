#include "tgaimage.h"
#include <iostream>
#include <sstream>


class drawLineUtils {
private:
    drawLineUtils() {} // �����࣬������ʵ����

public:
    // ���Բ�ֵ����
    static void lerpDrawLine(TGAImage& framebuffer, std::vector<int> stratPoint, std::vector<int> endPoint, float steps, TGAColor color);

    // Bresenham�㷨����
    static void BresenhamLine(TGAImage& framebuffer, const std::vector<int> &stratPoint, const std::vector<int> &endPoint, const TGAColor &color);

    // ����������
    static void drawTriangle(TGAImage& framebuffer, const std::vector<int> &point1, const std::vector<int> &point2, const std::vector<int> &point3, const std::vector<TGAColor> &colorVec);

    // ���Ժ���
    static void drawLineTest();
};