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

	// ��������
	void clear_framebuffer(TGAColor color);
	void clear_zbuffer();

	// ���ñ任����
	void set_viewMatrix(mat<4, 4> matrix);
	void set_projectionMatrix(mat<4, 4> matrix);
	void set_viewportMatrix(int x, int y, int w, int h);

	// ���Ļ��ƺ���
	void draw(Model* model, IShader* shader);

	// ��ȡ���
	TGAImage& get_framebuffer();

private:
	TGAImage framebuffer;
	std::vector<float> zbuffer;

	//mat<4, 4> modelMatrix;      // ��������任����(M)��shader��
	mat<4, 4> viewMatrix;       // �������任����(V)
	mat<4, 4> projectionMatrix; // ��ͶӰ�任�ľ���(P)
	mat<4, 4> viewPortMatrix;   // �����ı任�����ص�ı任����

	// �ڲ�����������㸨������
	vec3 barycentric(vec3* pts, vec2 P);

	// �ڲ���դ����
	void rasterize(vec4* screen_coords, IShader* shader);
};

