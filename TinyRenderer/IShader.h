#pragma once
#include "tgaimage.h"
#include "geometry.h"

class Model;

class IShader
{
	/* 抽象基类，定义了渲染管线中的可编程部分 */
public:
	virtual ~IShader() = default;

	virtual vec4 vertex(Model *model, int iface, int nthvert, const mat<4, 4>& ViewProjection) = 0;
	/*
		顶点着色器 Vertex Shader
		输入: 模型，模型上第iface个面，第nthvert个顶点, 组合好的VP矩阵
		输出: 变换后的"裁剪空间"坐标
	*/

	virtual bool fragment(vec3 barycentric, TGAColor& color) = 0;
	/* 
		片段着色器 Fragment Shader
		输入: 像素的重心坐标
		输出: 最终的颜色（传出参数返回）
		返回值： true则绘制，false则丢弃
	*/
};

