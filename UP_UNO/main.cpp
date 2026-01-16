#include <iostream>
#include <cstdlib>

const char* newline = "\r\n";

//
const size_t MIN_PLAYERS = 2;
const size_t MAX_PLAYERS = 4;
//

//ANSI Escape codes
// std::cout << "\033[31mR\033[0m";
// std::cout << "\033[32mG\033[0m";
// std::cout << "\033[33mY\033[0m";
// std::cout << "\033[34mB\033[0m";
// std::cout << "\033[2J\033[H";    //clear

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
            src[start] == ' '  ||
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

    char** playerNames = new char*[numberOfPlayers];
    for (size_t i = 0; i < numberOfPlayers; i++) {
        char buffer[256] = {};
        char trimmedBuffer[256] = {};

        std::cout << "Enter the name of Player " << i + 1 << ": ";
        std::cin >> std::ws;
        std::cin.getline(buffer, 256);
        while (inputFailed(std::cin) || !my_strlen(my_trim(trimmedBuffer, buffer)))
        {
            clearOutput(std::cout);
            std::cout << '\r' << "Invalid input!" << newline;
            std::cout << "Enter the name of Player " << i + 1 << ": ";
            std::cin >> std::ws;
            std::cin.getline(buffer, 256);
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



    ////////////////////////////////////////////
    for (size_t i = 0; i < numberOfPlayers; i++) 
    {
        delete[] playerNames[i];
    }
    delete[] playerNames;

    return 0;
}
