#pragma once

class TextInputValidator {
	public:
		virtual bool validate(const std::string&) = 0;
};

class IPValidator : TextInputValidator {
	public:
		bool validate(const std::string& text) {
			struct sockaddr_in sa;
			int result = inet_pton(AF_INET, text.c_str(), &(sa.sin_addr));
			return result != 0;
		}
};

class TextInputDialog {

	std::string text;

	int selectionLen;
	std::string composition;
	std::string cursor;

	std::string message;

	Renderer& renderer;

	SDL_Color textColor;
	SDL_Color boxColor;

	bool captureEvent;

	int x, y, w, h;

	public:
		TextInputDialog(const int& m_x, const int& m_y, const int& m_w, const int& m_h, const SDL_Color& m_textColor, const SDL_Color& m_boxColor, const std::string& m_message, Renderer& m_renderer) : renderer(m_renderer) {
			message = m_message;
			textColor = m_textColor;
			boxColor = m_boxColor;
			x = m_x;
			y = m_y;
			w = m_w;
			h = m_h;
			captureEvent = true;
		}

		void redraw() {
			renderer.drawRect(x - 1, y - 1, w + 2, h + 2, textColor);
			renderer.drawRect(x, y, w, h, boxColor);
			renderer.drawText(message, x + 10, y + 10, textColor);
			renderer.drawText(std::string(text) + "_", x + 10, y + 30, textColor);
		}

		std::string getInput() {
			return text;
		}

		void readUserInput(const SDL_Event& event) {
			SDL_bool done = SDL_FALSE;
			SDL_StartTextInput();
			switch (event.type) {
				case SDL_QUIT:
					/* Quit */
					done = SDL_TRUE;
					break;
			    case SDL_TEXTINPUT:
					/* Add new text onto the end of our text */
					if (captureEvent) {
						text += event.text.text;
						captureEvent = false;
					}
					
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_BACKSPACE:
						if (captureEvent) {
							text = text.substr(0, text.size() - 1);
							captureEvent = false;
						}
						
						break;
					}
					break;
		    	case SDL_TEXTEDITING:
					/*
					Update the composition text.
					Update the cursor position.
					Update the selection length (if any).
					*/
					if (captureEvent) {
						composition = event.edit.text;
						cursor = event.edit.start;
						selectionLen = event.edit.length;
						captureEvent = false;
					}
					break;
				default:
					if (!captureEvent) {
						captureEvent = true;
					}
					break;
					
			}

		}
};