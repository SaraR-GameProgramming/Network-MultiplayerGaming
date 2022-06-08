#include <SFML/Graphics.hpp>

#define MAX_NUMBER_OF_OPTIONS 3

class MainMenu
{
private:

	int selectedOption;
	sf::Font font;
	sf::Text menu[MAX_NUMBER_OF_OPTIONS];



public:
	MainMenu(float width, float height);
	~MainMenu();

	void draw(sf::RenderWindow& app);
	void moveUp();
	void moveDown();
	int getPressedOption() { return selectedOption; }
};

