#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional> 


// Реализуйте следующие методы
Cell::Cell() : value_(std::make_unique<EmptyCell>()) {}

void Cell::Set(std::string text, const SheetInterface& sheet) {
	if (text.size() > 1 && text[0] == FORMULA_SIGN) {
		auto formula = std::make_unique<FormulaCell>(sheet, std::move(text));
		for (const auto pos : formula->GetReferencedCells()) {
			cell_dependencies_up.insert(pos);
		}
		value_ = std::move(formula);
	}
	else if (text.size() > 0) {
		value_ = std::make_unique<TextCell>(std::move(text));
	}
	else {
		value_ = std::make_unique<EmptyCell>();
	}
}

void Cell::Clear() {
	value_.reset();
	value_ = std::make_unique<EmptyCell>();
}

Cell::Value Cell::GetValue() const {
	if (cash_value_) {
		return cash_value_.value();
	}
	Value result = value_->GetValue();
	if (std::holds_alternative<double>(result)) {
		cash_value_ = std::get<double>(result);
	}
	return result;
}
std::string Cell::GetText() const {
	return value_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
	return {};
}

std::unique_ptr<CellInterface> CreateCell()
{
	return std::make_unique<Cell>();
}
