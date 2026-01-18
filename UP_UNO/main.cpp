#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>

const char* newline = "\r\n";

// Constants
const size_t BUFFER_SIZE = 256;
const size_t MIN_PLAYERS = 2;
const size_t MAX_PLAYERS = 4;
const size_t CARD_COUNT = 30;
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
	clearOutput(std::cout);

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
	///*const*/ char drawPile[CARD_COUNT][MAX_CARD_LABEL_LENGTH + 1] = {
	//	// Red
	//	"R_0","R_1","R_2","R_3","R_4","R_5","R_6","R_7","R_8","R_9",
	//	"R_SKIP","R_REVERSE","R_+2",
	//	"R_1","R_2","R_3","R_4","R_5","R_6","R_7","R_8","R_9",
	//	"R_SKIP","R_REVERSE","R_+2",
	//
	//	// Green
	//	"G_0","G_1","G_2","G_3","G_4","G_5","G_6","G_7","G_8","G_9",
	//	"G_SKIP","G_REVERSE","G_+2",
	//	"G_1","G_2","G_3","G_4","G_5","G_6","G_7","G_8","G_9",
	//	"G_SKIP","G_REVERSE","G_+2",
	//
	//	// Yellow
	//	"Y_0","Y_1","Y_2","Y_3","Y_4","Y_5","Y_6","Y_7","Y_8","Y_9",
	//	"Y_SKIP","Y_REVERSE","Y_+2",
	//	"Y_1","Y_2","Y_3","Y_4","Y_5","Y_6","Y_7","Y_8","Y_9",
	//	"Y_SKIP","Y_REVERSE","Y_+2",
	//
	//	// Blue
	//	"B_0","B_1","B_2","B_3","B_4","B_5","B_6","B_7","B_8","B_9",
	//	"B_SKIP","B_REVERSE","B_+2",
	//	"B_1","B_2","B_3","B_4","B_5","B_6","B_7","B_8","B_9",
	//	"B_SKIP","B_REVERSE","B_+2",
	//
	//	// Wilds
	//	"W_WILD",
	//	"W_WILD",
	//	"W_WILD",
	//	"W_WILD",
	//	"W_WILD+4",
	//	"W_WILD+4",
	//	"W_WILD+4",
	//	"W_WILD+4"
	//};
	char drawPile[CARD_COUNT][MAX_CARD_LABEL_LENGTH + 1] = {
		// Red
		"R_0","R_1","R_2","R_3","R_4","R_5","R_6","R_7","R_8","R_9",


		// Green
		"G_0","G_1","G_2","G_3","G_4","G_5","G_6","G_7","G_8","G_9",


		// Yellow
		"Y_0","Y_1","Y_2","Y_3","Y_4","Y_5","Y_6","Y_7","Y_8","Y_9",


		// Blue
	};
	size_t drawPileSize = CARD_COUNT;
	char discardPile[CARD_COUNT][MAX_CARD_LABEL_LENGTH + 1] = {};
	size_t discardPileSize = 0;

	//CARD SHUFFLE
	std::random_device rndDevice = {};
	std::mt19937 mtGenerator(rndDevice());
	std::shuffle(drawPile, drawPile + drawPileSize, mtGenerator);

	char*** hands = new char** [numberOfPlayers];
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
			my_strcpy(hands[i][j], drawPile[--drawPileSize]);
		}
		handSizes[i] = INITIAL_CARD_DRAW;
	}

	my_strcpy(discardPile[discardPileSize], drawPile[drawPileSize - 1]);
	discardPileSize++;
	drawPileSize--;

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

	std::cin.ignore();
	clearOutput(std::cout);

	//GAME LOOP
	size_t currentPlayer = 0;
	bool direction = 1; //1 - ascending (clockwise), 0 - descending (counterclockwise)
	while (true)
	{
		clearOutput(std::cout);
		std::cout << "-----UNO-----" << newline;
		if (discardPileSize <= 0) break;

		const char* discardPileCard = discardPile[discardPileSize - 1];
		{
			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			std::cout << "Current Discard Pile card: " << formatCard(formattedCard, discardPileCard) << newline;
		}

		std::cout << playerNames[currentPlayer] << " (Player " << currentPlayer + 1 << ") is in turn! Their cards are: " << newline;
		bool hasValidCard = false;
		for (size_t i = 0; i < handSizes[currentPlayer]; i++)
		{
			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			hasValidCard = hasValidCard || isCardValid(hands[currentPlayer][i], discardPileCard);
			std::cout << '[' << i + 1 << "] " << formatCard(formattedCard, hands[currentPlayer][i]) << "  ";
		}
		std::cout << newline;
		if (!hasValidCard)
		{
			std::cout << "No valid cards... Automatically drawing card from the Draw Pile: ";
			addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			std::cout << '[' << handSizes[currentPlayer] << "] " << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << newline;

			if (!isCardValid(hands[currentPlayer][handSizes[currentPlayer] - 1], discardPileCard))
			{
				std::cout << "Still no valid cards... Press Enter to skip!" << newline;
				prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);
				std::cin.get();

				continue;
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
			continue;
		}
		if (cardToPlay >= handSizes[currentPlayer])
		{
			clearOutput(std::cout);
			std::cout << '\r' << "Invalid input!" << newline << "Press Enter to try again!" << newline;
			std::cin.ignore();
			std::cin.get();
			clearOutput(std::cout);
			continue;
		}

		const char* playedCard = hands[currentPlayer][cardToPlay];
		if (!isCardValid(playedCard, discardPileCard))
		{
			clearOutput(std::cout);
			std::cout << '\r' << "Invalid card played!" << newline << "Press Enter key to try again!" << newline;
			std::cin.ignore();
			std::cin.get();
			clearOutput(std::cout);
			continue;
		}

		{
			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			std::cout << "You played: " << formatCard(formattedCard, playedCard) << newline;
		}

		char playedCardValue[MAX_CARD_LABEL_LENGTH + 1] = {};
		my_strcpy(playedCardValue, playedCard + 2);


		addCardToDiscardPile(playedCard, discardPile, discardPileSize);
		removeCardFromHand(cardToPlay, hands[currentPlayer], handSizes[currentPlayer]);

		//Apply card effects
		if (!my_strcmp(playedCardValue, "REVERSE"))
		{
			direction = !direction;
			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
		}
		else if (!my_strcmp(playedCardValue, "SKIP"))
		{
			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);
			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
		}
		else if (!my_strcmp(playedCardValue, "+2"))
		{
			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
			std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
			addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
			std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << " drawed from the next player" << newline;

			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);
			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
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

			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
		}
		else if (!my_strcmp(playedCardValue, "WILD+4"))
		{
			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);

			char formattedCard[MAX_CARD_LABEL_LENGTH + 1] = {};
			addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
			std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
			addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
			std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
			addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
			std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << ", ";
			addCardToHand(drawPile, drawPileSize, hands[currentPlayer], handSizes[currentPlayer], discardPile, discardPileSize, &std::cout);
			std::cout << formatCard(formattedCard, hands[currentPlayer][handSizes[currentPlayer] - 1]) << " drawed from the next player" << newline;

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

			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
		}
		else
		{
			prepareNextPlayerIndex(currentPlayer, direction, numberOfPlayers);
			std::cin.ignore();
			std::cout << "Press Enter to continue!" << newline;
			std::cin.get();
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
