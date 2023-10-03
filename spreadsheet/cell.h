#pragma once

#include "common.h"
#include "formula.h"
#include <unordered_set>
#include <optional>

class BaseCell {
public:
    virtual ~BaseCell() = default;
    virtual void Clear() = 0;
    virtual void Set(std::string text) = 0;
    virtual CellInterface::Value GetValue() const = 0;
    virtual std::string GetText() const  = 0;
    virtual std::vector<Position> GetReferencedCells() const = 0;
};


class EmptyCell : public BaseCell {
public:
    EmptyCell() = default;
    virtual ~EmptyCell() = default;

    void Clear() override{};
    void Set(std::string text) override{};

    CellInterface::Value GetValue() const override {
        return 0.0;
    };

    std::string GetText() const override {
        return "";
    };

    std::vector<Position> GetReferencedCells() const override {
        return{};
    };
};


class TextCell : public BaseCell {
public:
    TextCell() = default;
    TextCell(std::string text) : value_(std::move(text)) {};
    virtual ~TextCell() = default;

    void Clear() override {
        value_.clear();
    };

    void Set(std::string text) override {
        value_ = std::move(text);
    };

    CellInterface::Value GetValue() const override {
        if (value_.size() && value_[0] == ESCAPE_SIGN) {
            return value_.substr(1);
        }
        return value_;
    };

    std::string GetText() const override {
        return value_;
    };

    std::vector<Position> GetReferencedCells() const override {
        return{};
    };
private:
    std::string value_ = "";
};


class FormulaCell : public BaseCell {
public:
    FormulaCell() = default;
    FormulaCell(const SheetInterface& sheet)
        : sheet_(sheet)
    {}
    FormulaCell(const SheetInterface& sheet, std::string value) 
        : sheet_(sheet) {
        Set(value);
    }
    virtual ~FormulaCell() = default;

    void Clear() override {
        value_.reset();
    };

    void Set(std::string text) override {
        value_ = ParseFormula(std::move(text.substr(1)));
    };

    CellInterface::Value GetValue() const override {
        if (!value_) {
            return FormulaError(FormulaError::Category::Value);
        }
        
        auto result = value_->Evaluate(sheet_);
        if (std::holds_alternative<double>(result)) {
            return std::get<double>(result);
        }
        
        return std::get<FormulaError>(result);
    };

    std::string GetText() const override {
        return FORMULA_SIGN + value_->GetExpression();
    };

    std::vector<Position> GetReferencedCells() const override {
        return value_->GetReferencedCells();
    };
private:
    std::unique_ptr<FormulaInterface> value_;
    const SheetInterface& sheet_;
};


class Cell : public CellInterface {
public:
    Cell();
    ~Cell() = default;

    void Set(std::string text, const SheetInterface& sheet);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

private:
    std::unique_ptr<BaseCell> value_;
    mutable std::optional<double> cash_value_;

    std::unordered_set<Position, PositionHasher> cell_dependencies_up;
    //std::unordered_set<Position> cell_dependencies_down;
};
std::unique_ptr<CellInterface> CreateCell();