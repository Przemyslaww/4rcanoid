#pragma once

class BlocksGrid {
	std::vector<Block*> blocks;
	int x;
	int y;

	public:
		BlocksGrid(Renderer& renderer, ImageLoader& imageLoader, int m_x, int m_y, const std::string& blocksDescription) {
			x = m_x;
			y = m_y;
			int offsetX = 0;
			int offsetY = 0;
			for (int i = 0; i < blocksDescription.length(); i++) {
				if (blocksDescription[i] == '\n') {
					offsetX = 0;
					offsetY++;
				}
				else if (blocksDescription[i] == '#') {
					blocks.push_back(new Block(renderer, imageLoader, g_greenColor,
						x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
					offsetX++;
				}
				else offsetX++;
			}

		}

		
		~BlocksGrid() {
			for (auto& block : blocks) {
				delete block;
			}
		}
};