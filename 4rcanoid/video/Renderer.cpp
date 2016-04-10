#include "../header.hpp"

void Renderer::drawObjects() {
	for (auto& renderable : renderableGameObjects) {
		renderable->draw();
	}
}