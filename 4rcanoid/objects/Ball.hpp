#pragma once

class Ball : public GameObject {

	double ballSpeed;
	double moveAngle;

	public:
	Ball(Renderer& renderer, ImageLoader& imageLoader, int m_x, int m_y, double m_ballSpeed = 1.0, double m_moveAngle = 0.0) : 
		GameObject(renderer, imageLoader.loadSurface(g_assetsFolder + "ball.bmp", renderer), m_x, m_y, 10, 10) {
		ballSpeed = m_ballSpeed;
		moveAngle = m_moveAngle;
	}

	void update() {
		moveBy(ballSpeed * cos(moveAngle), ballSpeed * sin(moveAngle));
	}

	void rotateMovementDirectionBy(double value) {
		moveAngle += value;
	}
};