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

int my_strcmp(const char* lhs, const char* rhs) 
{
	if (lhs == nullptr && rhs == nullptr) return 0;
	if (lhs == nullptr) return -1;
	if (rhs == nullptr) return 1;

	while (*lhs != '\0' && *rhs != '\0') 
	{
		if (*lhs > *rhs) 
			return 1;
		if (*lhs < *rhs)
			return -1;
		lhs++;
		rhs++;
	}

	if (*lhs == '\0' && *rhs == '\0')
		return 0;
	if (*lhs == '\0') return -1;
	return 1;
}

char* my_strcpy(char* dest, const char* src)
{
	if (dest == nullptr || src == nullptr) return nullptr;

	size_t i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';

	return dest;
}

char* my_trim(char* dest, const char* src)
{
	if (dest == nullptr || src == nullptr) return nullptr;
	if (!my_strlen(src))
	{
		dest[0] = '\0';
		return dest;
	}

	size_t srcLength = my_strlen(src);
	size_t start = 0;
	while (
		src[start] == ' ' ||
		src[start] == '\t' ||
		src[start] == '\n' ||
		src[start] == '\r' ||
		src[start] == '\v' ||
		src[start] == '\f'
		) 
	{
		start++;
		if (start == srcLength)
			break;
	}

	size_t end = srcLength - 1;
	while (
		src[end] == ' ' ||
		src[end] == '\t' ||
		src[end] == '\n' ||
		src[end] == '\r' ||
		src[end] == '\v' ||
		src[end] == '\f'
		) 
	{
		end--;
		if (end < start)
			break;
	}

	size_t k = 0;
	for (size_t i = start; i <= end; i++)
		dest[k++] = src[i];
	dest[k] = '\0';

	return dest;
}

/////////////////////////////////////
bool inputFailed(std::istream& input)
{
	if (input.fail()) 
	{
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

//////////////////////////////////////////////
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



int main()
{
	//Entering the number of players:
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

	//Entering the names of players:
	char** playerNames = new char* [numberOfPlayers];
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
	clearOutput(std::cout);

	std::cout << "Players: ";
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		size_t j = 0;
		while (playerNames[i][j++] != '\0') std::cout << playerNames[i][j - 1];
		if (i < numberOfPlayers - 1) std::cout << ", ";
	}
	std::cout << newline;

	//GAME LOGIC
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
	char discardPile[CARD_COUNT][MAX_CARD_LABEL_LENGTH + 1] = {};
	size_t discardPileLength = 0;

	//CARD SHUFFLE
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

	my_strcpy(discardPile[discardPileLength], drawPile[drawPileLength - 1]);
	discardPileLength++;
	drawPileLength--;

	std::cout << "Cards" << newline;
	for (size_t i = 0; i < numberOfPlayers; i++)
	{
		std::cout << playerNames[i] << ": ";
		for (size_t j = 0; j < handSizes[i]; j++)
		{
			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			std::cout << formatCard(formattedCard, hands[i][j]) << ' ';
		}
		std::cout << newline;
	}

	std::cin.get();
	clearOutput(std::cout);

	//GAME LOOP
	size_t currentPlayer = 0;
	bool direction = 1; //1 - ascending (clockwise), 0 - descending (counterclockwise)
	while (true) 
	{
		std::cout << "-----UNO-----" << newline;
		if (discardPileLength <= 0) break;

		{
			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			std::cout << "Current Discard Pile card: "  << formatCard(formattedCard, discardPile[discardPileLength - 1]) << newline;
		}

		std::cout << playerNames[currentPlayer] << " (Player " << currentPlayer + 1 << ") is in turn! Their cards are: " << newline;
		for (size_t i = 0; i < handSizes[currentPlayer]; i++)
		{
			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			std::cout << '[' << i + 1 << "] " << formatCard(formattedCard, hands[currentPlayer][i]) << "  ";
		}
		std::cout << newline;

		size_t cardToPlay = 0;
		std::cin >> cardToPlay;
		if(inputFailed(std::cin))
		{
			clearOutput(std::cout);
			std::cout << '\r' << "Invalid input!" << newline << "Pess any key to try again!" << newline;
			std::cin.get();
			clearOutput(std::cout);
			continue;
		}

		/* UNO DETECT
		char buffer[BUFFER_SIZE] = {};
		char trimmedBuffer[BUFFER_SIZE] = {};

		std::cin >> std::ws;
		std::cin.getline(buffer, BUFFER_SIZE);
		if (inputFailed(std::cin) || !my_strlen(my_trim(trimmedBuffer, buffer)))
		{
			clearOutput(std::cout);
			std::cout << '\r' << "Invalid input!" << newline;
			continue;
		}
		*/

		std::cin.get();
	}

	//MEMORY CLEANUP////////////////////////////
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
