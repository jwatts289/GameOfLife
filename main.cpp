#include "definitions.h"

int main()
{
	int frameLength = 150; //milliseconds
	int size = 100;
	std::vector<sf::Color> colors = { 
		sf::Color::Black, 
		sf::Color::Red};
	int defaultColor = 0;
	int edgeType = 2; // 0: dead, 1: alive, 2: wraps
	GridProperties grid(sf::Vector2i(size,size), colors, defaultColor, edgeType);

	display(grid, frameLength);

	return 0;
}

void display(GridProperties grid, int frameLength)
{
	sf::Vector2i windowSize = grid.findWindowSize();
	sf::RenderWindow window(sf::VideoMode(windowSize.x,windowSize.y), "Game of Life");

	grid.initialiseGrid();
	std::vector<int> newGridpointValues(grid.getNumGridpoints());

	grid.updateGrid(grid.generateRandomGridpointValues(),frameLength);

	bool wait = 0;
	bool edit = 0;
	bool mouseClick = 0;
	sf::Vector2i mousePosition;

	std::cout << "To pause press 'P', to edit press 'E' while paused\n";

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			mouseClick = 0;

			if (event.type == sf::Event::Closed) //closes window
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Escape: window.close(); //closes window
					break;

				case sf::Keyboard::P: //pauses
					wait = !wait;
					if (!wait) edit = 0;
					std::cout << "wait = " << wait << "	";
					std::cout << "edit = " << edit << "\n";
					break;

				case sf::Keyboard::E: //allows editing while paused
					if (wait)
					{
						edit = !edit;
						std::cout << "		edit = " << edit << "\n";
					}
					break;

				case sf::Keyboard::R:
					grid.updateGrid(grid.generateRandomGridpointValues(),frameLength);
					break;

				default: break;
				}
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				mouseClick = 1;
				mousePosition = sf::Mouse::getPosition(window);
			}
		}

		window.clear();
		
		for (int i = 0; i<grid.getNumGridpoints(); i++)
			window.draw(grid.getGridpoint(i));

		window.display();

		if (!wait)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(frameLength));
			newGridpointValues = grid.generateNewGridpointValues();
		}
		else
			if (edit && mouseClick) newGridpointValues = grid.manualOverrideGridpointValues(mousePosition);

		grid.updateGrid(newGridpointValues, frameLength);
	}
}