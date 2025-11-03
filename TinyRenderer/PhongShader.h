#pragma once
#include "IShader.h"
#include "model.h"
#include "mergemod.h"

class PhongShader : public IShader
{
public:
	mat<4, 4> modelMatrix_invert_transpose; // model矩阵的逆转置（后续用于变换法线）
	vec3 camera_pos_world;					// 相机在世界中的位置
	vec3 light_dir_world;					// 光源在世界中的方向
	Model* model_ptr = nullptr;				// 指向模型来访问纹理

	/* shadow mapping */
	TGAImage* shadow_map_texture = nullptr;  // 阴影贴图
	mat<4, 4> matrix_shadow_transform;			 // 顶点从世界空间变换到光源屏幕空间的矩阵


public:
	virtual vec4 vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix);

	virtual bool fragment(vec3 barycentric, TGAColor& out_color) override;

	virtual void setModelMatrix(mat<4, 4> matrix) override;

	virtual mat<4, 4> getModelMatrix() override;

private:
	mat<4, 4> modelMatrix;					// model矩阵
	vec3 varying_normals_world[3];			// 变换到世界空间的法线
	vec2 varying_uvs[3];					// 变换到世界空间的uv坐标
	vec3 varying_pos_world[3];				// 变换到世界空间的顶点位置
	mat<3, 3> varying_TBN[3];				// TBN矩阵：Tangent-to-World
};

