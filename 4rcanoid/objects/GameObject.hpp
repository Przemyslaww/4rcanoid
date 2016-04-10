#pragma once

class GameObject {
	protected:
	Renderer& renderer;
	SDL_Texture* image;

	int x;
	int y;
		
	unsigned width;
	unsigned height;

	public:
		GameObject(Renderer& m_renderer, SDL_Texture* m_image, int m_x, int m_y, unsigned w, unsigned h) : renderer(m_renderer) {
			x = m_x;
			y = m_y;
			width = w;
			height = h;
			image = m_image;
			renderer.registerRenderable(this);
		}

		virtual void moveBy(int m_x, int m_y) {
			x += m_x;
			y += m_y;
		}

		virtual void draw() {
			renderer.drawSurface(image, x - width / 2, y - height / 2, width, height);
		}

		virtual ~GameObject() {
			renderer.unregisterRenderable(this);
		}
};