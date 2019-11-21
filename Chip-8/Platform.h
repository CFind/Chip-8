#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include "main.h"

class Platform
{
private:
	tgui::Gui chip8Gui;
	sf::RenderWindow window;
	sf::Clock deltaClock;
	void draw(bool buffer[], unsigned int width, unsigned int height);
public:
	Platform(unsigned int width, unsigned int height);
	void getInput(bool* array);
	void update(bool buffer[], unsigned int width, unsigned int height, bool should_draw);
};
