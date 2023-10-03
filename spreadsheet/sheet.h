#pragma once

#include "cell.h"
#include "common.h"
#include <unordered_map>
#include <unordered_set>

#include <functional>

class Sheet : public SheetInterface {
public:
    Sheet() = default;
    ~Sheet() = default;

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;
    
    void ClearCell(Position pos) override;
    
    Size GetPrintableSize() const override;
    
    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:

    std::unordered_map<Position, std::unique_ptr<CellInterface>, PositionHasher> cells;
    std::unordered_map<int, int> rows;
    std::unordered_map<int, int> cols;

    Size printable_size_ = {0,0};

    bool CheckCircularReference(Position pos);
    bool CheckCircularReference(Position pos, Position start_pos, const std::unordered_set<Position>& past_cells);
    void UpdateDependencies(Position pos);
    void UpdateCash(Position pos);
    void ClearDependences(Position del_pos);
};
