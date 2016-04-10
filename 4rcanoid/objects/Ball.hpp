#pragma once

class Ball : public GameObject {

	public:
	Ball(Renderer& renderer, ImageLoader& imageLoader, int m_x, int m_y) : 
		GameObject(renderer, imageLoader.loadSurface(g_assetsFolder + "ball.bmp", renderer), m_x, m_y, 10, 10) {

	}
};