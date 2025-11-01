#include "PhongShader.h"

vec4 PhongShader::vertex(Model* model, int iface, int nthvert, const mat<4, 4>& ViewProjectionMatrix)
{
	// ģ���ж�ȡ��ԭʼ����
	vec4 local_position = model->vert(iface, nthvert);
	vec4 local_n = model->normal(iface, nthvert);
	vec2 local_uv = model->uv(iface, nthvert);

	// ת����������ϵ֮����������fragment_shader��ֵ
	varying_normals_world[nthvert] = normalized( (modelMatrix_invert_transpose * local_n).xyz() ); // ������model����ת��
	varying_pos_world[nthvert] = (modelMatrix * local_position).xyz();
	varying_uvs[nthvert] = local_uv;

	// ����MVP�任�������
	return ViewProjectionMatrix * modelMatrix * local_position;
}

bool PhongShader::fragment(vec3 barycentric, TGAColor& out_color)
{
	/* ���Բ�ֵ */
	double alpha = barycentric.x, beta = barycentric.y, gamma = barycentric.z;
	// ���߲�ֵ�����µ�λ��
	vec3 interpolate_normal = normalized(varying_normals_world[0] * alpha + varying_normals_world[1] * beta + varying_normals_world[2] * gamma);
	// ��ֵuv����
	vec2 interpolate_uv = varying_uvs[0] * alpha + varying_uvs[1] * beta + varying_uvs[2] * gamma;
	// ��ֵ��������
	vec3 interpolate_pos = varying_pos_world[0] * alpha + varying_pos_world[1] * beta + varying_pos_world[2] * gamma;


	/* ������� TODO */
	//TGAColor diffuse_map_color = model_ptr->diffuse().get(interpolate_uv.x * model_ptr->diffuse().width(), interpolate_uv.y * model_ptr->diffuse().height());
	//float    specular_map_intensity = model_ptr->specular().get(interpolate_uv.x * model_ptr->specular().width(), interpolate_uv.y * model_ptr->specular().height()).bgra[0] / 255.f;
	
	// ��ʱûʹ�������ù̶��İ�ɫ��������Ļ�����ɫ
	TGAColor diffuse_color = TGAColor({ 255, 255, 255, 255 }); 
	// �ù̶���1.0����߹���ͼǿ��
	float specular_intensity = 1.0f;
	// �����ϵ��
	float specular_power = 30.f;

	/* Ӧ��Blinn Pongģ�� */
	// ������ (��ģ�͵ĵĻ������������屾�����ɫ����һ��������ϵ���������岻����̫��)
	float ka = 0.1f;
	vec3 ambient = { diffuse_color.bgra[2] * ka, diffuse_color.bgra[1] * ka, diffuse_color.bgra[0] * ka };

	// �����䣨��ģ���в����ǳ��䷽�򣬿��������ط��ߺ͹�Դ����֮��ļнǣ������������Թ�Դ��kdΪ1������90����Ϊ0���׸�max��ֹ��͸��
	float kd = max(0.f, static_cast<float>(interpolate_normal * light_dir_world));
	vec3 diffuse = { diffuse_color.bgra[2] * kd, diffuse_color.bgra[1] * kd, diffuse_color.bgra[0] * kd };

	/* ============================================================================================
	// �߹���(Phongģ��)
	vec3 view_dir = normalized(camera_pos_world - interpolate_pos); // �۲��߷���
	vec3 reflect_dir = normalized(2 * kd * interpolate_normal - light_dir_world); // 
	float Ks_intensity = std::pow(max(0.f, (float)(view_dir * reflect_dir)), specular_power); // �����ǿ�ȣ������ϵ�����Կ��Ƹ߹�㷶Χ
	vec3 specular = vec3({ 255, 255, 255 }) * Ks_intensity * specular_intensity; // ����߹���ɫ
	 ============================================================================================== */

	// �߹���(Blinn-Phong ģ��ʹ�ð��������������)
	vec3 view_dir = normalized(camera_pos_world - interpolate_pos); // �۲��߷���
	vec3 h = normalized(light_dir_world + view_dir); // �������
	float Ks_intensity = std::pow(max(0.f, interpolate_normal * h), specular_power); // ��������뷨�������������ƶ��������ǿ�ȣ������ϵ�����Կ��Ƹ߹�㷶Χ
	vec3 specular = vec3({ 255, 255, 255 }) * Ks_intensity * specular_intensity; // ����߹���ɫ

	// �ϲ���ɫ
	vec3 final_color_vec = ambient + diffuse + specular;

	/* ��ɫ */
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
