#ifndef UTILS_H

#define UTILS_H

const char* newline = "\r\n";

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

#endif // !UTILS_H
