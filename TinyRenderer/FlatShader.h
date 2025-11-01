#pragma once
#include "IShader.h"
#include "model.h"


class FlatShader : public IShader
{
public:
	virtual vec4 vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix) {
		// MVP变换
		vec4 v_local = model->vert(iface, nthvert);  // 从obj中获取的局部坐标

		// 返回裁剪空间坐标
		return ViewProjectionMatrix * modelMatrix * v_local;
	}

	virtual bool fragment(vec3 barycentric, TGAColor& out_color) override {
		// TODO: 光照处理
		return true;
	}

	virtual void setModelMatrix(mat<4, 4> matrix) override {
		modelMatrix = matrix;
	}

	virtual mat<4, 4> getModelMatrix() override {
		return modelMatrix;
	}

private:
	mat<4, 4> modelMatrix;
};

