#pragma once

class Ball : public GameObject {

	double ballSpeed;
	public: double moveX;
	public: double moveY;

	public:
	Ball(Renderer& renderer, ImageLoader& imageLoader, int m_x, int m_y, double m_ballSpeed = 1.0, double m_moveAngle = 0.0) : 
		GameObject(renderer, imageLoader.loadSurface(g_assetsFolder + "ball.bmp", renderer), m_x, m_y, getBallWidth(), getBallWidth()) {
		ballSpeed = m_ballSpeed;
		moveX = ballSpeed * cos(m_moveAngle);
		moveY = ballSpeed * sin(m_moveAngle);
	}

	void setMovementAngle(double angle) {
		moveX = ballSpeed * cos(angle);
		moveY = ballSpeed * sin(angle);
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

	void angle() {
		double tmp = moveX;
		moveX = moveY;
		moveY = -1 * tmp;		
	}
	
	void update() {
		double nextX = x + moveX;
		double nextY = y + moveY;
		if (y + moveY < 0 && moveX < 0 && moveY < 0) rotateWithNormal (false);

		else if (x + moveX < 0 && moveX < 0 && moveY > 0) rotateWithNormal(false);
		else if (x + moveX > SCREEN_WIDTH && moveX > 0 && moveY < 0) rotateWithNormal(false);
		else if (y + moveY < 0 && moveX > 0 && moveY < 0) rotateWithNormal(true);
		else if (x + moveX < 0 && moveX < 0 && moveY < 0) rotateWithNormal(true);
		else if (x + moveX > SCREEN_WIDTH && moveX > 0 && moveY > 0) rotateWithNormal(true);
		
		else if (nextX >= 0 && nextX <= wallWidth && nextY >= 0 && nextY <= wallWidth && (nextY + nextX) <= wallWidth) angle(); //rotateWithNormal(true);
		else if (nextX >= 0 && nextX <= wallWidth && nextY >= (SCREEN_HEIGHT - wallWidth) && nextY <= SCREEN_HEIGHT && (nextY - nextX) >= (SCREEN_HEIGHT - wallWidth)) angle(); // rotateWithNormal(true);
		//felerny
		else if (nextX >= (SCREEN_WIDTH - wallWidth) && nextX <= SCREEN_WIDTH && nextY >= 0 && nextY <= wallWidth && (nextY - nextX) <= (wallWidth - SCREEN_HEIGHT)) angle(); // rotateWithNormal(true);
		else if (nextX >= (SCREEN_WIDTH - wallWidth) && nextX <= SCREEN_WIDTH && nextY >= (SCREEN_HEIGHT - wallWidth) && (nextY <= SCREEN_HEIGHT) && (nextX + nextY) >= (SCREEN_HEIGHT + SCREEN_WIDTH - wallWidth)) angle(); // rotateWithNormal(true);

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
		if (paddle.isHorizontal)
		{
			if (paddle.choosen)	//down
			{
				if (x + moveX >= startX && y + moveY >= startY && x + moveX <= startX + paddleWidth) {
					double position = paddle.getX();
					double BALL_RADIUS = Ball::getWidth() / 2;
					double radius = paddle.getWidth() / 2;
					double velocity = sqrt((moveX*moveX) + (moveY*moveY));
					double kx = ((position - getX()) / ((BALL_RADIUS + radius) / velocity));
					double ky = sqrt((velocity * velocity) - (kx * kx));
					moveX = -kx;
					moveY = -fabs(ky);
				}
			}
			else	//up
			{
				if (x + moveX >= startX && y + moveY <= startY + paddleHeight && x + moveX <= startX + paddleWidth) {
					double position = paddle.getX();
					double BALL_RADIUS = Ball::getWidth() / 2;
					double radius = paddle.getWidth() / 2;
					double velocity = sqrt((moveX*moveX) + (moveY*moveY));
					double kx = ((position - getX()) / ((BALL_RADIUS + radius) / velocity));
					double ky = sqrt((velocity * velocity) - (kx * kx));
					moveX = -kx;
					moveY = fabs(ky);
				}
			}
		}
		else
		{

			startX = paddle.getX() - paddle.getHeight() /2;
			startY = paddle.getY() - paddle.getWidth() /2;
			int paddleWidthN = paddle.getHeight();
			int paddleHeightN = paddle.getWidth();
			if (paddle.choosen)	//	left
			{
				if (y + moveY >= startY && x + moveX <= startX + paddleWidthN && y + moveY <= startY + paddleHeightN) {
					double position = paddle.getY();
					double BALL_RADIUS = Ball::getWidth() / 2;
					double radius = paddle.getWidth() / 2;
					double velocity = sqrt((moveX*moveX) + (moveY*moveY));
					double ky = ((position - getY()) / ((BALL_RADIUS + radius) / velocity));
					double kx = sqrt((velocity * velocity) - (ky * ky));
					moveX = fabs(kx);
					moveY = -ky;
				}
			}
			else	//	right
			{
				if (y + moveY >= startY && x + moveX >= startX  && y + moveY <= startY + paddleHeightN) {
					double position = paddle.getY();
					double BALL_RADIUS = Ball::getWidth() / 2;
					double radius = paddle.getWidth() / 2;
					double velocity = sqrt((moveX*moveX) + (moveY*moveY));
					double ky = ((position - getY()) / ((BALL_RADIUS + radius) / velocity));
					double kx = sqrt((velocity * velocity) - (ky * ky));
					moveX = -fabs(kx);
					moveY = -ky;
				}
			}
		}
	}

	void detectCollisions(BlocksGrid& blockGrid) {
		for (auto& block : blockGrid.getBlocks()) {
			int startX = block->getBitmapX();
			int startY = block->getBitmapY();
			int blockWidth = block->getWidth();
			int blockHeight = block->getHeight();
			if (x + moveX >= startX && y + moveY >= startY && x + moveX <= startX + blockWidth && y + moveY <= startY + blockHeight) {
				if ((x < startX || x > startX + blockWidth) && y > startY && y < startY + blockHeight) {
					moveX *= -1;
				}
				if ((y < startY || y > startY + blockHeight) && x > startX && x < startX + blockWidth) {
					moveY *= -1;
				}
				blockGrid.destroyBlock(block);
				return;
			}
		}
	}

};