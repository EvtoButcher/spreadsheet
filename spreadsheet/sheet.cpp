#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <vector>

using namespace std::literals;

//Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("SetCell - Invalid position");
    }

    printable_size_.cols = pos.col + 1 > printable_size_.cols ? pos.col + 1 : printable_size_.cols;
    printable_size_.rows = pos.row + 1 > printable_size_.rows ? pos.row  + 1: printable_size_.rows;
    
    auto cell_ptr = CreateCell();
    static_cast<Cell*>(cell_ptr.get())->Set(text, *this);
    cells[pos] = std::move(cell_ptr);

    rows[pos.row]++;
    cols[pos.col]++;

}

const CellInterface* Sheet::GetCell(Position pos) const {
    return cells.count(pos) && pos.IsValid() ? cells.at(pos).get() : nullptr;
}
CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("GetCell - Invalid position");
    }
    return cells.count(pos) ? cells.at(pos).get() : nullptr;
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("ClearCell - Invalid position");
    }

    if (cells.count(pos)) {
        cells.at(pos).reset();
        cells.erase(pos);
        if (!(--rows[pos.row])) {
            rows.erase(pos.row);
        }
        if (!(--cols[pos.col])) {
            cols.erase(pos.col);
        }
    }
    else {
        return;
    }

    if ((pos.row + 1) >= printable_size_.rows){ 
        while (printable_size_.rows && !rows.count(printable_size_.rows - 1)) {
            --printable_size_.rows;
        }
    }
    if ((pos.col + 1) >= printable_size_.cols){ 
        while (printable_size_.cols && !cols.count(printable_size_.cols - 1)) {
            --printable_size_.cols;
        }
    }
}

Size Sheet::GetPrintableSize() const {
    return printable_size_;
}

void Sheet::PrintValues(std::ostream& output) const {

    for (int i = 0; i < printable_size_.rows; ++i){
        for (int j = 0; j < printable_size_.cols; ++j){
            if (cells.count({ i, j })) {
                std::visit(
                   [&](const auto& x) {
                        output << x;
                    },
                    cells.at({ i, j })->GetValue());
            }
            if (j < printable_size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {

    for (int i = 0; i < printable_size_.rows; ++i) {
        for (int j = 0; j < printable_size_.cols; ++j) {
            if (cells.count({ i, j })) {
                output << cells.at({ i, j })->GetText();
            }
            if (j < printable_size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}