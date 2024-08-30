// WaveFunctionCollapseTest.cpp : Runs Wave Function Collapse in Console Application. 
// Red index : That index will update next draw
// Purple index : That index has a chance to update next draw
// Indexs Read : UP, RIGHT, DOWN, LEFT
// IMPORTANT : DRAWING GRIDS ONLY WORKS WITH GRIDS THAT CAN FIT ON SCREEN

#include <iostream>
#include <algorithm>
#include <windows.h> 
#include <vector>

// Constant Variables
const int SIDESIZE = 110; // Legnth of each side of the grid
const int SIZEARRAY = SIDESIZE * SIDESIZE; // # of indexs in grid
const int OPTIONSIZE = 5; // How many options a index can be (ex: 0 - " ", 1 - "^", 2 - ">", 3 - "v", 4 - "<")

//Structs
struct Tile {
    std::vector<int> edges;

    std::vector<int> up;
    std::vector<int> right;
    std::vector<int> down;
    std::vector<int> left;

    // Sets rule for each type of tiles UP, RIGHT, DOWN, LEFT locations
    void analyze(std::vector<Tile> tiles)
    {
        for (int i = 0; i < tiles.size(); i++)
        {
            Tile tile = tiles[i];
            //up
            if (tile.edges[2] == this->edges[0]) {
                this->up.push_back(i);
            }

            //right
            if (tile.edges[3] == this->edges[1]) {
                this->right.push_back(i);
            }

            //down
            if (tile.edges[0] == this->edges[2]) {
                this->down.push_back(i);
            }

            //left
            if (tile.edges[1] == this->edges[3]) {
                this->left.push_back(i);
            }
        }
    }
};

struct GridSpot {
    bool collapsed = false;
    std::vector<int> options;
    int GridLocation = -1;

    GridSpot() {
        options.resize(OPTIONSIZE);

        for (int i = 0; i < OPTIONSIZE; i++)
        {
            options[i] = i;
        }
    }
};

// Grid and Tile Arrays
std::vector<GridSpot> grid(SIZEARRAY);
std::vector<Tile> Tiles(OPTIONSIZE);

// Helper Functionns
const bool compareInterval(GridSpot a, GridSpot b)
{
    return a.options.size() < b.options.size();
}

void checkValid(std::vector<int>& array, std::vector<int>& valid)
{
    for (int i = array.size() - 1; i >= 0; i--)
    {
        if (std::count(std::begin(valid), std::end(valid), array.at(i)) == 0)
        {
            array.erase(array.begin() + i);
        }
    }
}

char gridSpotConverter(int input) {
    switch (input) {
    case 0:
        return char(32);
    case 1:
        return char(193);
    case 2:
        return char(195);
    case 3:
        return char(194);
    case 4:
        return char(180);
    default:
        return '*';
    }
}

std::vector<GridSpot> slicing(std::vector<GridSpot>& arr, int X)
{
    auto start = arr.begin();
    auto end = arr.begin() + X;

    std::vector<GridSpot> result(X);

    copy(start, end, result.begin());

    return result;
}

std::vector<GridSpot> filterCollapsed(const std::vector<GridSpot>& arr)
{
    std::vector<GridSpot> returnArray;
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr.at(i).collapsed != true)
        {
            returnArray.push_back(arr.at(i));
        }
    }
    return returnArray;
}

bool inLowestEntropy(std::vector<GridSpot>& lowestEntropyArr, int index)
{
    for (GridSpot spot : lowestEntropyArr)
    {
        if (spot.collapsed == false && spot.GridLocation == index) { return true; }
    }
    return false;
}

// Draw Grid Functions
void DrawGridColor(std::vector<GridSpot>& lowestEntropys)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int column = 0; column < SIDESIZE; column++)
    {
        for (int row = 0; row < SIDESIZE; row++)
        {
            SetConsoleTextAttribute(handle, 7);
            //print GridSpot
            if (grid[row + column * SIDESIZE].collapsed == true)
            {               
                std::cout << gridSpotConverter(grid[row + column * SIDESIZE].options.front());
            }
            else
            {
                // Set up Grid Spot Color (only checking future uncollapsed indexs)
                if (inLowestEntropy(lowestEntropys, row + (column * SIDESIZE)))
                {
                    if (lowestEntropys.size() == 1)
                    {
                        SetConsoleTextAttribute(handle, 12);
                    }
                    else if (lowestEntropys.at(0).options.size() < 5)
                    {
                        SetConsoleTextAttribute(handle, 13);
                    }
                }

                std::cout << "0";
            }
        }
        std::cout << "\n";
    }
}

void DrawGrid()
{
    for (int column = 0; column < SIDESIZE; column++)
    {
        for (int row = 0; row < SIDESIZE; row++)
        {
            //print GridSpot
            if (grid[row + column * SIDESIZE].collapsed == true) {
                std::cout << gridSpotConverter(grid[row + column * SIDESIZE].options.front());
            }
            else {
                std::cout << "0";
            }     
        }
        std::cout << "\n";
    }
}

// Grid Funcitons
void SetupOptions()
{
    // Setup for Tile Options ( Up, Right, Down, Left )
    Tiles.at(0).edges = { 0,0,0,0 };
    Tiles.at(1).edges = { 1,1,0,1 };
    Tiles.at(2).edges = { 1,1,1,0 };
    Tiles.at(3).edges = { 0,1,1,1 };
    Tiles.at(4).edges = { 1,0,1,1 };

    for (int i = 0; i < Tiles.size(); i++)
    {
        Tile tile = Tiles.at(i);
        tile.analyze(Tiles);
        Tiles.at(i) = tile;
    }

    // Setup for Grid Spot Indexs
    for (int i = 0; i < grid.size(); i++)
    {
        grid.at(i).GridLocation = i;
    }
}

void UpdateNextGridSpots() // Main Algorithm Here
{
    std::vector<GridSpot> nextGrid(SIZEARRAY);
    std::vector<int> validOptions;
    validOptions.reserve(OPTIONSIZE * 4);

    for (int column = 0; column < SIDESIZE; column++)
    {
        for (int row = 0; row < SIDESIZE; row++)
        {
            int index = row + (column * SIDESIZE);
            if (grid.at(index).collapsed)
            {
                nextGrid.at(index) = grid.at(index);
            }
            else
            {
                // Set up all possible options
                std::vector<int> options(OPTIONSIZE);
                for (int i = 0; i < OPTIONSIZE; i++)
                {
                    options.at(i) = i;
                }

                //Look up
                if (column > 0)
                {
                    GridSpot up = grid[row + (column - 1) * SIDESIZE];
                    if (up.options[0] != 5) {
                        for (int option : up.options)
                        {
                            std::vector<int> valid = Tiles[option].down; // <--- List of valid GridSpots
                            validOptions.insert(validOptions.end(), valid.begin(), valid.end());
                        }
                        checkValid(options, validOptions);
                    }
                    validOptions.clear();
                }
                //Look right
                if (row < SIDESIZE - 1)
                {
                    GridSpot right = grid[row + 1 + (column * SIDESIZE)];
                    if (right.options[0] != 5) {
                        for (int option : right.options)
                        {
                            std::vector<int> valid = Tiles[option].left; // <--- List of valid GridSpots
                            validOptions.insert(validOptions.end(), valid.begin(), valid.end());
                        }
                        checkValid(options, validOptions);
                    }
                    validOptions.clear();
                }
                //Look down
                if (column < SIDESIZE - 1)
                {
                    GridSpot down = grid[row + (column + 1) * SIDESIZE];
                    if (down.options[0] != 5) {
                        for (int option : down.options)
                        {
                            std::vector<int> valid = Tiles[option].up; // <--- List of valid GridSpots
                            validOptions.insert(validOptions.end(), valid.begin(), valid.end());
                        }
                        checkValid(options, validOptions);
                    }
                    validOptions.clear();
                }
                //Look left
                if (row > 0)
                {
                    GridSpot left = grid[row - 1 + (column * SIDESIZE)];
                    if (left.options[0] != 5) {
                        for (int option : left.options)
                        {
                            std::vector<int> valid = Tiles[option].right; // <--- List of valid GridSpots
                            validOptions.insert(validOptions.end(), valid.begin(), valid.end());
                        }
                        checkValid(options, validOptions);
                    }
                    validOptions.clear();
                }

                // Create new grid spot and update on next grid
                GridSpot newSpot;
                newSpot.collapsed = false;
                newSpot.options = options;
                newSpot.GridLocation = grid[index].GridLocation;
                nextGrid[index] = newSpot;
            }
        }
    }

    // Updates grid for next run
    grid = nextGrid;
}

void UpdateGrid(bool shouldDraw)
{
    // Setting up copy of grid and saving the locations of the grid that need updating
    std::vector<GridSpot> OGgridCopy(SIZEARRAY);
    std::copy(std::begin(grid), std::end(grid), std::begin(OGgridCopy));
    std::vector<GridSpot> gridCopy = filterCollapsed(OGgridCopy);
    std::sort(std::begin(gridCopy), std::end(gridCopy), compareInterval);

    // Getting the stop index/location of the last lowest Entropy
    int len = gridCopy[0].options.size();
    int stopIndex = 0;
    for (int i = 1; i < gridCopy.size(); i++) {
        if (gridCopy[i].options.size() > len) {
            stopIndex = i;
            break;
        }
    }

    // Grab the lowest Entropy cells in Grid Copy
    std::vector<GridSpot> gridLowestEntropy;
    if (stopIndex > 0)
    {
        gridLowestEntropy = slicing(gridCopy, stopIndex);
    }
    else
    {
        gridLowestEntropy = gridCopy;
    }

    // Draw Grid onto screen every time but last time
    if (shouldDraw && gridCopy.size() > 1) { DrawGridColor(gridLowestEntropy); }

    // Pick random cell from the lowest and update its value
    srand(time(NULL));
    GridSpot& cell = gridLowestEntropy.at(rand() % gridLowestEntropy.size());
    cell.collapsed = true;
    if (cell.options.size() > 0)
    {
        int pick = cell.options.at(rand() % cell.options.size());
        cell.options = { pick };
    }
    else
    {
        // Set cell to * in case it does not find a solution
        cell.options = { 5 };
    }

    grid.at(cell.GridLocation) = cell;

    // Update next grid spots
    UpdateNextGridSpots();

    // Draw Grid onto screen one last time
    if (shouldDraw && gridCopy.size() <= 1) { DrawGrid(); }
}

// Main Function
int main()
{
    system("cls"); // Clear screen before next run

    // Needed setup before grid update
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetupOptions();

    // Run through text options and start program
    std::string choice = "";
    while (choice == "")
    {
        std::cout << "Would you like to step through or automate Wave Function Collapse? 1 - Step Through | 2 - Automate | 3 - Automate No Draw (for Large Grids)\n";
        std::cin >> choice;
        system("cls");

        int numberofruns = SIZEARRAY;
        if (choice == "1")
        {
            while (numberofruns > 0)
            {
                UpdateGrid(true);

                std::cout << "\n";
                std::cin.ignore();

                numberofruns--;
                if (numberofruns != 0) { SetConsoleCursorPosition(handle, {0,0}); };
            }

            // Just to make sure program doesnt close if you hold enter
            std::string stopBeforeClosing = "";
            std::cin >> stopBeforeClosing;
        }
        else if (choice == "2")
        {
            while (numberofruns > 0) 
            {
                UpdateGrid(true);

                std::cout << "\n";

                numberofruns--;
                if (numberofruns != 0) { SetConsoleCursorPosition(handle, {0,0}); };
            }
        }
        else if (choice == "3")
        {
            while (numberofruns > 0) 
            {
                std::cout << "Making Grid... " << numberofruns << " Updates Left";
                UpdateGrid(false);
                numberofruns--;
                SetConsoleCursorPosition(handle, { 0,0 });
            }
            DrawGrid();
        }
        else
        {
            std::cout << "Please choose a valid option. \n";
            choice = "";
        }
    }

    return 0;
}