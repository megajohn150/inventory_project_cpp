#include "inventory.h"
#include <vector>
#include <cctype>
#include <sstream>
#include <algorithm>
Inventory::Inventory() : GridBase(4, 4) {}

void Inventory::expandCols(int addCols)
{
    int newCols = cols + addCols;

    Item*** newItems = new Item**[rows];
    for (int i = 0; i < rows; i++) {
        newItems[i] = new Item*[newCols];
        for (int j = 0; j < newCols; j++) {
            if (j < cols)
                newItems[i][j] = items[i][j];
            else
                newItems[i][j] = nullptr;
        }
        delete[] items[i];
    }
    delete[] items;

    items = newItems;
    cols  = newCols;
}

bool Inventory::addSmallBackpack()
{
    if (hasSmallBackpack) return false;
    hasSmallBackpack = true;
    expandCols(1);
    return true;
}

bool Inventory::addLargeBackpack()
{
    if (hasLargeBackpack) return false;
    hasLargeBackpack = true;
    expandCols(2);
    return true;
}

void Inventory::display(Equipment* eq, bool equipMode)
{

    int baseEnd  = 4;
    int smallEnd = hasSmallBackpack ? baseEnd + 1 : baseEnd;
    // int largeEnd = hasLargeBackpack ? smallEnd + 2 : smallEnd;
    if (hasSmallBackpack || hasLargeBackpack) {

        std::cout << "                    ";
        if (hasSmallBackpack) {
            std::cout << "    SBP ";
        }
        if (hasLargeBackpack) {
            std::cout << "       BBP    ";
        }
        std::cout << "\n";
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (j == baseEnd && hasSmallBackpack)
                std::cout << "   ";
            if (j == smallEnd && hasLargeBackpack)
                std::cout << "   ";

            bool selected = !equipMode && (i == currentRow && j == currentCol);
            if (selected) std::cout << ">[";
            else          std::cout << " [";

            if (items[i][j] == nullptr)
                std::cout << " ";
            else
                std::cout << char(std::toupper(items[i][j]->getName()[0]));

            if (selected) std::cout << "]<";
            else          std::cout << "] ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";



    if (eq)
        eq->display(equipMode);
    else {
        std::cout << "         [ ]\n";
        std::cout << "     [ ][   ][ ]\n";
        std::cout << "     { }[   ]{ }\n";
        std::cout << "        [ | ]\n";
        std::cout << "        [ | ]\n";
    }
}

void Inventory::clearInv(Equipment* eq)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            removeItem(i, j);
        }
    }
    if (eq) {
        eq->clear();
    }
}

bool Inventory::invNotEmpCheckup(){
    std::vector<Item*> temp;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(items[i][j] != nullptr){
                temp.push_back(items[i][j]);
            }
        }
    }
    if(temp.size() > 0) return true;
    else return false;
}

void Inventory::cleanUp()
{
    std::vector<Item*> temp;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(items[i][j] != nullptr){
                temp.push_back(items[i][j]);
                items[i][j] = nullptr;
            }
        }
    }
    int k = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            items[i][j] = (k < (int)temp.size()) ? temp[k++] : nullptr;
        }
    }
}

void Inventory::sortByRarity()
{
    std::vector<Item*> temp;
    auto eachRarity = [&](Rarity r){
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                if(items[i][j] != nullptr){
                    if(items[i][j]->getRarity() == r && items[i][j]->getCategory() != "medkit"){
                        temp.push_back(items[i][j]);
                        items[i][j] = nullptr;
                    }
                }
            }
        }
    };
    eachRarity(unknown);
    eachRarity(legendary);
    eachRarity(epic);
    eachRarity(rare);
    eachRarity(uncommon);
    eachRarity(common);


    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(items[i][j] != nullptr && items[i][j]->getCategory() == "medkit"){
                temp.push_back(items[i][j]);
                items[i][j] = nullptr;
            }
        }
    }

    int k = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            items[i][j] = (k < (int)temp.size()) ? temp[k++] : nullptr;
        }
    }
}

void Inventory::sortByType()
{
    std::vector<Item*> temp;
    auto eachType = [&](Type t){
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                if(items[i][j] != nullptr){
                    if(items[i][j]->getType() == t && items[i][j]->getCategory() != "medkit"){
                        temp.push_back(items[i][j]);
                        items[i][j] = nullptr;
                    }
                }
            }
        }
    };
    eachType(unseen);
    eachType(diamond);
    eachType(gold);
    eachType(iron);
    eachType(stone);
    eachType(wooden);


    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(items[i][j] != nullptr && items[i][j]->getCategory() == "medkit"){
                temp.push_back(items[i][j]);
                items[i][j] = nullptr;
            }
        }
    }

    int k = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            items[i][j] = (k < (int)temp.size()) ? temp[k++] : nullptr;
        }
    }
}

void Inventory::filter()
{
    maskedItems.clear();
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(items[i][j] != nullptr && items[i][j]->getType() != diamond){
                maskedItems[{i,j}] = items[i][j]->getName();
                items[i][j]->setName(".");
            }
        }
    }
    filtered = true;
}

void Inventory::unfilter()
{
    for(auto& [pos, name] : maskedItems){
        int i = pos.first;
        int j = pos.second;
        if(items[i][j] != nullptr){
            items[i][j]->setName(name);
        }
    }
    maskedItems.clear();
    filtered = false;
}



int Inventory::levenshteinDistance(std::string firstString, std::string secondString)
{
    int firstLength = firstString.length();
    int secondLength = secondString.length();

    // Create a 2D table to store edit distances
    std::vector<std::vector<int>> editDistanceTable(firstLength + 1, std::vector<int>(secondLength + 1));

    // Initialize first column (deletions from firstString)
    for (int row = 0; row <= firstLength; row++) {
        editDistanceTable[row][0] = row;
    }

    // Initialize first row (insertions to match secondString)
    for (int col = 0; col <= secondLength; col++) {
        editDistanceTable[0][col] = col;
    }

    // Fill the table using dynamic programming
    for (int row = 1; row <= firstLength; row++) {
        for (int col = 1; col <= secondLength; col++) {
            if (tolower(firstString[row - 1]) == tolower(secondString[col - 1])) {
                // Characters match - no operation needed
                editDistanceTable[row][col] = editDistanceTable[row - 1][col - 1];
            } else {
                // Take minimum of: delete, insert, or substitute
                editDistanceTable[row][col] = 1 + std::min({
                                                  editDistanceTable[row - 1][col],     // deletion
                                                  editDistanceTable[row][col - 1],     // insertion
                                                  editDistanceTable[row - 1][col - 1]  // substitution
                                              });
            }
        }
    }

    return editDistanceTable[firstLength][secondLength];
}

std::pair<int, int> Inventory::searchNames(std::string target)
{
    std::vector<std::tuple<int, int, std::string, int>> results;


    std::string normalizedTarget = target;
    std::transform(normalizedTarget.begin(), normalizedTarget.end(), normalizedTarget.begin(), ::tolower);

    std::vector<std::string> targetWords;
    std::stringstream tss(normalizedTarget);
    std::string tword;
    while (tss >> tword) targetWords.push_back(tword);

    bool twoWords = targetWords.size() >= 2;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (items[i][j] == nullptr) continue;

            std::string itemName = items[i][j]->getName();
            std::string itemType = items[i][j]->getTypeString();

            std::string normName = itemName;
            std::string normType = itemType;
            std::transform(normName.begin(), normName.end(), normName.begin(), ::tolower);
            std::transform(normType.begin(), normType.end(), normType.begin(), ::tolower);

            int bestDist = INT_MAX;

            if (twoWords) {

                std::string t0 = targetWords[0];
                std::string t1 = targetWords[1];

                int maxLen0 = std::max((int)t0.length(), (int)normType.length());
                int threshold0 = std::max(1, maxLen0 / 4);
                int distType = levenshteinDistance(t0, normType);

                int maxLen1 = std::max((int)t1.length(), (int)normName.length());
                int threshold1 = std::max(1, maxLen1 / 4);
                int distName = levenshteinDistance(t1, normName);


                if (normType.find(t0) != std::string::npos) distType = 0;
                if (normName.find(t1) != std::string::npos) distName = 0;

                if (distType <= threshold0 && distName <= threshold1) {
                    bestDist = distType + distName;
                }
            } else {

                std::string t0 = targetWords.empty() ? normalizedTarget : targetWords[0];

                int maxLenName = std::max((int)t0.length(), (int)normName.length());
                int thresholdName = std::max(1, maxLenName / 4);
                int distName = levenshteinDistance(t0, normName);
                if (normName.find(t0) != std::string::npos) distName = 0;
                if (distName <= thresholdName) bestDist = std::min(bestDist, distName);

                int maxLenType = std::max((int)t0.length(), (int)normType.length());
                int thresholdType = std::max(1, maxLenType / 4);
                int distType = levenshteinDistance(t0, normType);
                if (normType.find(t0) != std::string::npos) distType = 0;
                if (distType <= thresholdType) bestDist = std::min(bestDist, distType);
            }

            if (bestDist != INT_MAX) {
                results.push_back({i, j, itemName, bestDist});
            }
        }
    }

    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return std::get<3>(a) < std::get<3>(b);
    });

    if (results.empty()) return {-1, -1};

    auto [bestRow, bestCol, bestName, bestDist] = results[0];
    return {bestRow, bestCol};
}
