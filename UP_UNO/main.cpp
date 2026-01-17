#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>

const char* newline = "\r\n";

// Constants
const size_t BUFFER_SIZE = 256;
const size_t MIN_PLAYERS = 2;
const size_t MAX_PLAYERS = 4;
const size_t CARD_COUNT = 108;
const size_t MAX_CARD_LABEL_LENGTH = 9;
const size_t INITIAL_CARD_DRAW = 7;

// ANSI Escape codes
// Red std::cout << "\033[31mR\033[0m";
// Green std::cout << "\033[32mG\033[0m";
// Yellow std::cout << "\033[33mY\033[0m";
// Blue std::cout << "\033[34mB\033[0m";
// Clear std::cout << "\033[2J\033[H";

size_t my_strlen(const char* str)
{
	if (str == nullptr) return 0;

	size_t length = 0;
	while (str[length++] != '\0') {}

	return length - 1;
}

char* my_strcpy(char* dest, const char* src)
{
	if (dest == nullptr || src == nullptr) return nullptr;

	size_t i = 0;
	while (src[i++] != '\0')
	{
		dest[i - 1] = src[i - 1];
	}
	dest[i - 1] = '\0';

	return dest;
}

char* my_trim(char* dest, const char* src)
{
	if (dest == nullptr || src == nullptr) return nullptr;

	size_t start = 0;
	while (
		src[start] == ' ' ||
		src[start] == '\t' ||
		src[start] == '\n' ||
		src[start] == '\r' ||
		src[start] == '\v' ||
		src[start] == '\f'
		) {
		start++;
	}

	size_t end = my_strlen(src) - 1;
	if (end == -1)
		return nullptr;

	while (
		src[end] == ' ' ||
		src[end] == '\t' ||
		src[end] == '\n' ||
		src[end] == '\r' ||
		src[end] == '\v' ||
		src[end] == '\f'
		) {
		end--;
	}

	size_t k = 0;
	for (size_t i = start; i <= end; i++)
		dest[k++] = src[i];
	dest[k] = '\0';

	return dest;
}

bool inputFailed(std::istream& input)
{
	if (input.fail()) {
		input.clear();

		char c;
		while (input.get(c) && c != '\n') {}

		return true;
	}
	return false;
}

void clearOutput(std::ostream& output)
{
	output << "\033[2J\033[H";
}

void printCard(std::ostream& output, const char* card) {
	if (card == nullptr) return;
	switch (card[0])
	{
	case 'R':
	{
		output << "\033[31m" << card << "\033[0m";
		break;
	}
	case 'G':
	{
		output << "\033[32m" << card << "\033[0m";
		break;
	}
	case 'Y':
	{
		output << "\033[33m" << card << "\033[0m";
		break;
	}
	case 'B':
	{
		output << "\033[34m" << card << "\033[0m";
		break;
	}
	case 'W':
	{
		output << "\033[37m" << card << "\033[0m";
		break;
	}
	default:
	{
		break;
	}
	}
}



int main()
{
	std::cout << "Hello World!" << newline;

	size_t numberOfPlayers = 0;
	std::cout << "To start a new game, enter the number of players: ";
	std::cin >> numberOfPlayers;
	while (inputFailed(std::cin) || numberOfPlayers < MIN_PLAYERS || numberOfPlayers > MAX_PLAYERS)
	{
		clearOutput(std::cout);
		std::cout << '\r' << "Invalid input!" << newline;
		std::cout << "To start a new game, enter the number of players: ";
		std::cin >> numberOfPlayers;
	}

	char** playerNames = new char* [numberOfPlayers];
	for (size_t i = 0; i < numberOfPlayers; i++) {
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
	clearOutput(std::cout);

	std::cout << "Players: ";
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		size_t j = 0;
		while (playerNames[i][j++] != '\0') std::cout << playerNames[i][j - 1];
		if (i < numberOfPlayers - 1) std::cout << ", ";
	}
	std::cout << newline;

	/*const*/ char drawPile[CARD_COUNT][MAX_CARD_LABEL_LENGTH + 1] = {
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
	size_t drawPileLength = CARD_COUNT;

	std::random_device rndDevice = {};
	std::mt19937 mtGenerator(rndDevice());
	std::shuffle(drawPile, drawPile + drawPileLength - 1, mtGenerator);

	char*** hands = new char**[numberOfPlayers];
	size_t* handSizes = new size_t[numberOfPlayers];
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		hands[i] = new char* [CARD_COUNT];
		for (size_t j = 0; j < CARD_COUNT; j++)
		{
			hands[i][j] = new char[MAX_CARD_LABEL_LENGTH + 1];
		}
		for (size_t j = 0; j < INITIAL_CARD_DRAW; j++)
		{
			my_strcpy(hands[i][j], drawPile[--drawPileLength]);
		}
		handSizes[i] = INITIAL_CARD_DRAW;
	}

	std::cout << "Cards" << newline;
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		std::cout << playerNames[i] << ": ";
		for (size_t j = 0; j < handSizes[i]; j++)
		{
			//std::cout << hands[i][j] << ' ';
			printCard(std::cout, hands[i][j]);
			std::cout << ' ';
		}
		std::cout << newline;
	}

	////////////////////////////////////////////
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		delete[] playerNames[i];
	}
	delete[] playerNames;

	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		for (size_t j = 0; j < CARD_COUNT; j++)
		{
			delete[] hands[i][j];
		}
		delete[] hands[i];
	}
	delete[] hands;
	delete[] handSizes;

	return 0;
}
