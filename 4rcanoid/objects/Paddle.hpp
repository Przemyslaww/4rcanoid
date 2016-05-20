#pragma once

class Paddle : public GameObject {

	unsigned boundaryArea;
	public: bool isHorizontal;
			bool choosen; 
			int o_x;
			int o_y;
			
	public:
		Paddle(Renderer& m_renderer, ImageLoader& imageLoader,
			const std::string& paddleColor,
			int m_x, int m_y,
			double m_isHorizontal,
			unsigned m_boundaryArea,
			bool m_choosen) : 
				GameObject(m_renderer, imageLoader.loadSurface(g_assetsFolder + "paddle" + paddleColor + "Horizontal.bmp", m_renderer), m_x, m_y, paddleWidth, paddleHeight),
				o_x(m_x), o_y(m_y), isHorizontal(m_isHorizontal),choosen(m_choosen), boundaryArea(m_boundaryArea) {}
		
		void moveBy(double m_x, double m_y) {
			if (isHorizontal)
			{
				if ((m_x > 0 && x + m_x <= o_x + boundaryArea) || (m_x < 0 && x - m_x >= o_x - boundaryArea + 13)) {
					x += m_x;
				}
			}
			else
			{
				if ((m_y > 0 && y + m_y <= o_y + boundaryArea) || (m_y < 0 && y - m_y >= o_y - boundaryArea)) {
					y += m_y;
				}
			}
		}

		void draw() {
			if(isHorizontal)
			renderer.drawSurface(image, x - width / 2, y - height / 2, width, height);
			else
			renderer.drawSurfaceVertical(image, x - width / 2, y - height / 2, width, height);
		}
		

		void update() {

		}
};