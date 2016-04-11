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
					if (blockLines[i][j] == '#') {
						blocks.push_back(new Block(renderer, imageLoader, g_greenColor,
							x + offsetX*Block::getBlockWidth(), y + offsetY*Block::getBlockHeight()));
						offsetX++;
					}
					else offsetX++;
				}
				offsetX = 0;
				offsetY++;
				
			}

		}

		
		~BlocksGrid() {
			for (auto& block : blocks) {
				delete block;
			}
		}
};