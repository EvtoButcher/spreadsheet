#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {

    class Formula : public FormulaInterface {
    public:
        // Реализуйте следующие методы:
        explicit Formula(std::string expression) 
            try : ast_(ParseFormulaAST(std::move(expression)))
            {}
            catch (const std::exception& err) {
                throw FormulaException(err.what());
            };

        Value Evaluate(const SheetInterface& sheet) const override {
            auto func = [&sheet](const Position pos) -> CellInterface* 
            {
                return const_cast<CellInterface*>(sheet.GetCell(pos));
            };
            try
            {
                return ast_.Execute(func); 
            }
            catch (const FormulaError& err)
            {
                return err;
            }
        }

        std::string GetExpression() const override { 
            std::ostringstream ostr;
            ast_.PrintFormula(ostr);
            return ostr.str();
        }

        std::vector<Position> GetReferencedCells() const override {
            auto cell_list = ast_.GetCells();

            if (cell_list.empty()) {
                return {};
            }

            std::vector<Position>result(cell_list.begin(), cell_list.end());
            return { result.begin(), std::unique(result.begin(), result.end()) };
        };

    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}