#pragma once

class GameObject {
	protected:
	Renderer& renderer;
	SDL_Texture* image;

	double x;
	double y;
		
	unsigned width;
	unsigned height;

	public:

		void setPosition(int m_x, int m_y) {
			x = m_x;
			y = m_y;
		}

		int getWidth() const {
			return width;
		}
		
		int getBitmapX() const {
			return x - getWidth() / 2;
		}

		int getBitmapY() const {
			return y - getHeight() / 2;
		}

		double getX() {
			return x;
		}

		double getY() {
			return y;
		}

		int getHeight() const {
			return height;
		}

		GameObject(Renderer& m_renderer, SDL_Texture* m_image, int m_x, int m_y) : renderer(m_renderer) {
			image = m_image;
			x = m_x;
			y = m_y;
			int w, h;
			SDL_QueryTexture(image, NULL, NULL, &w, &h);
			width = w;
			height = h;
		}

		GameObject(Renderer& m_renderer, SDL_Texture* m_image, int m_x, int m_y, unsigned w, unsigned h) : renderer(m_renderer) {
			x = m_x;
			y = m_y;
			width = w;
			height = h;
			image = m_image;
		}

		virtual void update() = 0;



		virtual void moveBy(double m_x, double m_y) {
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