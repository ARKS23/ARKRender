#pragma once
#include "IShader.h"
#include "model.h"
#include "geometry.h"

/* 深度着色器: 实现shadow mapping */
class DepthShader : public IShader
{
public:
	mat<4, 4> modelMatrix;
	mat<4, 4> view_project_light; // 光源VP矩阵

	float varying_depth[3];

public:
	virtual vec4 vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix) override;

	virtual bool fragment(vec3 barycentric, TGAColor& out_color);

	virtual void setModelMatrix(mat<4, 4> matrix) override;

	virtual mat<4, 4> getModelMatrix() override;
};

