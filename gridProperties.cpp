#include "definitions.h"

GridProperties::GridProperties(sf::Vector2i size, std::vector<sf::Color> colors, int defaultColor, int edgeType)
{
	this->size = size;

	findPixelsPerGridpoint();
	findNumGridpoints();
	this->gridpoints.resize(this->numGridpoints); //might as well resize here as anywhere else.
	this->gridpointValues.resize(this->numGridpoints);
	this->gridpointNumNeighbours.resize(this->numGridpoints);

	this->colors = colors;
	this->defaultColor = defaultColor;
	this->edgeType = edgeType;

	//set gridpointValues so consistent with defaultColor - necessary for how grid update works
	for (int i = 0; i < numGridpoints; i++)
	{
		this->gridpointValues[i] = this->defaultColor;
		if (this->defaultColor == 0) //default dead
			this->gridpointNumNeighbours[i] = 0;
		else //default alive
			this->gridpointNumNeighbours[i] = 8;
	}
}

void GridProperties::initialiseGrid()
{
	for (int i = 0; i < this->numGridpoints; i++)
	{
		gridpoints[i].setSize(this->pixelsPerGridpoint);

		gridpoints[i].setFillColor(this->colors[this->defaultColor]);

		sf::Vector2f gridCoord = coordFromIndex(i);

		gridpoints[i].setPosition(gridCoordToPixelCoord(gridCoord));
	}
}

void GridProperties::updateGrid(std::vector<int> newGridpointValues, int frameLength)
{
	if (sizeof(this->gridpointValues) != sizeof(newGridpointValues))
	{
		std::cout << "ERROR: updating grid: new grid values not compatible with grid size\n";
		system("pause");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < this->numGridpoints; i++)
	{
		if (this->gridpointValues[i] != newGridpointValues[i])
		{
			if (newGridpointValues[i] >= this->colors.size())
			{
				std::cout << "ERROR: updating grid: new grid value (" << newGridpointValues[i] << ") invalid color\n";
				system("pause");
				exit(EXIT_FAILURE);
			}

			this->gridpoints[i].setFillColor(this->colors[newGridpointValues[i]]);
		}
	}

	this->gridpointValues = newGridpointValues;

	this->findGridpointNumNeighbours();

}

void GridProperties::findPixelsPerGridpoint()
{

	int x = floor(SCREENWIDTH / this->size.x);
	int y = floor(SCREENHEIGHT / this->size.y);

	pixelsPerGridpoint.x = (float)x;
	pixelsPerGridpoint.y = (float)y;
}

sf::Vector2f GridProperties::coordFromIndex(int index)
{
	int x = index%this->size.x;
	int y = floor(index / this->size.y);

	sf::Vector2f coord((float)x, (float)y);

	return(coord);
}
//Given a 2-space coord returns index. 	// going off edges needs to be dealt with when this is used
int GridProperties::indexFromCoord(sf::Vector2f coord)
{
	int index = coord.y*this->size.y + coord.x;

	return(index);
}
//Given 2-space coord returns index.  Wraps at edges.
int GridProperties::indexFromCoordWrap(sf::Vector2f coord)
{
	int x, y;

	if (coord.x < 0)
		x = coord.x + size.x;
	else if (coord.x >= size.x)
		x = coord.x - size.x;
	else
		x = coord.x;

	if (coord.y < 0)
		y = coord.y + size.y;
	else if (coord.y >= size.y)
		y = coord.y - size.y;
	else
		y = coord.y;

	int index = indexFromCoord(sf::Vector2f(x, y));

	return(index);
}

void GridProperties::findNumGridpoints()
{
	numGridpoints = this->size.x*this->size.y;
}

sf::Vector2f GridProperties::gridCoordToPixelCoord(sf::Vector2f gridCoord)
{
	int x = gridCoord.x*this->pixelsPerGridpoint.x;
	int y = gridCoord.y*this->pixelsPerGridpoint.y;

	return(sf::Vector2f(x, y));
}
sf::Vector2f GridProperties::pixelCoordToGridCoord(sf::Vector2f pixelCoord)
{
	int x = round(pixelCoord.x / this->pixelsPerGridpoint.x);
	int y = round(pixelCoord.y / this->pixelsPerGridpoint.y);

	return(sf::Vector2f(x, y));
}

sf::Vector2i GridProperties::getSize()
{
	return size;
}
sf::RectangleShape GridProperties::getGridpoint(int i)
{
	return gridpoints[i];
}
sf::Vector2f GridProperties::getPixelsPerGridpoint()
{
	return pixelsPerGridpoint;
}
int GridProperties::getNumGridpoints()
{
	return numGridpoints;
}
int GridProperties::getEdgeType()
{
	return edgeType;
}
int GridProperties::getGridpointValue(int i)
{
	return gridpointValues[i];
}
std::vector<int> GridProperties::getGridpointValues()
{
	return gridpointValues;
}
int GridProperties::getGridpointNumNeighbours(int i)
{
	return gridpointNumNeighbours[i];
}

int GridProperties::getNumColors()
{
	return colors.size();
}

void GridProperties::findGridpointNumNeighbours()
{
	switch (edgeType)
	{
	case 0: //edges dead
		edgesDead();
		break;
	case 1: //live edge
		edgesAlive();
		break;
	case 2: //wrap edge
		edgesWrap();
		break;
	default:
		std::cout << "NEED TO DEFINE AN EDGE TYPE";
		system("pause");
		exit(EXIT_FAILURE);
	}
}

void GridProperties::edgesDead()
{
	for (int i = 0; i < numGridpoints; i++)
	{
		gridpointNumNeighbours[i] = 0; //start with all surrounding cells dead, add as appropriate

									   //look at in terms of coordinates, rather than indices

		if ((coordFromIndex(i).x - 1) > 0)
		{
			if ((coordFromIndex(i).y - 1) > 0)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x - 1, coordFromIndex(i).y - 1))) == 1)
					gridpointNumNeighbours[i] += 1;
			}
			if ((coordFromIndex(i).y + 1) < size.y)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x - 1, coordFromIndex(i).y + 1))) == 1)
					gridpointNumNeighbours[i] += 1;
			}
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x - 1, coordFromIndex(i).y))) == 1)
				gridpointNumNeighbours[i] += 1;
		}

		if ((coordFromIndex(i).x + 1) < size.x)
		{
			if ((coordFromIndex(i).y - 1) > 0)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x + 1, coordFromIndex(i).y - 1))) == 1)
					gridpointNumNeighbours[i] += 1;
			}
			if ((coordFromIndex(i).y + 1) < size.y)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x + 1, coordFromIndex(i).y + 1))) == 1)
					gridpointNumNeighbours[i] += 1;
			}
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x + 1, coordFromIndex(i).y))) == 1)
				gridpointNumNeighbours[i] += 1;
		}

		if ((coordFromIndex(i).y - 1) > 0)
		{
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x, coordFromIndex(i).y - 1))) == 1)
				gridpointNumNeighbours[i] += 1;
		}
		if ((coordFromIndex(i).y + 1) < size.y)
		{
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x, coordFromIndex(i).y + 1))) == 1)
				gridpointNumNeighbours[i] += 1;
		}
	}
}

void GridProperties::edgesAlive()
{
	for (int i = 0; i < numGridpoints; i++)
	{
		gridpointNumNeighbours[i] = 8; //start with all surrounding cells alive, subtract as appropriate

									   //look at in terms of coordinates, rather than indices

		if ((coordFromIndex(i).x - 1) > 0)
		{
			if ((coordFromIndex(i).y - 1) > 0)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x - 1, coordFromIndex(i).y - 1))) == 0)
					gridpointNumNeighbours[i] -= 1;
			}
			if ((coordFromIndex(i).y + 1) < size.y)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x - 1, coordFromIndex(i).y + 1))) == 0)
					gridpointNumNeighbours[i] -= 1;
			}
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x - 1, coordFromIndex(i).y))) == 0)
				gridpointNumNeighbours[i] -= 1;
		}

		if ((coordFromIndex(i).x + 1) < size.x)
		{
			if ((coordFromIndex(i).y - 1) > 0)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x + 1, coordFromIndex(i).y - 1))) == 0)
					gridpointNumNeighbours[i] -= 1;
			}
			if ((coordFromIndex(i).y + 1) < size.y)
			{
				if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x + 1, coordFromIndex(i).y + 1))) == 0)
					gridpointNumNeighbours[i] -= 1;
			}
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x + 1, coordFromIndex(i).y))) == 0)
				gridpointNumNeighbours[i] -= 1;
		}

		if ((coordFromIndex(i).y - 1) > 0)
		{
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x, coordFromIndex(i).y - 1))) == 0)
				gridpointNumNeighbours[i] -= 1;
		}
		if ((coordFromIndex(i).y + 1) < size.y)
		{
			if (getGridpointValue(indexFromCoord(sf::Vector2f(coordFromIndex(i).x, coordFromIndex(i).y + 1))) == 0)
				gridpointNumNeighbours[i] -= 1;
		}
	}
}

void GridProperties::edgesWrap()
{
	for (int i = 0; i < numGridpoints; i++)
	{
		gridpointNumNeighbours[i] = 0;
		for (int x = coordFromIndex(i).x - 1; x <= coordFromIndex(i).x + 1; x++)
		{
			for (int y = coordFromIndex(i).y - 1; y <= coordFromIndex(i).y + 1; y++)
			{
				if (getGridpointValue(indexFromCoordWrap(sf::Vector2f(x, y))) == 1)
					gridpointNumNeighbours[i] += 1;
			}
		}
	}
}

//updating grid values

sf::Vector2i GridProperties::findWindowSize()
{
	float windowSizeX, windowSizeY;

	windowSizeX = this->getPixelsPerGridpoint().x*this->getSize().x;
	windowSizeY = this->getPixelsPerGridpoint().y*this->getSize().y;

	sf::Vector2i windowSize((int)windowSizeX, (int)windowSizeY);

	return(windowSize);
}

std::vector<int> GridProperties::generateRandomGridpointValues()
{
	std::vector<int> newGridpointValues(this->getNumGridpoints());
	srand(time(NULL));

	for (int i = 0; i < this->getNumGridpoints(); i++)
	{
		double randomNum = (double)rand() / RAND_MAX;
		newGridpointValues[i] = round(randomNum);
		if (newGridpointValues[i] == 0 || newGridpointValues[i] == 1)
		{
		}
		else
		{
			std::cout << "ERROR: randomly generated gridpoint has invalid value\n";
			system("pause");
			exit(EXIT_FAILURE);
		}
	}

	return newGridpointValues;
}

std::vector<int> GridProperties::generateNewGridpointValues()
{
	std::vector<int> newGridpointValues(this->getNumGridpoints());

	newGridpointValues = this->applyGameOfLifeRules();

	return newGridpointValues;
}

std::vector<int> GridProperties::manualOverrideGridpointValues(sf::Vector2i mousePosition)
{
	std::vector<int> newGridpointValues(getNumGridpoints());

	newGridpointValues = getGridpointValues();

	sf::Vector2f gridPosition = pixelCoordToGridCoord(sf::Vector2f((float)mousePosition.x, (float)mousePosition.y));

	std::cout << "gridPosition:		" << gridPosition.x << ", " << gridPosition.y << "\n";

	if (newGridpointValues[indexFromCoord(gridPosition)] == 0)
		newGridpointValues[indexFromCoord(gridPosition)] = 1;
	else if (newGridpointValues[indexFromCoord(gridPosition)] == 1)
		newGridpointValues[indexFromCoord(gridPosition)] = 0;

	return newGridpointValues;
}

std::vector<int> GridProperties::applyGameOfLifeRules()
{
	int numGridpoints = this->getNumGridpoints();

	std::vector<int> newGridpointValues(numGridpoints);

	if (this->getNumColors() == 2) //standard game of life rules
	{
		for (int i = 0; i < numGridpoints; i++)
		{
			if (this->getGridpointValue(i) == 1) //currently alive
			{
				switch (this->getGridpointNumNeighbours(i))
				{
				case 0:
				case 1:
					newGridpointValues[i] = 0; //dies due to underpopulation
					break;
				case 2:
				case 3:
					newGridpointValues[i] = 1; //stays alive
					break;
				default:
					newGridpointValues[i] = 0; //dies due to overpopulation
					break;
				}
			}
			else //currently dead
			{
				switch (this->getGridpointNumNeighbours(i))
				{
				case 3:
					newGridpointValues[i] = 1; //born
					break;
				default:
					newGridpointValues[i] = 0; //no change
					break;
				}
			}
		}
	}
	else if (this->getNumColors() == 3)
	{
		std::cout << "need to write this bit";
		system("pause");
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "invalid number of colours: should have either 2 or 3";
		system("pause");
		exit(EXIT_FAILURE);
	}
	return newGridpointValues;
}
