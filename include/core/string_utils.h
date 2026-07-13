/**
 * @file mesh_tools\math\string_utils.h
 * @brief Small utilities for string manipulation.
 *
 * This header provides lightweight, header-only utilities used across the
 * math/mesh code.
 */
#ifndef __string_utils__
#define __string_utils__

#include <string>
#include <vector>
#include <sstream>

/**
 * @brief Extract the file extension from a filename.
 *
 * The function finds the last occurrence of the '.' character in `fname`
 * and returns the substring that follows it. Examples:
 * - "archive.tar.gz" -> "gz"
 * - "image.png"      -> "png"
 *
 * @param fname The filename or path string to inspect. Passed by const reference.
 * @return A std::string containing the characters after the last '.'.
 *
 * @note The function is declared `inline` to allow header-only inclusion
 * without violating the one-definition rule.
 */
inline std::string file_extension(const std::string& fname) {
	std::size_t dot = fname.find_last_of(".");
	if (dot == std::string::npos)
		return "";
	return fname.substr(dot + 1);
}

/**
 * @brief Change the file extension of a filename.
 *
 * This function replaces the existing file extension in `fname` with
 * `new_ext`. If `fname` has no extension, `new_ext` is appended.
 * Examples:
 * - ("document.txt", "md") -> "document.md"
 * - ("archive", "zip")     -> "archive.zip"
 *
 * @param fname The original filename or path string. Passed by const reference.
 * @param new_ext The new file extension to use (without leading dot).
 * @return A std::string with the updated filename.
 *
 * @note The function is declared `inline` to allow header-only inclusion
 * without violating the one-definition rule.
 */
inline std::string change_file_extension(const std::string& fname, const std::string& new_ext) {
    std::size_t dot = fname.find_last_of(".");
    if (dot == std::string::npos)
        return fname + "." + new_ext;
    return fname.substr(0, dot + 1) + new_ext;
}

/**
 * @brief Split a string into tokens based on a delimiter.
 *
 * This function takes an input string `str` and splits it into
 * substrings (tokens) wherever the specified `delimiter` character
 * is found. The resulting tokens are returned as a vector of strings.
 *
 * @param str The input string to be split. Passed by const reference.
 * @param delimiter The character used to separate tokens in the string.
 *                  Defaults to a space character (' ').
 * @return A std::vector<std::string> containing the extracted tokens.
 *
 * @note The function is declared `inline` to allow header-only inclusion
 * without violating the one-definition rule.
 */
inline std::vector<std::string> splitString(const std::string& str, char delimiter = ' ') {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * @brief Splits a C-style string into tokens based on a delimiter character.
 * 
 * This function parses the input string and extracts substrings separated by the
 * specified delimiter character. Empty tokens (consecutive delimiters) are skipped.
 * The resulting tokens are appended to the provided vector.
 * 
 * @param str The null-terminated input string to split. Must not be nullptr.
 * @param tokens Reference to a vector where the extracted tokens will be stored.
 *               Tokens are appended to the vector (existing content is preserved).
 * @param delimiter The character used to split the string. Defaults to space (' ').
 * 
 * @note This function does not clear the tokens vector before adding new elements.
 * @note Consecutive delimiters do not create empty tokens.
 * @note Leading and trailing delimiters are handled correctly (no empty tokens created).
 * 
 * @example
 * std::vector<std::string> tokens;
 * splitString("hello world test", tokens);
 * // tokens now contains: ["hello", "world", "test"]
 */
inline void splitString(const char* str, std::vector<std::string>& tokens, char delimiter = ' ') {
    size_t start = 0;
    size_t current = 0;

    while (str[current] != '\0') {
        if (str[current] == delimiter) {
            if (current > start) {
                tokens.emplace_back(str + start, current - start);
            }
            start = current + 1;
        }
        ++current;
    }
    
    if (current > start) {
        tokens.emplace_back(str + start, current - start);
    }
}


#endif // __string_utils__
