#ifndef __GRID_H__
#define __GRID_H__

#include <vector>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const int SCREEN_HALF_WIDTH = SCREEN_WIDTH * 0.5f;
const int SCREEN_HALF_HEIGHT = SCREEN_HEIGHT * 0.5f;

struct Node
{
	int mID;
	int mRow;
	int mColumn;

	int mG;
	int mH;
	int mWeight;

	Node* mParent;
	std::vector<Node*> mNeighbours;

	USVec2D mCenterPoint;

	int GetF() const { return mG + mH + mWeight; }
	bool IsWalkable() const { return mWeight >= 0; }
};

struct Grid
{
	int mRows;
	int mColumns;

	int mWidth;
	int mHeight;

	std::vector<Node*> mNodes;
};

bool ReadGrid(const char* gridFilename, const char* weightsFilename, Grid& grid);
void FillNodesNeighbours(const Grid& grid);
void FillNodesCenterPoints(const Grid& grid);

#endif