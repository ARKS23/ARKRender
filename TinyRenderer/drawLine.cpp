#include "drawLine.h"

void drawLineUtils::lerpDrawLine(TGAImage& framebuffer, std::vector<int> stratPoint, std::vector<int> endPoint, float steps, TGAColor color)
{
    // �������̽����ߵĻ��ƣ����Ʋ���Ϊstpes �������������β�ֵ��
    float xa = stratPoint[0], ya = stratPoint[1];
    float xb = endPoint[0], yb = endPoint[1];
    for (float t = 0.0; t < 1; t += steps) {
        float xt = (1 - t) * xa + t * xb;
        float yt = (1 - t) * ya + t * yb;

        framebuffer.set(xt, yt, color);
    }
}

void drawLineUtils::BresenhamLine(TGAImage& framebuffer, const std::vector<int>& stratPoint, const std::vector<int>& endPoint, const TGAColor& color = white)
{
    // ͨ��Bresenham�㷨�����߶�
    int ax = stratPoint[0], ay = stratPoint[1];
    int bx = endPoint[0], by = endPoint[1];
    // �жϡ����͡����, ������������x, y�Ľ���
    bool isSteep = std::abs(bx - ax) < std::abs(by - ay);
    if (isSteep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    // ����󡱵��߶δ������������յ�
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    // y��ǰ������
    int sy = (by > ay) ? 1 : -1;

    int y = ay;
    int ierror = 0; // ���߱���������������
    for (int x = ax; x <= bx; ++x) {
        if (isSteep) { // ����ǡ����͡��ģ�֮ǰ������x��y���ǵ�ת�û���
            framebuffer.set(y, x, color);
        }
        else {
            framebuffer.set(x, y, color);
        }

        ierror += 2 * std::abs(by - ay); // �븡��������errorб���ۼ�0.5��ͬ�������������
        if (ierror > bx - ax) {
            y += sy;
            ierror -= 2 * std::abs(bx - ax);
        }
    }
}

void drawLineUtils::drawTriangle(TGAImage& framebuffer, const std::vector<int>& point1, const std::vector<int>& point2, const std::vector<int>& point3, const std::vector<TGAColor>& colorVec)
{
    // ���������Σ����Ľ�
    drawLineUtils::BresenhamLine(framebuffer, point1, point2, colorVec[0]);
    drawLineUtils::BresenhamLine(framebuffer, point1, point3, colorVec[1]);
    drawLineUtils::BresenhamLine(framebuffer, point2, point3, colorVec[2]);
}

void drawLineUtils::drawLineTest()
{
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    // ���Բ�ֵ���߲���
    // drawLineUtils::drawLine(framebuffer, {0, 0}, {224, 224}, 0.001);

    // ���������β���
    // std::vector<int> point1({7, 3}), point2({12, 37}), point3({62, 53});
    // drawLineUtils::drawTriangle(framebuffer, point1, point2, point3);

    // // ������Ʋ���
    // std::srand(std::time({}));
    // std::vector<TGAColor> colorVec({white, green, red, yellow, blue}); 
    // for (int i=0; i<(1<<24); i++) {
    //     int ax = rand()%width, ay = rand()%height;
    //     int bx = rand()%width, by = rand()%height;
    //     drawLineUtils::drawLine(framebuffer, {ax, ay}, {bx, by}, 0.01, colorVec[rand() % colorVec.size()]);
    // }

    // �����㷨����
    drawLineUtils::BresenhamLine(framebuffer, { 0, 0 }, { 63, 62 }, green);


    framebuffer.write_tga_file("framebuffer.tga");
}
