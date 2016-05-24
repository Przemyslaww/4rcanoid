#pragma once

class TextBox {

	protected:

	std::string text;

	int x, y, w, h;

	Renderer& renderer;
	SDL_Color textColor;
	SDL_Color boxColor;

	public:
		void redraw() {
			auto lines = split(text,  '\n');
			renderer.drawRect(x-1, y-1, w+2, h+2, textColor);
			renderer.drawRect(x, y, w, h, boxColor);
			int i = 0;
			for (auto& line : lines) {
				renderer.drawText(line, x + 10, y + 20*(i+1), textColor);
				i++;
			}
		}

		TextBox(const int& m_x, const int& m_y, const int& m_w, const int& m_h, const SDL_Color& m_textColor, const SDL_Color& m_boxColor, Renderer& m_renderer, const std::string m_text) :
		   renderer(m_renderer) {
			textColor = m_textColor;
			boxColor = m_boxColor;
			text = m_text;
			x = m_x;
			y = m_y;
			w = m_w;
			h = m_h;
		}

		void setText(const std::string& m_text) {
			text = m_text;
		}

		void removeTextLine(const std::string& m_line) {
			size_t nFPos = text.find(m_line);
			size_t secondNL = text.find('\n', nFPos);
			size_t firstNL = text.rfind('\n', nFPos);
			text.erase(firstNL, secondNL - firstNL);
		}

		std::string getText() {
			return text;
		}

};