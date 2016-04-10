#pragma once

class Paddle : public GameObject {
	int o_x;
	int o_y;

	unsigned boundaryArea;

	public:
		Paddle(Renderer& m_renderer, ImageLoader& imageLoader,
			const std::string& paddleColor,
			int m_x, int m_y,
			unsigned m_boundaryArea,
			bool isHorizontal) : 
				GameObject(m_renderer, imageLoader.loadSurface(g_assetsFolder + "paddle" + paddleColor + "Horizontal.bmp", m_renderer), m_x, m_y, 100, 20),
				o_x(m_x), o_y(m_y), boundaryArea(m_boundaryArea) {}
		
		void moveBy(double m_x, double m_y) {
			if ((m_x > 0 && x + m_x <= o_x + boundaryArea)
			 || (m_x < 0 && x - m_x >= o_x - boundaryArea)) {
				x += m_x;
			}

			if ((m_y > 0 && y + m_y <= o_y + boundaryArea)
				|| (m_y < 0 && y - m_y >= o_y - boundaryArea)) {
				y += m_y;
			}
		}

		void update() {

		}
};