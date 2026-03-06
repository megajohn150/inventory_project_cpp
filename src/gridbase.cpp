#include "gridbase.h"

GridBase::GridBase(int r, int c)
{
    rows = r;
    cols = c;
    currentRow = 0;
    currentCol = 0;

    items = new Item**[rows];
    for (int i = 0; i < rows; i++) {
        items[i] = new Item*[cols];
        for (int j = 0; j < cols; j++) {
            items[i][j] = nullptr;
        }
    }
}

GridBase::~GridBase()
{
    clear();
    for (int i = 0; i < rows; i++) {
        delete[] items[i];
    }
    delete[] items;
}

void GridBase::clear()
{
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (items[i][j] != nullptr){
                delete items[i][j];
                items[i][j] = nullptr;
            }
        }
    }
}

bool GridBase::addItem(Item *item)
{
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (items[i][j] == nullptr){
                items[i][j] = item;
                return true;
            }
        }
    }
    return false;
}

bool GridBase::removeItem(int row, int col)
{
    if (items[row][col] != nullptr){
        delete items[row][col];
        items[row][col] = nullptr;
        return true;
    }
    return false;
}

bool GridBase::moveTo(int oldRow, int oldCol, int newRow, int newCol)
{
    if (oldRow < 0 || oldRow >= rows || oldCol < 0 || oldCol >= cols ||
        newRow < 0 || newRow >= rows || newCol < 0 || newCol >= cols)
        return false;

    if (items[oldRow][oldCol] == nullptr)
        return false;

    if (items[newRow][newCol] != nullptr) {
        Item* temp = items[oldRow][oldCol];
        items[oldRow][oldCol] = items[newRow][newCol];
        items[newRow][newCol] = temp;
    } else {
        items[newRow][newCol] = items[oldRow][oldCol];
        items[oldRow][oldCol] = nullptr;
    }
    return true;
}

void GridBase::sort()
{
    std::vector<Item*> allItems;

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (items[i][j] != nullptr){
                allItems.push_back(items[i][j]);
                items[i][j] = nullptr;
            }
        }
    }

    for (Item* item : allItems)
        addItem(item);
}

int GridBase::getRows() const { return rows; }
void GridBase::setRows(int newRows) { rows = newRows; }
int GridBase::getCols() const { return cols; }
void GridBase::setCols(int newCols) { cols = newCols; }
Item*** GridBase::getItems() const { return items; }
Item* GridBase::getItemOnSelectedRC(int row, int col) {
    if(row < 0 || row >= rows || col < 0 || col >= cols)
        return nullptr;
    return items[row][col];
}

void GridBase::setItems(Item*** newItems) { items = newItems; }
int GridBase::getCurrentRow() const { return currentRow; }

void GridBase::setCurrentRow(int newCurrentRow)
{
    if (newCurrentRow >= 0 && newCurrentRow < rows) currentRow = newCurrentRow;
    else if (newCurrentRow < 0) currentRow = rows - 1;
    else currentRow = 0;
}

int GridBase::getCurrentCol() const { return currentCol; }

void GridBase::setCurrentCol(int newCurrentCol)
{
    if (newCurrentCol >= 0 && newCurrentCol < cols) currentCol = newCurrentCol;
    else if (newCurrentCol < 0) currentCol = cols - 1;
    else currentCol = 0;
}
