#pragma once

class BlocksGrid {
	std::vector<Block*> blocks;

	public:
		BlocksGrid(Renderer& renderer, ImageLoader& imageLoader, const std::string& blocksDescription) {

			std::vector<std::string> blockLines = split(blocksDescription, '\n');

			int x = SCREEN_WIDTH/2 + Block::getBlockWidth()/2;
			int y = SCREEN_HEIGHT/2;

			int offsetX = 0;
			int offsetY = -(int)blockLines.size() / 2;
			for (int i = 0; i < blockLines.size(); i++) {

				int size = blockLines[i].length();
				offsetX = -size / 2;

				for (int j = 0; j < blockLines[i].length(); j++) {
					if (blockLines[i][j] == 'g') {
						blocks.push_back(new Block(renderer, imageLoader, g_greenColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else if (blockLines[i][j] == 'y') {
						blocks.push_back(new Block(renderer, imageLoader, g_yellowColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else if (blockLines[i][j] == 'r') {
						blocks.push_back(new Block(renderer, imageLoader, g_redColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else if (blockLines[i][j] == 'b') {
						blocks.push_back(new Block(renderer, imageLoader, g_blueColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else offsetX++;
				}
				offsetX = 0;
				offsetY++;
				
			}

		}

		void destroyBlock(Block* block) {
			auto it = std::find(blocks.begin(), blocks.end(), block);
			if (it != blocks.end()) {
				delete block;
				blocks.erase(it);
			}
		}

		void setBlocksDescription(Renderer& renderer, ImageLoader& imageLoader, const std::string& blocksDescription) {
			for (auto& block : blocks) {
				delete block;
			}

			std::vector<std::string> blockLines = split(blocksDescription, '\n');

			int x = SCREEN_WIDTH / 2 + Block::getBlockWidth() / 2;
			int y = SCREEN_HEIGHT / 2;

			int offsetX = 0;
			int offsetY = -(int)blockLines.size() / 2;
			for (int i = 0; i < blockLines.size(); i++) {

				int size = blockLines[i].length();
				offsetX = -size / 2;

				for (int j = 0; j < blockLines[i].length(); j++) {
					if (blockLines[i][j] == 'g') {
						blocks.push_back(new Block(renderer, imageLoader, g_greenColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else if (blockLines[i][j] == 'y') {
						blocks.push_back(new Block(renderer, imageLoader, g_yellowColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else if (blockLines[i][j] == 'r') {
						blocks.push_back(new Block(renderer, imageLoader, g_redColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else if (blockLines[i][j] == 'b') {
						blocks.push_back(new Block(renderer, imageLoader, g_blueColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else offsetX++;
				}
				offsetX = 0;
				offsetY++;

			}


		}

		void draw() {
			for (auto& block : blocks) {
				block->draw();
			}
		}

		std::vector<Block*>& getBlocks() {
			return blocks;
		}
		
		~BlocksGrid() {
			for (auto& block : blocks) {
				delete block;
			}
		}
};