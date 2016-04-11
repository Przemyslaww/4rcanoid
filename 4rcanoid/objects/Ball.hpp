#pragma once

class Ball : public GameObject {

	double ballSpeed;

	double moveX;
	double moveY;

	public:
	Ball(Renderer& renderer, ImageLoader& imageLoader, int m_x, int m_y, double m_ballSpeed = 1.0, double m_moveAngle = 0.0) : 
		GameObject(renderer, imageLoader.loadSurface(g_assetsFolder + "ball.bmp", renderer), m_x, m_y, getBallWidth(), getBallWidth()) {
		ballSpeed = m_ballSpeed;
		moveX = ballSpeed * cos(m_moveAngle);
		moveY = ballSpeed * sin(m_moveAngle);
	}

	void rotateWithNormal(bool rotateRight) {
		if (rotateRight) {
			if (moveX > 0 && moveY > 0) {
				moveX = -moveX;
			}
			else if (moveX < 0 && moveY > 0) {
				moveY = -moveY;
			}
			else if (moveX < 0 && moveY < 0) {
				moveX = -moveX;
			}
			else if (moveX > 0 && moveY < 0) {
				moveY = -moveY;
			}
			
		}
		else {
			if (moveX < 0 && moveY < 0) {
				moveY = -moveY;
			}
			else if (moveX < 0 && moveY > 0) {
				moveX = -moveX;
			}
			else if (moveX > 0 && moveY > 0) {
				moveY = -moveY;
			}
			else if (moveX > 0 && moveY < 0) {
				moveX = -moveX;
			}
		}
	}

	void update() {
		if (y + moveY < 0 && moveX < 0 && moveY < 0) rotateWithNormal (false);
		else if (y + moveY > SCREEN_HEIGHT && moveX > 0 && moveY > 0) rotateWithNormal(false);
		else if (x + moveX < 0 && moveX < 0 && moveY > 0) rotateWithNormal(false);
		else if (x + moveX > SCREEN_WIDTH && moveX > 0 && moveY < 0) rotateWithNormal(false);

		else if (y + moveY < 0 && moveX > 0 && moveY < 0) rotateWithNormal(true);
		else if (y + moveY > SCREEN_HEIGHT && moveX < 0 && moveY > 0) rotateWithNormal(true);
		else if (x + moveX < 0 && moveX < 0 && moveY < 0) rotateWithNormal(true);
		else if (x + moveX > SCREEN_WIDTH && moveX > 0 && moveY > 0) rotateWithNormal(true);

		moveBy(moveX, moveY);
		
	}

	static unsigned getBallWidth() {
		return 15;
	}

	static unsigned getBallHeight() {
		return 15;
	}

	void detectCollisions(Paddle& paddle) {
		int startX = paddle.getBitmapX();
		int startY = paddle.getBitmapY();
		int paddleWidth = paddle.getWidth();
		int paddleHeight = paddle.getHeight();

		if (x >= startX && y >= startY && x <= startX + paddleWidth && y <= startY + paddleWidth) {
			if (moveX < 0) rotateWithNormal(true);
			else rotateWithNormal(false);
		}
	}

	void detectCollisions(BlocksGrid& blockGrid) {
		for (auto& block : blockGrid.getBlocks()) {
			int startX = block->getBitmapX();
			int startY = block->getBitmapY();
			int blockWidth = block->getWidth();
			int blockHeight = block->getHeight();
			if (x >= startX && y >= startY && x <= startX + blockWidth && y <= startY + blockWidth) {
				if (moveX > 0 && moveY > 0) {
					rotateWithNormal(false);
				}
				else if (moveX < 0 && moveY > 0) {
					rotateWithNormal(false);
				}
				else if (moveX < 0 && moveY < 0) {
					rotateWithNormal(true);
				}
				else if (moveX > 0 && moveY < 0) {
					rotateWithNormal(true);
				}
				blockGrid.destroyBlock(block);
				return;
			}
		}
	}

};