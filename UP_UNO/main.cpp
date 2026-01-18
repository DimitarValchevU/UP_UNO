/**
*
* Solution to course project # 04
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2025/2026
*
* @author Dimitar Valchev
* @idnumber 5MI0600625
* @compiler MSVC
*
* main.cpp
*
*/

#include <iostream>
#include "game_logic.h"

int main()
{
	size_t numberOfPlayers = 0;
	char** playerNames = nullptr;

	//Game logic
	char drawPile[CARD_COUNT][MAX_CARD_LABEL_LENGTH + 1] = {
		// Red
		"R_0","R_1","R_2","R_3","R_4","R_5","R_6","R_7","R_8","R_9",
		"R_SKIP","R_REVERSE","R_+2",
		"R_1","R_2","R_3","R_4","R_5","R_6","R_7","R_8","R_9",
		"R_SKIP","R_REVERSE","R_+2",

		// Green
		"G_0","G_1","G_2","G_3","G_4","G_5","G_6","G_7","G_8","G_9",
		"G_SKIP","G_REVERSE","G_+2",
		"G_1","G_2","G_3","G_4","G_5","G_6","G_7","G_8","G_9",
		"G_SKIP","G_REVERSE","G_+2",

		// Yellow
		"Y_0","Y_1","Y_2","Y_3","Y_4","Y_5","Y_6","Y_7","Y_8","Y_9",
		"Y_SKIP","Y_REVERSE","Y_+2",
		"Y_1","Y_2","Y_3","Y_4","Y_5","Y_6","Y_7","Y_8","Y_9",
		"Y_SKIP","Y_REVERSE","Y_+2",

		// Blue
		"B_0","B_1","B_2","B_3","B_4","B_5","B_6","B_7","B_8","B_9",
		"B_SKIP","B_REVERSE","B_+2",
		"B_1","B_2","B_3","B_4","B_5","B_6","B_7","B_8","B_9",
		"B_SKIP","B_REVERSE","B_+2",

		// Wilds
		"W_WILD",
		"W_WILD",
		"W_WILD",
		"W_WILD",
		"W_WILD+4",
		"W_WILD+4",
		"W_WILD+4",
		"W_WILD+4"
	};
	size_t drawPileSize = CARD_COUNT;
	char discardPile[CARD_COUNT][MAX_CARD_LABEL_LENGTH + 1] = {};
	size_t discardPileSize = 0;

	char*** hands = nullptr;
	size_t* handSizes = nullptr;

	size_t currentPlayer = 0;
	bool direction = 1; //1 - ascending (clockwise), 0 - descending (counterclockwise)


	//Menu Choice
	std::cout << "--- UNO ---" << std::endl;
	std::cout << "1. New Game" << std::endl;
	std::cout << "2. Load Game" << std::endl;
	std::cout << "Enter your choice: ";

	size_t choice;
	std::cin >> choice;

	if (choice != 1 && choice != 2)
		return -1;

	if (choice == 2)
	{
		if (!loadGame("game.txt", numberOfPlayers, playerNames, drawPile, drawPileSize, discardPile, discardPileSize, hands, handSizes, direction, currentPlayer))
		{
			std::cout << "Failed to load save file. Starting new game instead..." << std::endl;
			choice = 1;
		}
		else
		{
			std::cout << "Game loaded successfully!" << std::endl;
		}
	}

	if (choice == 1)
	{
		enterPlayers(numberOfPlayers, playerNames);
		cardShuffleAndInitialDraw(numberOfPlayers, drawPile, drawPileSize, discardPile, discardPileSize, hands, handSizes);
	}

	clearOutput(std::cout);

	//Game loop
	while (true)
	{
		saveGame("game.txt", numberOfPlayers, playerNames, drawPile, drawPileSize, discardPile, discardPileSize, hands, handSizes, direction, currentPlayer);

		size_t result = executeGameLogic("game.txt", numberOfPlayers, playerNames, drawPile, discardPileSize, discardPile, discardPileSize, hands, handSizes, direction, currentPlayer);
		if (result == 1)
			continue;
		else if (result == 2)
			break;
	}

	//Memory cleanup
	cleanupMemory(numberOfPlayers, playerNames, hands, handSizes);

	return 0;
}
