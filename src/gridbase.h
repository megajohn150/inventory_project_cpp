#ifndef GRIDBASE_H
#define GRIDBASE_H

#include "item.h"
#include <iostream>
#include <vector>

class GridBase
{
protected:
    int rows;
    int cols;
    Item*** items;
    int currentRow;
    int currentCol;

public:
    GridBase(int r, int c);
    virtual ~GridBase();

    void clear();
    virtual void display() = 0;

    bool addItem(Item* item);
    bool removeItem(int row, int col);
    bool moveTo(int oldRow, int oldCol, int newRow, int newCol);
    void sort();

    // getters & setters
    int getRows() const;
    void setRows(int newRows);
    int getCols() const;
    void setCols(int newCols);
    Item*** getItems() const;
    Item* getItemOnSelectedRC(int row, int col);
    void setItems(Item*** newItems);
    int getCurrentRow() const;
    void setCurrentRow(int newCurrentRow);
    int getCurrentCol() const;
    void setCurrentCol(int newCurrentCol);
};

#endif
