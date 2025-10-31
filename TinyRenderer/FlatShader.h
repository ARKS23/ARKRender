#pragma once
#include "IShader.h"
#include "model.h"


class FlatShader : public IShader
{
public:
	mat<4, 4> modelMatrix;

	virtual vec4 vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix) {
		// TODO: ���մ���

		// MVP�任
		vec4 v_local = model->vert(iface, nthvert);  // ��obj�л�ȡ�ľֲ�����

		// ���زü��ռ�����
		return ViewProjectionMatrix * modelMatrix * v_local;
	}

	virtual bool fragment(vec3 barycentric, TGAColor& out_color) override {
		// TODO: ���մ���
		return true;
	}
};

