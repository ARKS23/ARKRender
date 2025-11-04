// TinyRenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "tgaimage.h"
#include "drawLine.h"
#include "model.h"
#include <iostream>
#include "Renderer.h"
#include "PhongShader.h"
#include "DepthShader.h"
#include <cmath>        
#include <windows.h>    

#define M_PI 3.14159265358979323846

const int width = 800;
const int height = 800;
const std::string FILEPATH = "E:/code/TinyRenderer/obj/african_head.obj";
const std::string OUTPATH = "E:/code/TinyRenderer/images/african_head.tga";
const std::string OUTPATH_SHADING = "E:/code/TinyRenderer/images/african_head_shading.tga";


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

mat<4, 4> orthographic(float left, float right, float bottom, float top, float near_, float far_) {
    /* 正交投影矩阵用于定向光的计算 */
    mat<4, 4> m = {
        {2.f / (right - left), 0, 0, -(left + right) / (right - left)},
        {0, 2.f / (top - bottom), 0, -(bottom + top) / (top - bottom)},
        // 修正: 使用 LHS 的 Z-Mapping (0 到 1)
        {0, 0, 1.f / (far_ - near_), -near_ / (far_ - near_)},
        {0, 0, 0, 1}
    };

    return m;
}

mat<4, 4> viewport(int x, int y, int w, int h) {
    mat<4, 4> m = {
                        {w / 2.0, 0, 0, x + w / 2.0},
                        {0, h / 2.0, 0, y + h / 2.0},
                        {0, 0, 1.0, 0},
                        {0, 0, 0, 1}
    };
    return m;
}

mat<4, 4> rotation_y(float angle_radians) {
    /* 绕y轴转的旋转矩阵 */
    float c = cos(angle_radians);
    float s = sin(angle_radians);
    return {
        {c, 0, s, 0},
        {0, 1, 0, 0},
        {-s, 0, c, 0},
        {0, 0, 0, 1}
    };
}

int main()
{
    Renderer renderer(width, height);
    Model monsterModel(FILEPATH);

    /* 着色器设置 */
    PhongShader phong_shader;
    DepthShader depth_shader;

    // --- 1. 初始化 ---
    std::cout << "初始化渲染器" << std::endl;
    std::cout << "按下键盘左右按键旋转摄像机，\n按下ESC退出" << std::endl;

    vec3 eye = { 3, 0, 3 }; // 相机位置(按键改变)
    vec3 center = { 0, 0, 0 }; // 看向原点
    vec3 up = { 0, 1, 0 }; // Y轴上方向为“上”

    // 相机状态相关
    float cameraAngle = 0.f; //相机当前的旋转角度
    const float cameraRadius = 3.f;  // 相机距中心点距离
    const float rotationSpeed = 0.05f;  // 相机旋转幅度

    // 模型状态相关
    float modelAngle = 0.f;

    // 光源设置
    vec3 light_dir = normalized(vec3({ 1, 0, 0 })); // 光的方向
    vec3 light_pos = light_dir * 3.f;               // 光源位置
    TGAImage shadow_map_texture(width, height, TGAImage::RGB); // 阴影贴图

    while (true)
    {
        bool isInput = false;
        // 检查 ESC 键是否被按下
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            //cameraAngle -= rotationSpeed;
            modelAngle -= rotationSpeed;
            isInput = true;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            //cameraAngle += rotationSpeed;
            modelAngle += rotationSpeed;
            isInput = true;
        }

        if (!isInput) continue;

        /*
        // 使用三角函数计算相机在X和Z轴上的新位置，使其围绕(0,0,0)旋转
        vec3 eye = {
            (float)(sin(cameraAngle) * cameraRadius), // X
            -0.5,                                      // Y (保持一点高度)
            (float)(cos(cameraAngle) * cameraRadius)  // Z
        };
        */

        mat<4, 4> modelMatrix = rotation_y(modelAngle); // 模型旋转矩阵

        /* 光源的变换矩阵 */
        mat<4, 4> lightView = lookat(light_pos, center, up);
        mat<4, 4> lightProjection = orthographic(-1.f, 1.f, -1.f, 1.f, 1.f, 5.f); // 光源视角用正交投影
        mat<4, 4> matrix_shadow_transform = lightProjection * lightView;

        /* 相机的变换矩阵 */
        mat<4, 4> CameraView = lookat(eye, center, up);
        mat<4, 4> CameraProjection = projection(60.0, (double)width / height, 0.1, 100.0);
        mat<4, 4> CameraViewport = viewport(0, 0, width, height);

        /* 渲染阴影贴图 */
        renderer.set_viewMatrix(lightView);
        renderer.set_projectionMatrix(lightProjection);
        renderer.set_viewportMatrix(0, 0, width, height);
        depth_shader.setModelMatrix(modelMatrix); // 模型旋转，传入模型旋转矩阵
        renderer.clear_framebuffer(black);
        renderer.clear_zbuffer();
        renderer.draw(&monsterModel, &depth_shader);
        shadow_map_texture = renderer.get_framebuffer();
        shadow_map_texture.write_tga_file(OUTPATH_SHADING); // 灰度图

        /* 渲染最终场景 */
        renderer.set_viewMatrix(CameraView);
        renderer.set_projectionMatrix(CameraProjection);
        renderer.set_viewportMatrix(0, 0, width, height);

        /* PhongShader设置 */
        phong_shader.setModelMatrix(modelMatrix); // 模型旋转，传入模型旋转矩阵
        phong_shader.modelMatrix_invert_transpose = modelMatrix.invert_transpose();
        phong_shader.camera_pos_world = eye;
        phong_shader.light_dir_world = light_dir;
        phong_shader.model_ptr = &monsterModel;
        phong_shader.shadow_map_texture = &shadow_map_texture; // 传入阴影贴图
        phong_shader.matrix_shadow_transform = matrix_shadow_transform; // 传入光源MVP变换矩阵

        renderer.clear_framebuffer(black);
        renderer.clear_zbuffer();
        renderer.draw(&monsterModel, &phong_shader);

        // 保存结果
        TGAImage& final_image = renderer.get_framebuffer();
        final_image.write_tga_file(OUTPATH);

        // 60fps  (1000ms / 16)
        //Sleep(16);
    }
    return 0;
}