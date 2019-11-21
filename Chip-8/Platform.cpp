#include "Platform.h"
#include "main.h"

Platform::Platform(unsigned int width, unsigned int height) 
{
	window.create(sf::VideoMode(width, height), "Chip-8");
	chip8Gui.setTarget(window);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	tgui::Label::Ptr label = tgui::Label::create("Hello World!");
	tgui::Button::Ptr button = tgui::Button::create("Press Me!");

	sf::Vector2u window_size = window.getSize();
	label->setPosition(window_size.x/2 - label->getSize().x/2, window_size.y/2);
	label->setAutoSize(false);

	button->setPosition(window_size.x / 2 - button->getSize().x/2, window_size.y / 2 - 50);

	chip8Gui.add(label, "hw");
	chip8Gui.add(button, "button");
	
}

void Platform::draw(bool buffer[], unsigned int width, unsigned int height)
{

	sf::Vector2f pixel_size(16, 16);
	sf::RectangleShape pixel;
	pixel.setFillColor(sf::Color::White);
	window.clear(sf::Color::Black);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			pixel.setSize(pixel_size);
			sf::Vector2f pixel_pos(x * pixel_size.x, y * pixel_size.y);
			if (buffer[x + width * y]) {
				pixel.setPosition(pixel_pos);
				window.draw(pixel);
			}
		}
	}
	
	
	window.display();
}

void Platform::getInput(bool* array) 
{
	using namespace sf;
	
	array[0] = Keyboard::isKeyPressed(Keyboard::X);
	array[1] = Keyboard::isKeyPressed(Keyboard::Num1);
	array[2] = Keyboard::isKeyPressed(Keyboard::Num2);
	array[3] = Keyboard::isKeyPressed(Keyboard::Num3);
	array[4] = Keyboard::isKeyPressed(Keyboard::Q);
	array[5] = Keyboard::isKeyPressed(Keyboard::W);
	array[6] = Keyboard::isKeyPressed(Keyboard::E);
	array[7] = Keyboard::isKeyPressed(Keyboard::A);
	array[8] = Keyboard::isKeyPressed(Keyboard::S);
	array[9] = Keyboard::isKeyPressed(Keyboard::D);
	array[10] = Keyboard::isKeyPressed(Keyboard::Z);
	array[11] = Keyboard::isKeyPressed(Keyboard::C);
	array[12] = Keyboard::isKeyPressed(Keyboard::Num4);
	array[13] = Keyboard::isKeyPressed(Keyboard::R);
	array[14] = Keyboard::isKeyPressed(Keyboard::F);
	array[15] = Keyboard::isKeyPressed(Keyboard::V);
}

void Platform::update(bool buffer[], unsigned int width, unsigned int height, bool should_draw)
{
	sf::Event e;
	if (window.pollEvent(e)) {
		switch (e.type) {
		case sf::Event::EventType::Closed:
			window.close();
			quit();
			break;
		}
		chip8Gui.handleEvent(e);
	}
	window.clear();

	chip8Gui.draw();
	window.display();
}

