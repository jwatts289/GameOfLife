#pragma once
#include "definitions.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#define SCREENWIDTH 800 //change this so automatically finds size of screen?
#define SCREENHEIGHT 800

class GridProperties
{
	sf::Vector2i size;
	std::vector<sf::Color> colors;
	int defaultColor;
	int edgeType; // 0: dead, 1: alive, 2: wraps

	std::vector<sf::RectangleShape> gridpoints;
	std::vector<int> gridpointValues;
	std::vector<int> gridpointNumNeighbours;

	sf::Vector2f pixelsPerGridpoint;
	int numGridpoints;

public:
	GridProperties() = delete;
	GridProperties(sf::Vector2i size, std::vector<sf::Color> colors, int defaultColor, int edgeType);

	//changing grid values
	void initialiseGrid();
	void updateGrid(std::vector<int> newGridpointValues, int frameLength);
	sf::Vector2i GridProperties::findWindowSize();
	std::vector<int> generateRandomGridpointValues();
	std::vector<int> generateNewGridpointValues();
	std::vector<int> manualOverrideGridpointValues(sf::Vector2i mousePosition);
	std::vector<int> applyGameOfLifeRules();

	sf::Vector2i getSize();
	sf::Vector2f getPixelsPerGridpoint();
	int getNumGridpoints();
	int getEdgeType();
	sf::RectangleShape getGridpoint(int i);
	int getGridpointValue(int i);
	std::vector<int> getGridpointValues();
	int getGridpointNumNeighbours(int i);
	int getNumColors();

private:
	void findPixelsPerGridpoint();
	void findNumGridpoints();
	void findGridpointNumNeighbours();
		void edgesDead();
		void edgesAlive();
		void edgesWrap();

	sf::Vector2f gridCoordToPixelCoord(sf::Vector2f gridCoord);
	sf::Vector2f pixelCoordToGridCoord(sf::Vector2f pixelCoord);
	sf::Vector2f coordFromIndex(int index);
	int indexFromCoord(sf::Vector2f coord);
	int indexFromCoordWrap(sf::Vector2f coord);
};