#include "MainMenu.h"

MainMenu::MainMenu(float width, float height)
{
	if (!font.loadFromFile("fonts/sylfaen.ttf"))
	{
		// ERROR
	}

	menu[0].setFont(font);
	menu[0].setFillColor(sf::Color::Blue);
	menu[0].setString("PLAY");
	menu[0].setPosition(sf::Vector2f(width / 2, height / (MAX_NUMBER_OF_OPTIONS + 1) * 1));

	menu[1].setFont(font);
	menu[1].setFillColor(sf::Color::White);
	menu[1].setString("MULITPLAYER");
	menu[1].setPosition(sf::Vector2f(width / 2, height / (MAX_NUMBER_OF_OPTIONS + 1) * 2));

	menu[2].setFont(font);
	menu[2].setFillColor(sf::Color::White);
	menu[2].setString("EXIT");
	menu[2].setPosition(sf::Vector2f(width / 2, height / (MAX_NUMBER_OF_OPTIONS + 1) * 3));


	selectedOption = 0;

}

MainMenu::~MainMenu()
{
}

void MainMenu::draw(sf::RenderWindow& app)
{

	for (int i = 0; i < MAX_NUMBER_OF_OPTIONS; i++)
	{
		app.draw(menu[i]);
	}


}

void MainMenu::moveUp()
{
	if (selectedOption -1 >= 0)
	{
		menu[selectedOption].setFillColor(sf::Color::White);
		selectedOption--;
		menu[selectedOption].setFillColor(sf::Color::Blue);
	}
}

void MainMenu::moveDown()
{

	if (selectedOption + 1 < MAX_NUMBER_OF_OPTIONS)
	{
		menu[selectedOption].setFillColor(sf::Color::White);
		selectedOption++;
		menu[selectedOption].setFillColor(sf::Color::Blue);
	}

}


