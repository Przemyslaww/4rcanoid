#pragma once


class Block : public GameObject {

	public:
	Block(Renderer& renderer, ImageLoader& imageLoader, const std::string& color, int m_x, int m_y) :
		GameObject(renderer,
					imageLoader.loadSurface(g_assetsFolder + "block" + color + ".bmp", renderer), m_x, m_y, getBlockWidth(), getBlockHeight()) {
		
	}

	static int getBlockWidth() {
		return 30;
	}

	static int getBlockHeight() {
		return 30;
	}

	void update() {

	}

		
};