#include "common.h"

#include <cctype>
#include <sstream>
#include <algorithm>
#include <cmath>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;
const int CHAR_OFFSET = 64;

const Position Position::NONE = { -1, -1 };

bool Position::operator==(const Position rhs) const {
    return row == rhs.row && col == rhs.col;
}

bool Position::operator<(const Position rhs) const {
    return std::tie(row, col) < std::tie(rhs.row, rhs.col);
}

bool Position::IsValid() const {
    return row >= 0 && col >= 0 && row < MAX_ROWS && col < MAX_COLS;
}

std::string Position::ToString() const {
    if (!IsValid()) {
        return "";
    }

    std::string result;
    result.reserve(MAX_POSITION_LENGTH);
    int c = col;
    while (c >= 0) {
        result.insert(result.begin(), 'A' + c % LETTERS);
        c = c / LETTERS - 1;
    }

    result += std::to_string(row + 1);

    return result;
}

bool IsValidTableIndex(const std::string_view row, const std::string_view col) {
    if (row.empty() || col.empty() || col.size() > MAX_POS_LETTER_COUNT || row.size() > MAX_POSITION_LENGTH) {
        return false;
    }

    for (char letter : col) {
        if (!std::isalpha(letter) || !std::isupper(letter)) {
            return false;
        }
    }

    for (char symbol : row) {
        if (!std::isdigit(symbol)) {
            return false;
        }
    }

    return true;
}

int GetPositionByLetter(const std::string_view col) {
    int position = 0;
    int r_size = col.size() - 1;
    for (char letter : col) {
        position += ((int)letter - CHAR_OFFSET) * std::pow(LETTERS, r_size);
        r_size--;
    }
    return --position;
}

Position Position::FromString(std::string_view str) {
    auto border = str.find_first_of("0123456789");
    if (border > str.size()) {
        return Position::NONE;
    }
    auto col = str.substr(0, border);
    auto row = str.substr(border);

    if (IsValidTableIndex(row, col)) {
        Position pos{ std::stoi(std::string(row)) - 1, GetPositionByLetter(col) };

        return pos.IsValid() ? pos : Position::NONE;
    }

    return Position::NONE;
}

bool Size::operator==(Size rhs) const {
    return cols == rhs.cols && rows == rhs.rows;
}