#include "DepthShader.h"

vec4 DepthShader::vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix) {
	vec4 local_pos = model->vert(iface, nthvert);
	vec4 v_clip = ViewProjectionMatrix * modelMatrix * local_pos; // MVP

	varying_depth[nthvert] = v_clip.z / v_clip.w; // NDC深度

	return v_clip;
}

bool DepthShader::fragment(vec3 barycentric, TGAColor& out_color)
{
	double alpha = barycentric.x, beta = barycentric.y, gamma = barycentric.z;
	float depth = alpha * varying_depth[0] + beta * varying_depth[1] + gamma * varying_depth[2]; // 深度线性插值

	int gary = static_cast<int>(depth * 255.f); // 深度从[0, 1]映射到[0, 255]的灰度
	TGAColor result = { gary, gary, gary, 255 };
	out_color = result;

	return true;
}


void DepthShader::setModelMatrix(mat<4, 4> matrix) {
	modelMatrix = matrix;
}

mat<4, 4> DepthShader::getModelMatrix(){
	return modelMatrix;
}