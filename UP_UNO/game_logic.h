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
* game_logic.h - Game logic helper functions
*
*/

#ifndef GAME_LOGIC_H

#define GAME_LOGIC_H

#include<iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include "utils.h"

// Constants
const size_t BUFFER_SIZE = 256;
const size_t MIN_PLAYERS = 2;
const size_t MAX_PLAYERS = 4;
const size_t CARD_COUNT = 108;
const size_t MAX_CARD_LABEL_LENGTH = 9;
const size_t MAX_FORMATTED_CARD_LABEL_LENGTH = 18;
const size_t INITIAL_CARD_DRAW = 7;

// Helper functions
char* formatCard(char* dest, const char* card)
{
	if (card == nullptr)
	{
		dest[0] = '\0';
		return dest;
	}
	const char* start;
	const char* end = "\033[0m";
	switch (card[0])
	{
	case 'R':
	{
		start = "\033[31m";
		break;
	}
	case 'G':
	{
		start = "\033[32m";
		break;
	}
	case 'Y':
	{
		start = "\033[33m";
		break;
	}
	case 'B':
	{
		start = "\033[34m";
		break;
	}
	case 'W':
	{
		start = "\033[37m";
		break;
	}
	default:
	{
		dest[0] = '\0';
		return dest;
	}
	}

	size_t k = 0;
	while (*start != '\0')
		dest[k++] = *(start++);

	size_t cardLength = my_strlen(card);
	for (size_t i = 0; i < cardLength; i++)
	{
		dest[k++] = card[i];
	}

	while (*end != '\0')
		dest[k++] = *(end++);
	dest[k] = '\0';

	return dest;
}

bool isCardValid(const char* playedCard, const char* discardPileCard)
{
	if (playedCard == nullptr || discardPileCard == nullptr) return false;

	if (playedCard[0] == discardPileCard[0])
		return true;
	if (playedCard[0] == 'W')
		return true;

	char playedCardValue[MAX_CARD_LABEL_LENGTH + 1] = {};
	char discardPileCardValue[MAX_CARD_LABEL_LENGTH + 1] = {};
	my_strcpy(playedCardValue, playedCard + 2);
	my_strcpy(discardPileCardValue, discardPileCard + 2);

	bool invalid = my_strcmp(playedCardValue, discardPileCardValue);
	return !invalid;
}

void removeCardFromHand(size_t cardIndex, char* hand[MAX_CARD_LABEL_LENGTH + 1], size_t& handSize)
{
	for (size_t i = cardIndex; i < handSize - 1; i++)
	{
		my_strcpy(hand[i], hand[i + 1]);
	}
	handSize--;
}

void addCardToHand(char drawPile[][MAX_CARD_LABEL_LENGTH + 1], size_t& drawPileSize, char* hand[MAX_CARD_LABEL_LENGTH + 1], size_t& handSize,
	char discardPile[][MAX_CARD_LABEL_LENGTH + 1], size_t& discardPileSize, std::ostream* reshuffleSignalizationOutput = nullptr) //for reshuffle
{
	if (drawPileSize == 0)
	{
		char discardPileTop[MAX_CARD_LABEL_LENGTH + 1];
		my_strcpy(discardPileTop, discardPile[discardPileSize - 1]);
		for (size_t i = 0; i < discardPileSize - 1; i++)
		{
			char cardValue[MAX_CARD_LABEL_LENGTH + 1] = {};
			my_strcpy(cardValue, discardPile[i] + 2);

			if (!my_strcmp(cardValue, "WILD"))
				discardPile[i][0] = 'W';
			else if (!my_strcmp(cardValue, "WILD+4"))
				discardPile[i][0] = 'W';

			my_strcpy(drawPile[i], discardPile[i]);
		}
		my_strcpy(discardPile[0], discardPileTop);
		drawPileSize = discardPileSize - 1;
		discardPileSize = 1;

		std::random_device rndDevice = {};
		std::mt19937 mtGenerator(rndDevice());
		std::shuffle(drawPile, drawPile + drawPileSize, mtGenerator);

		if (reshuffleSignalizationOutput != nullptr)
			std::cout << newline << "In order to continue the game, the Discard Pile (except for the top card) has been reshuffled and is now the new Draw Pile!" << newline;
	}

	my_strcpy(hand[handSize], drawPile[drawPileSize - 1]);
	handSize++;
	drawPileSize--;
}

void addCardToDiscardPile(const char* card, char discardPile[][MAX_CARD_LABEL_LENGTH + 1], size_t& discardPileSize)
{
	my_strcpy(discardPile[discardPileSize], card);
	discardPileSize++;
}

void prepareNextPlayerIndex(size_t& currentPlayer, bool direction, size_t numberOfPlayers)
{
	currentPlayer = (direction) ? (currentPlayer + 1) % numberOfPlayers
		: (currentPlayer == 0) ? numberOfPlayers - 1
		: currentPlayer - 1;
}

// Game logic functions
void enterPlayers(size_t& numberOfPlayers, char**& playerNames)
{
	// Entering the number of players:
	std::cout << "To start a new game, enter the number of players: ";
	std::cin >> numberOfPlayers;
	while (inputFailed(std::cin) || numberOfPlayers < MIN_PLAYERS || numberOfPlayers > MAX_PLAYERS)
	{
		clearOutput(std::cout);
		std::cout << '\r' << "Invalid input!" << newline;
		std::cout << "To start a new game, enter the number of players: ";
		std::cin >> numberOfPlayers;
	}
	clearOutput(std::cout);

	playerNames = new char* [numberOfPlayers];

	// Entering the names of players:
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		char buffer[BUFFER_SIZE] = {};
		char trimmedBuffer[BUFFER_SIZE] = {};

		std::cout << "Enter the name of Player " << i + 1 << ": ";
		std::cin >> std::ws;
		std::cin.getline(buffer, BUFFER_SIZE);
		while (inputFailed(std::cin) || !my_strlen(my_trim(trimmedBuffer, buffer)))
		{
			clearOutput(std::cout);
			std::cout << '\r' << "Invalid input!" << newline;
			std::cout << "Enter the name of Player " << i + 1 << ": ";
			std::cin >> std::ws;
			std::cin.getline(buffer, BUFFER_SIZE);
		}

		size_t nameLength = my_strlen(trimmedBuffer);
		playerNames[i] = new char[nameLength + 1];
		my_strcpy(playerNames[i], trimmedBuffer);
	}
}

void cardShuffleAndInitialDraw(size_t numberOfPlayers, char drawPile[][MAX_CARD_LABEL_LENGTH + 1], size_t& drawPileSize,
	char discardPile[][MAX_CARD_LABEL_LENGTH + 1], size_t& discardPileSize,
	char***& hands, size_t*& handSizes)
{
	std::random_device rndDevice = {};
	std::mt19937 mtGenerator(rndDevice());
	std::shuffle(drawPile, drawPile + drawPileSize, mtGenerator);

	hands = new char** [numberOfPlayers];
	handSizes = new size_t[numberOfPlayers];

	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		hands[i] = new char* [CARD_COUNT];
		for (size_t j = 0; j < CARD_COUNT; j++)
		{
			hands[i][j] = new char[MAX_CARD_LABEL_LENGTH + 1];
		}
		for (size_t j = 0; j < INITIAL_CARD_DRAW; j++)
		{
			my_strcpy(hands[i][j], drawPile[--drawPileSize]);
		}
		handSizes[i] = INITIAL_CARD_DRAW;
	}

	my_strcpy(discardPile[discardPileSize], drawPile[drawPileSize - 1]);
	discardPileSize++;
	drawPileSize--;
}

void cleanupMemory(size_t numberOfPlayers, char**& playerNames, char***& hands, size_t*& handSizes)
{
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		delete[] playerNames[i];
		playerNames[i] = nullptr;
	}
	delete[] playerNames;
	playerNames = nullptr;

	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		for (size_t j = 0; j < CARD_COUNT; j++)
		{
			delete[] hands[i][j];
			hands[i][j] = nullptr;
		}
		delete[] hands[i];
		hands[i] = nullptr;
	}
	delete[] hands;
	hands = nullptr;
	delete[] handSizes;
	handSizes = nullptr;
}

size_t executeGameLogic(const char* saveFilename, size_t& numberOfPlayers, char** playerNames,
	char drawPile[][MAX_CARD_LABEL_LENGTH + 1], size_t& drawPileSize, char discardPile[][MAX_CARD_LABEL_LENGTH + 1], size_t& discardPileSize, char*** hands, size_t* handSizes,
	bool& direction, size_t& currentPlayer)
{
	clearOutput(std::cout);
	std::cout << "--- UNO ---" << newline;
	if (discardPileSize <= 0) return 2;

	const char* discardPileCard = discardPile[discardPileSize - 1];
	{
		char formattedCard[MAX_FORMATTED_CARD_LABEL_LENGTH + 1] = {};
		std::cout << "Current Discard Pile card: " << formatCard(formattedCard, discardPileCard) << newline;
	}

	std::cout << playerNames[currentPlayer] << " (Player " << currentPlayer + 1 << ") is in turn! Their cards are: " << newline;
	bool hasValidCard = false;
	for (size_t i = 0; i < handSizes[currentPlayer]; i++)
	{
		char formattedCard[MAX_FORMATTED_CARD_LABEL_LENGTH + 1] = {};
		hasValidCard = hasValidCard || isCardValid(hands[currentPlayer][i], discardPileCard);
		std::cout << '[' << i + 1 << "] " << formatCard(formattedCard, hands[currentPlayer][i]) << "  ";
	}
	std::cout << newline;
	if (!hasValidCard)
	{
		std::cout << "No valid cards... Automatically drawing card from the Draw Pile: ";
		addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
		char formattedCard[MAX_FORMATTED_CARD_LABEL_LENGTH + 1] = {};
		std::cout << '[' << handSizes[currentPlayer] << "] " << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << newline;

		if (!isCardValid(hands[currentPlayer][handSizes[currentPlayer] - 1], discardPileCard))
		{
			std::cout << "Still no valid cards... Press Enter to skip!" << newline;
			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);
			std::cin.get();

			return 1;
		}
	}

	size_t cardToPlay = 0;
	std::cin >> cardToPlay;
	cardToPlay--;
	if (inputFailed(std::cin))
	{
		clearOutput(std::cout);
		std::cout << '\r' << "Invalid input!" << newline << "Press Enter to try again!" << newline;
		std::cin.get();
		clearOutput(std::cout);
		return 1;
	}
	if (cardToPlay >= handSizes[currentPlayer])
	{
		clearOutput(std::cout);
		std::cout << '\r' << "Invalid input!" << newline << "Press Enter to try again!" << newline;
		std::cin.ignore();
		std::cin.get();
		clearOutput(std::cout);
		return 1;
	}

	const char* playedCard = hands[currentPlayer][cardToPlay];
	if (!isCardValid(playedCard, discardPileCard))
	{
		clearOutput(std::cout);
		std::cout << '\r' << "Invalid card played!" << newline << "Press Enter key to try again!" << newline;
		std::cin.ignore();
		std::cin.get();
		clearOutput(std::cout);
		return 1;
	}

	{
		char formattedCard[MAX_FORMATTED_CARD_LABEL_LENGTH + 1] = {};
		std::cout << "You played: " << formatCard(formattedCard, playedCard) << newline;
	}

	char playedCardValue[MAX_CARD_LABEL_LENGTH + 1] = {};
	my_strcpy(playedCardValue, playedCard + 2);


	addCardToDiscardPile(playedCard, discardPile, discardPileSize);
	removeCardFromHand(cardToPlay, hands[currentPlayer], handSizes[currentPlayer]);
	bool unoCheck = false;
	size_t currentPlayerBackup = currentPlayer;
	if (handSizes[currentPlayer] == 1)
		unoCheck = true;
	else if (handSizes[currentPlayer] == 0)
	{
		// Game loop end
		clearOutput(std::cout);
		std::cout << playerNames[currentPlayer] << " has won the game!" << newline;
		return 2;
	}

	// Apply card effects
	if (!my_strcmp(playedCardValue, "REVERSE"))
	{
		direction = !direction;
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		if (!unoCheck)
		{
			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
			return 1;
		}
	}
	else if (!my_strcmp(playedCardValue, "SKIP"))
	{
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		if (!unoCheck)
		{
			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
			return 1;
		}
	}
	else if (!my_strcmp(playedCardValue, "+2"))
	{
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		char formattedCard[MAX_FORMATTED_CARD_LABEL_LENGTH + 1] = {};
		addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
		addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << " drawed from the next player!" << newline;

		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		if (!unoCheck)
		{
			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
			return 1;
		}
	}
	else if (!my_strcmp(playedCardValue, "WILD"))
	{
		char buffer[BUFFER_SIZE] = {};
		char trimmedBuffer[BUFFER_SIZE] = {};
		std::cout << "Choose a color code: [R] Red  [G] Green  [Y] Yellow  [B] Blue" << newline;
		std::cin >> std::ws;
		std::cin.getline(buffer, BUFFER_SIZE);
		while (inputFailed(std::cin) || my_strlen(my_trim(trimmedBuffer, buffer)) != 1 ||
			(my_strcmp(trimmedBuffer, "R") && my_strcmp(trimmedBuffer, "G") && my_strcmp(trimmedBuffer, "Y") && my_strcmp(trimmedBuffer, "B")))
		{
			clearOutput(std::cout);
			std::cout << '\r' << "Invalid input!" << newline;
			std::cout << "Choose a color code: [R] Red  [G] Green  [Y] Yellow  [B] Blue" << newline;
			std::cin >> std::ws;
			std::cin.getline(buffer, BUFFER_SIZE);
		}

		discardPile[discardPileSize - 1][0] = trimmedBuffer[0];
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		if (!unoCheck)
		{
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
			return 1;
		}
	}
	else if (!my_strcmp(playedCardValue, "WILD+4"))
	{
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		char formattedCard[MAX_FORMATTED_CARD_LABEL_LENGTH + 1] = {};
		addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
		addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
		addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
		addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << " drawed from the next player!" << newline;

		char buffer[BUFFER_SIZE] = {};
		char trimmedBuffer[BUFFER_SIZE] = {};
		std::cout << "Choose a color code: [R] Red  [G] Green  [Y] Yellow  [B] Blue" << newline;
		std::cin >> std::ws;
		std::cin.getline(buffer, BUFFER_SIZE);
		while (inputFailed(std::cin) || my_strlen(my_trim(trimmedBuffer, buffer)) != 1 ||
			(my_strcmp(trimmedBuffer, "R") && my_strcmp(trimmedBuffer, "G") && my_strcmp(trimmedBuffer, "Y") && my_strcmp(trimmedBuffer, "B")))
		{
			clearOutput(std::cout);
			std::cout << '\r' << "Invalid input!" << newline;
			std::cout << "Choose a color code: [R] Red  [G] Green  [Y] Yellow  [B] Blue" << newline;
			std::cin >> std::ws;
			std::cin.getline(buffer, BUFFER_SIZE);
		}

		discardPile[discardPileSize - 1][0] = trimmedBuffer[0];
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		if (!unoCheck)
		{
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
			return 1;
		}
	}
	else
	{
		prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

		if (!unoCheck)
		{
			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
			return 1;
		}
	}


	// UNO! check
	char buffer[BUFFER_SIZE] = {};
	char trimmedBuffer[BUFFER_SIZE] = {};

	std::cin >> std::ws;
	std::cin.getline(buffer, BUFFER_SIZE);
	if (inputFailed(std::cin) || !my_strlen(my_trim(trimmedBuffer, buffer)))
	{
		clearOutput(std::cout);
		std::cout << '\r' << "Invalid input!" << newline;
		return 1;
	}
	if (my_strcmp(trimmedBuffer, "UNO!"))
	{
		std::cout << "You forgot to say \"UNO!\"...";
		char formattedCard[MAX_FORMATTED_CARD_LABEL_LENGTH + 1] = {};
		addCardToHand(drawPile, drawPileSize, hands[currentPlayerBackup], handSizes[currentPlayerBackup], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayerBackup][handSizes[currentPlayerBackup] - 1]) << ", ";
		addCardToHand(drawPile, drawPileSize, hands[currentPlayerBackup], handSizes[currentPlayerBackup], discardPile, discardPileSize, &std::cout);
		std::cout << formatCard(formattedCard, hands[currentPlayerBackup][handSizes[currentPlayerBackup] - 1]) << " drawed from you!" << newline;
	}

	std::cout << "Press Enter to continue!" << newline;
	std::cin.get();

	return 1;
}

// Game state save and load functions
bool saveGame(const char* filename, size_t numberOfPlayers, char** playerNames,
	char drawPile[][MAX_CARD_LABEL_LENGTH + 1], size_t drawPileSize, char discardPile[][MAX_CARD_LABEL_LENGTH + 1], size_t discardPileSize, char*** hands, size_t* handSizes,
	bool direction, size_t currentPlayer)
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	file << "NUMBER_OF_PLAYERS" << newline;
	file << numberOfPlayers << newline;
	file << "PLAYER_NAMES" << newline;
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		file << playerNames[i] << newline;
	}
	file << newline;

	file << "DRAW_PILE_SIZE" << newline;
	file << drawPileSize << newline;
	file << "DRAW_PILE" << newline;
	for (size_t i = 0; i < drawPileSize; i++)
	{
		file << drawPile[i] << newline;
	}
	file << newline;

	file << "DISCARD_PILE_SIZE" << newline;
	file << discardPileSize << newline;
	file << "DISCARD_PILE" << newline;
	for (size_t i = 0; i < discardPileSize; i++)
	{
		file << discardPile[i] << newline;
	}
	file << newline;

	file << "HANDS_SIZE" << newline;
	file << numberOfPlayers << newline;
	file << "HANDS" << newline;
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		file << "HAND_SIZE" << newline;
		file << handSizes[i] << newline;
		file << "HAND_" << i << newline;
		for (size_t j = 0; j < handSizes[i]; j++)
		{
			file << hands[i][j] << newline;
		}
	}
	file << newline;

	file << "DIRECTION" << newline;
	file << direction << newline;
	file << newline;

	file << "CURRENT_PLAYER" << newline;
	file << currentPlayer << newline;

	file.close();
	return true;
}

bool loadGame(const char* filename,
	size_t& numberOfPlayers,
	char**& playerNames,
	char drawPile[][MAX_CARD_LABEL_LENGTH + 1],
	size_t& drawPileSize,
	char discardPile[][MAX_CARD_LABEL_LENGTH + 1],
	size_t& discardPileSize,
	char***& hands,
	size_t*& handSizes,
	bool& direction,
	size_t& currentPlayer)
{
	std::ifstream file(filename);
	if (!file.is_open() || file.tellg() == 0) // Empty file...
		return false;

	char buffer[BUFFER_SIZE] = {};
	char trimmed[BUFFER_SIZE] = {};

	file.getline(buffer, BUFFER_SIZE);
	file.getline(buffer, BUFFER_SIZE);
	size_t numPlayersFromFile = (size_t)(my_atoi(my_trim(trimmed, buffer)));

	if (playerNames != nullptr && hands != nullptr)
	{
		cleanupMemory(numberOfPlayers, playerNames, hands, handSizes);
	}

	numberOfPlayers = numPlayersFromFile;

	file.getline(buffer, BUFFER_SIZE);
	playerNames = new char* [numberOfPlayers];
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		file.getline(buffer, BUFFER_SIZE);
		my_trim(trimmed, buffer);
		size_t len = my_strlen(trimmed);
		playerNames[i] = new char[len + 1];
		my_strcpy(playerNames[i], trimmed);
	}
	file.getline(buffer, BUFFER_SIZE);

	file.getline(buffer, BUFFER_SIZE);
	file.getline(buffer, BUFFER_SIZE);
	drawPileSize = (size_t)(my_atoi(my_trim(trimmed, buffer)));

	file.getline(buffer, BUFFER_SIZE);
	for (size_t i = 0; i < drawPileSize; i++)
	{
		file.getline(buffer, BUFFER_SIZE);
		my_strcpy(drawPile[i], my_trim(trimmed, buffer));
	}
	file.getline(buffer, BUFFER_SIZE);

	file.getline(buffer, BUFFER_SIZE);
	file.getline(buffer, BUFFER_SIZE);
	discardPileSize = (size_t)(my_atoi(my_trim(trimmed, buffer)));

	file.getline(buffer, BUFFER_SIZE);
	for (size_t i = 0; i < discardPileSize; i++)
	{
		file.getline(buffer, BUFFER_SIZE);
		my_strcpy(discardPile[i], my_trim(trimmed, buffer));
	}
	file.getline(buffer, BUFFER_SIZE);

	file.getline(buffer, BUFFER_SIZE);
	file.getline(buffer, BUFFER_SIZE);

	hands = new char** [numberOfPlayers];
	handSizes = new size_t[numberOfPlayers];

	file.getline(buffer, BUFFER_SIZE);
	for (size_t p = 0; p < numberOfPlayers; p++)
	{
		file.getline(buffer, BUFFER_SIZE);
		file.getline(buffer, BUFFER_SIZE);
		handSizes[p] = (size_t)(my_atoi(my_trim(trimmed, buffer)));

		file.getline(buffer, BUFFER_SIZE);
		hands[p] = new char* [CARD_COUNT];
		for (size_t j = 0; j < CARD_COUNT; j++)
			hands[p][j] = new char[MAX_CARD_LABEL_LENGTH + 1];

		for (size_t c = 0; c < handSizes[p]; c++)
		{
			file.getline(buffer, BUFFER_SIZE);
			my_strcpy(hands[p][c], my_trim(trimmed, buffer));
		}
		for (size_t c = handSizes[p]; c < CARD_COUNT; c++)
			hands[p][c][0] = '\0';
	}
	file.getline(buffer, BUFFER_SIZE);

	file.getline(buffer, BUFFER_SIZE);
	file.getline(buffer, BUFFER_SIZE);
	my_trim(trimmed, buffer);
	direction = (trimmed[0] == '1');
	file.getline(buffer, BUFFER_SIZE);

	file.getline(buffer, BUFFER_SIZE);
	file.getline(buffer, BUFFER_SIZE);
	currentPlayer = (size_t)(my_atoi(my_trim(trimmed, buffer)));

	file.close();
	return true;
}

bool clearGame(const char* filename)
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	file.close();
	return true;
}

#endif // !GAME_LOGIC_H