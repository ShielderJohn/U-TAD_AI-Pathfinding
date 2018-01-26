#include "stdafx.h"
#include "tinyxml.h"
#include "grid.h"
#include <map>
#include <string>

bool ReadGrid(const char* gridFilename, const char* weightsFilename, Grid& grid)
{
	// Grid
	TiXmlDocument gridDoc(gridFilename);
	if (!gridDoc.LoadFile())
	{
		fprintf(stderr, "Couldn't read path from %s", gridFilename);
		return false;
	}

	TiXmlHandle hGridDoc(&gridDoc);

	TiXmlElement* pGridElem;
	pGridElem = hGridDoc.FirstChildElement().Element();
	if (!pGridElem)
	{
		fprintf(stderr, "Invalid format for %s", gridFilename);
		return false;
	}

	// Weights
	TiXmlDocument weightsDoc(weightsFilename);
	if (!weightsDoc.LoadFile())
	{
		fprintf(stderr, "Couldn't read path from %s", weightsFilename);
		return false;
	}

	TiXmlHandle hWeightsDoc(&weightsDoc);

	TiXmlElement* pWeightElem;
	pWeightElem = hWeightsDoc.FirstChildElement().Element();
	if (!pWeightElem)
	{
		fprintf(stderr, "Invalid format for %s", weightsFilename);
		return false;
	}

	// Create map with weights
	std::map<std::string, int> weights;

	TiXmlHandle hWeightsRoot(pWeightElem);
	TiXmlHandle hWeights = hWeightsRoot.FirstChildElement("weights");

	TiXmlElement* weightElem = hWeights.FirstChild().Element();

	for (weightElem; weightElem; weightElem = weightElem->NextSiblingElement())
	{
		std::string weightType = weightElem->Attribute("type");

		int weightValue = 0;
		weightElem->Attribute("value", &weightValue);

		weights.insert(std::pair<std::string, int>(weightType, weightValue));
	}

	// Fill grid
	TiXmlHandle hGridRoot(pGridElem);
	TiXmlHandle hGrid = hGridRoot.FirstChildElement("grid");

	hGrid.Element()->Attribute("rows", &grid.mRows);
	hGrid.Element()->Attribute("columns", &grid.mColumns);
	hGrid.Element()->Attribute("width", &grid.mWidth);
	hGrid.Element()->Attribute("height", &grid.mHeight);

	TiXmlElement* rowElem = hGrid.FirstChild().Element();

	for (rowElem; rowElem; rowElem = rowElem->NextSiblingElement())
	{
		TiXmlElement* nodeElem = rowElem->FirstChildElement();

		for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
		{
			Node* node = new Node();

			nodeElem->Attribute("id", &node->mID);
			nodeElem->Attribute("row", &node->mRow);
			nodeElem->Attribute("column", &node->mColumn);

			node->mG = 0;
			node->mH = 0;

			std::string type = nodeElem->Attribute("type");
			node->mWeight = weights[type];

			node->mParent = nullptr;

			grid.mNodes.push_back(node);
		}
	}

	FillNodesNeighbours(grid);
	FillNodesCenterPoints(grid);

	return true;
}

void FillNodesNeighbours(const Grid& grid)
{
	int numberOfNodes = grid.mNodes.size();
	Node* nodei;
	Node* nodej;

	for (int i = 0; i < numberOfNodes; ++i)
	{
		nodei = grid.mNodes[i];

		for (int j = 0; j < numberOfNodes; ++j)
		{
			nodej = grid.mNodes[j];

			if (nodej->mRow == nodei->mRow)
			{
				if (nodej->mColumn == nodei->mColumn - 1 || nodej->mColumn == nodei->mColumn + 1)
				{
					grid.mNodes[i]->mNeighbours.push_back(grid.mNodes[j]);
				}
			}
			else if (nodej->mColumn == nodei->mColumn)
			{
				if (nodej->mRow == nodei->mRow - 1 || nodej->mRow == nodei->mRow + 1)
				{
					grid.mNodes[i]->mNeighbours.push_back(grid.mNodes[j]);
				}
			}
		}
	}
}

void FillNodesCenterPoints(const Grid& grid)
{
	int numberOfNodes = grid.mNodes.size();
	int nodeWidth = grid.mWidth / grid.mColumns;
	int nodeHeight = grid.mHeight / grid.mRows;

	for (int i = 0; i < numberOfNodes; ++i)
	{
		// X = (nodeWidth * nodeColumn) + (nodeWidth / 2)
		int x = (nodeWidth * grid.mNodes[i]->mColumn) + (nodeWidth * 0.5f) - SCREEN_HALF_WIDTH;

		// Y = (nodeHeight * nodeRow) + (nodeHeight / 2)
		int y = (nodeHeight * grid.mNodes[i]->mRow) + (nodeHeight * 0.5f) - SCREEN_HALF_HEIGHT;

		grid.mNodes[i]->mCenterPoint = USVec2D(x, y);
	}
}