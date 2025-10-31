#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include "IShader.h"
#include <vector>

class Renderer
{
public:
	Renderer(int w, int h);

	// 清理缓冲区
	void clear_framebuffer(TGAColor color);
	void clear_zbuffer();

	// 设置变换矩阵
	void set_viewMatrix(mat<4, 4> matrix);
	void set_projectionMatrix(mat<4, 4> matrix);
	void set_viewportMatrix(int x, int y, int w, int h);

	// 核心绘制函数
	void draw(Model* model, IShader* shader);

	// 获取结果
	TGAImage& get_framebuffer();

private:
	TGAImage framebuffer;
	std::vector<float> zbuffer;

	//mat<4, 4> modelMatrix;      // 世界坐标变换矩阵(M)在shader中
	mat<4, 4> viewMatrix;       // 相机坐标变换矩阵(V)
	mat<4, 4> projectionMatrix; // 做投影变换的矩阵(P)
	mat<4, 4> viewPortMatrix;   // 做最后的变换到像素点的变换矩阵

	// 内部重心坐标计算辅助函数
	vec3 barycentric(vec3* pts, vec2 P);

	// 内部光栅化器
	void rasterize(vec4* screen_coords, IShader* shader);
};

