#pragma once
#include "IShader.h"
#include "model.h"
#include "mergemod.h"

class PhongShader : public IShader
{
public:
	mat<4, 4> modelMatrix_invert_transpose; // model�������ת�ã��������ڱ任���ߣ�
	vec3 camera_pos_world;					// ����������е�λ��
	vec3 light_dir_world;					// ��Դ�������еķ���
	Model* model_ptr = nullptr;				// ָ��ģ������������


public:
	virtual vec4 vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix);

	virtual bool fragment(vec3 barycentric, TGAColor& out_color) override;

	virtual void setModelMatrix(mat<4, 4> matrix) override;

	virtual mat<4, 4> getModelMatrix() override;

private:
	mat<4, 4> modelMatrix;					// model����
	vec3 varying_normals_world[3];			// �任������ռ�ķ���
	vec2 varying_uvs[3];					// �任������ռ��uv����
	vec3 varying_pos_world[3];				// �任������ռ�Ķ���λ��
};

