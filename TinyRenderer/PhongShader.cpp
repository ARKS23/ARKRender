#include "PhongShader.h"

vec4 PhongShader::vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix)
{
	// 模型中读取的原始数据
	vec4 local_position = model->vert(iface, nthvert);
	vec4 local_n = model->normal(iface, nthvert);
	vec2 local_uv = model->uv(iface, nthvert);

	// 转成世界坐标系之后存入变量供fragment_shader插值
	varying_normals_world[nthvert] = normalized( (modelMatrix_invert_transpose * local_n).xyz() ); // 法线用model的逆转置
	varying_pos_world[nthvert] = (modelMatrix * local_position).xyz();
	varying_uvs[nthvert] = local_uv;

	// 返回MVP变换后的坐标
	return ViewProjectionMatrix * modelMatrix * local_position;
}

bool PhongShader::fragment(vec3 barycentric, TGAColor& out_color)
{
	/* 线性插值 */
	double alpha = barycentric.x, beta = barycentric.y, gamma = barycentric.z;
	// 法线插值并重新单位化
	vec3 interpolate_normal = normalized(varying_normals_world[0] * alpha + varying_normals_world[1] * beta + varying_normals_world[2] * gamma);
	// 插值uv坐标
	vec2 interpolate_uv = varying_uvs[0] * alpha + varying_uvs[1] * beta + varying_uvs[2] * gamma;
	// 插值世界坐标
	vec3 interpolate_pos = varying_pos_world[0] * alpha + varying_pos_world[1] * beta + varying_pos_world[2] * gamma;


	/* 纹理采样 TODO */
	//TGAColor diffuse_map_color = model_ptr->diffuse().get(interpolate_uv.x * model_ptr->diffuse().width(), interpolate_uv.y * model_ptr->diffuse().height());
	//float    specular_map_intensity = model_ptr->specular().get(interpolate_uv.x * model_ptr->specular().width(), interpolate_uv.y * model_ptr->specular().height()).bgra[0] / 255.f;
	
	// 暂时没使用纹理，用固定的白色代表物体的基础颜色
	TGAColor diffuse_color = TGAColor({ 255, 255, 255, 255 }); 
	// 用固定的1.0代替高光贴图强度
	float specular_intensity = 1.0f;
	// 光泽度系数
	float specular_power = 30.f;

	/* 应用Blinn Pong模型 */
	// 环境光 (该模型的的环境光是让物体本身的颜色乘以一个环境光系数，让物体不至于太暗)
	float ka = 0.1f;
	vec3 ambient = { diffuse_color.bgra[2] * ka, diffuse_color.bgra[1] * ka, diffuse_color.bgra[0] * ka };

	// 漫反射（该模型中不考虑出射方向，看的是像素法线和光源方向之间的夹角）（若法线正对光源则kd为1，若成90度则为0，套个max防止穿透）
	float kd = max(0.f, static_cast<float>(interpolate_normal * light_dir_world));
	vec3 diffuse = { diffuse_color.bgra[2] * kd, diffuse_color.bgra[1] * kd, diffuse_color.bgra[0] * kd };

	/* ============================================================================================
	// 高光项(Phong模型)
	vec3 view_dir = normalized(camera_pos_world - interpolate_pos); // 观察者方向
	vec3 reflect_dir = normalized(2 * kd * interpolate_normal - light_dir_world); // 
	float Ks_intensity = std::pow(max(0.f, (float)(view_dir * reflect_dir)), specular_power); // 计算光强度，光泽度系数可以控制高光点范围
	vec3 specular = vec3({ 255, 255, 255 }) * Ks_intensity * specular_intensity; // 计算高光颜色
	 ============================================================================================== */

	// 高光项(Blinn-Phong 模型使用半程向量加速运算)
	vec3 view_dir = normalized(camera_pos_world - interpolate_pos); // 观察者方向
	vec3 h = normalized(light_dir_world + view_dir); // 半程向量
	float Ks_intensity = std::pow(max(0.f, interpolate_normal * h), specular_power); // 半程向量与法向量的余弦相似度来计算光强度，光泽度系数可以控制高光点范围
	vec3 specular = vec3({ 255, 255, 255 }) * Ks_intensity * specular_intensity; // 计算高光颜色

	// 合并颜色
	vec3 final_color_vec = ambient + diffuse + specular;

	/* 着色 */
	TGAColor result = { static_cast<int>(min(255.f, final_color_vec.x)),
						static_cast<int>(min(255.f, final_color_vec.y)),
						static_cast<int>(min(255.f, final_color_vec.z)),
						255 
	};
	out_color = result;

	return true;
}

void PhongShader::setModelMatrix(mat<4, 4> matrix)
{
	modelMatrix = matrix;
}

mat<4, 4> PhongShader::getModelMatrix()
{
	return modelMatrix;
}
