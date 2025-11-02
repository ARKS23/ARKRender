// TinyRenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "tgaimage.h"
#include "drawLine.h"
#include "model.h"
#include <iostream>
#include "Renderer.h"
#include "PhongShader.h"
#include <cmath>        
#include <windows.h>    

#define M_PI 3.14159265358979323846

const int width = 800;
const int height = 800;
const std::string FILEPATH = "E:/code/TinyRenderer/obj/african_head.obj";
const std::string OUTPATH = "E:/code/TinyRenderer/images/african_head.tga";


mat<4, 4> lookat(vec3 eye, vec3 center, vec3 up) {
    vec3 z = normalized(center - eye); // 左手坐标系前向量Z
    vec3 x = normalized(cross(up, z)); // 计算右向量X
    vec3 y = normalized(cross(z, x));  // 计算上向量Y

    // 旋转整个世界，让x,y,z分别对齐到世界坐标系的(1, 0, 0),(0, 1, 0),(0,0,1)
    mat<4, 4> Minv = { {
        {x.x, x.y, x.z, 0},
        {y.x, y.y, y.z, 0},
        {z.x, z.y, z.z, 0},
        {0, 0, 0, 1}
    } };

    // 让世界移动-eye距离，eye点和世界原点(0,0,0)重合
    mat<4, 4> Tr = { {
        {1, 0, 0, -eye.x},
        {0, 1, 0, -eye.y},
        {0, 0, 1, -eye.z},
        {0, 0, 0, 1}
    } };
    return Minv * Tr; // 先平移再旋转
}

mat<4, 4> projection(double fov_y_degrees, double aspect_ratio, double n, double f) {
    /* 透视投影 */
    // 1. 计算 r 和 t
    double fov_y_radians = fov_y_degrees * M_PI / 180.0;
    double t = n * std::tan(fov_y_radians / 2.0);
    double r = t * aspect_ratio;
    double l = -r;
    double b = -t;

    // 2. 使用推导的 LHS 矩阵公式
    mat<4, 4> P = { {
        {2 * n / (r - l), 0,        (r + l) / (r - l),  0}, // 注意: (r+l)/(r-l)
        {0,         2 * n / (t - b), (t + b) / (t - b),  0}, // 注意: (t+b)/(t-b)
        {0,         0,         f / (f - n),      -n * f / (f - n)},
        {0,         0,         1,            0}
    } };
    return P;
}

mat<4, 4> identity() {
    return { {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    } };
}

int main()
{
    Renderer renderer(width, height);
    Model monsterModel(FILEPATH);
    PhongShader shader;
    shader.model_ptr = &monsterModel;

    // --- 1. 初始化 ---
    std::cout << "初始化渲染器" << std::endl;
    std::cout << "按下键盘左右按键旋转摄像机，\n按下ESC退出" << std::endl;

    // 设置镜头 (Projection 矩阵)
    renderer.set_projectionMatrix(projection(60.0, (double)width / height, 0.1, 100.0));

    // 设置视口 (Viewport 矩阵)
    renderer.set_viewportMatrix(0, 0, width, height);

    // 设置 Model 矩阵 (让模型保持在原点)
    shader.setModelMatrix(identity());

    vec3 eye = { 10, 0, 10 }; // 相机位置(按键改变)
    vec3 center = { 0, 0, 0 }; // 看向原点
    vec3 up = { 0, 1, 0 }; // Y轴上方向为“上”

    // 相机状态相关
    float cameraAngle = 0.f; //相机当前的旋转角度
    const float cameraRadius = 3.f;  // 相机距中心点距离
    const float rotationSpeed = 0.05f;  // 相机旋转幅度

    while (true)
    {
        bool isInput = false;
        // 检查 ESC 键是否被按下
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            cameraAngle -= rotationSpeed;
            isInput = true;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            cameraAngle += rotationSpeed;
            isInput = true;
        }

        // 使用三角函数计算相机在X和Z轴上的新位置，使其围绕(0,0,0)旋转
        vec3 eye = {
            (float)(sin(cameraAngle) * cameraRadius), // X
            -0.5,                                      // Y (保持一点高度)
            (float)(cos(cameraAngle) * cameraRadius)  // Z
        };
        renderer.set_viewMatrix(lookat(eye, center, up)); // 更新view矩阵

        // 设置shader
        shader.setModelMatrix(identity());
        shader.modelMatrix_invert_transpose = identity().invert_transpose();
        shader.camera_pos_world = eye;
        shader.light_dir_world = normalized(vec3{ 0, 0, 1 });  // 光源来自z+

        if (!isInput) continue;

        // 清理缓冲区
        renderer.clear_framebuffer(black);
        renderer.clear_zbuffer();

        // 绘制
        renderer.draw(&monsterModel, &shader);

        // 保存结果
        TGAImage& final_image = renderer.get_framebuffer();
        final_image.write_tga_file(OUTPATH);

        // 60fps  (1000ms / 16)
        //Sleep(16);
    }
    return 0;
}