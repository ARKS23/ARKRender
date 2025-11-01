#include "Renderer.h"

Renderer::Renderer(int w, int h) : framebuffer(w, h, TGAImage::RGB), zbuffer(w * h, -std::numeric_limits<float>::infinity()){}

void Renderer::clear_framebuffer(TGAColor color)
{
	// ���bufferֱ�����´���һ������
	framebuffer = TGAImage(framebuffer.width(), framebuffer.height(), TGAImage::RGB, color);
}

void Renderer::clear_zbuffer()
{
	// zbuffer���
	std::fill(zbuffer.begin(), zbuffer.end(), -std::numeric_limits<float>::infinity());
}

void Renderer::set_viewMatrix(mat<4, 4> matrix)
{
	viewMatrix = matrix;
}

void Renderer::set_projectionMatrix(mat<4, 4> matrix)
{
	projectionMatrix = matrix;
}

void Renderer::set_viewportMatrix(int x, int y, int w, int h)
{
	viewPortMatrix = { 
						{w / 2.0, 0, 0, x + w / 2.0},
						{0, h / 2.0, 0, y + h / 2.0},
						{0, 0, 1.0, 0},
						{0, 0, 0, 1} 
					  };
}

void Renderer::draw(Model* model, IShader* shader)
{
	/* �ⲿ���õĻ��ƺ��� */
	mat<4, 4> VP = projectionMatrix * viewMatrix;

	for (int i = 0; i < model->nfaces(); ++i) { // �洦��
		vec4 clip_coords[3];   // �ü��ռ�����
		vec4 screen_coords[3]; // ���յ���Ļ����

		// ������ɫ��
		for (int j = 0; j < 3; ++j) {
			clip_coords[j] = shader->vertex(model, i, j, VP);
		}

		// �̶�����: NDC + �ӿڱ任
		for (int j = 0; j < 3; ++j) {
			screen_coords[j] = viewPortMatrix * (clip_coords[j] / clip_coords[j].w);
		}

		// �̶����ߣ���դ��
		rasterize(screen_coords, shader);
	}
}

TGAImage& Renderer::get_framebuffer()
{
	return framebuffer;
}

vec3 Renderer::barycentric(vec3* pts, vec2 P)
{
	/* ���������� */
	vec3 v1 = vec3{ pts[1].x - pts[0].x, pts[2].x - pts[0].x, pts[0].x - P.x };
	vec3 v2 = vec3{ pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - P.y };

	vec3 c = cross(v1, v2);

	if (std::abs(c.z) < 1) return vec3{ - 1, 1, 1}; // �˻�������

	// (1-u-v, u, v)
	return vec3{ 1.f - (c.x + c.y) / c.z, c.x / c.z, c.y / c.z };
}

void Renderer::rasterize(vec4* screen_coords, IShader* shader)
{
	// ��Χ���Ż�
	vec2 bbox_min = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	vec2 bbox_max = { -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };
	for (int i = 0; i < 3; ++i) {
		bbox_min.x = std::min(bbox_min.x, screen_coords[i].x);
		bbox_max.x = std::max(bbox_max.x, screen_coords[i].x);
		bbox_min.y = std::min(bbox_min.y, screen_coords[i].y);
		bbox_max.y = std::max(bbox_max.y, screen_coords[i].y);
	}

	// ��������
	vec3 pts[3]; // �洢3D��Ļ���꣨x, y, z��
	for (int i = 0; i < 3; ++i) pts[i] = screen_coords[i].xyz();

	// ================================================================================����
	// TGAColor test_color;
	// for (int c = 0; c < 3; c++) test_color[c] = std::rand() % 255; // �����ɫ����
	// ================================================================================����


	for (int x = static_cast<int>(bbox_min.x); x <= static_cast<int>(bbox_max.x); ++x) {
		for (int y = static_cast<int>(bbox_min.y); y <= static_cast<int>(bbox_max.y); ++y) {
			vec2 curP = { x, y };
			vec3 bary = barycentric(pts, curP);
			if (bary.x < 0 || bary.y < 0 || bary.z < 0) { continue; } // �����������ڲ�

			// ��Ȳ���
			float z_interopolated = pts[0].z * bary.x + pts[1].z * bary.y + pts[2].z * bary.z; //�����������Բ�ֵ
			int z_buffer_index = x + y * framebuffer.width();


			if (z_interopolated > zbuffer[z_buffer_index]) {
				TGAColor color;
				if (!shader->fragment(bary, color)) continue; // TODO: shader���������򲻽��в���
				zbuffer[z_buffer_index] = z_interopolated;
				framebuffer.set(x, y, color);
			}
		}
	}
}






