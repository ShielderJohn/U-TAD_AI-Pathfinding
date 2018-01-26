#include <stdafx.h>

#include "pathfinder.h"

Pathfinder::Pathfinder() : MOAIEntity2D()
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
	RTTI_END

	ReadGrid("grid.xml", "weights.xml", mGrid);
}

Pathfinder::~Pathfinder()
{

}

void Pathfinder::UpdatePath()
{
	Reset();

	mStartNode = GetNodeFromPosition(m_StartPosition);
	mTargetNode = GetNodeFromPosition(m_EndPosition);

	if (mStartNode == nullptr || mTargetNode == nullptr)
	{
		return;
	}

	mOpenList.push_back(mStartNode);

	while (mOpenList.size() > 0)
	{
		Node* currentNode = PopNodeWithLowestCost(mOpenList);

		if (currentNode != nullptr)
		{
			mClosedList.push_back(currentNode);

			if (currentNode->mID == mTargetNode->mID)
			{
				TracePath(currentNode);
				return;
			}

			int numberOfNeighbours = currentNode->mNeighbours.size();

			for (int i = 0; i < numberOfNeighbours; ++i)
			{
				if (!currentNode->mNeighbours[i]->IsWalkable() || IsNodeInList(currentNode->mNeighbours[i], mClosedList))
				{
					continue;
				}

				int costToNeighbour = currentNode->mG + GetDistanceBetweenNodes(currentNode, currentNode->mNeighbours[i]);

				if (costToNeighbour < currentNode->mNeighbours[i]->mG || !IsNodeInList(currentNode->mNeighbours[i], mOpenList))
				{
					currentNode->mNeighbours[i]->mG = costToNeighbour;
					currentNode->mNeighbours[i]->mH = GetDistanceBetweenNodes(currentNode->mNeighbours[i], mTargetNode);
					currentNode->mNeighbours[i]->mParent = currentNode;

					if (!IsNodeInList(currentNode->mNeighbours[i], mOpenList))
					{
						mOpenList.push_back(currentNode->mNeighbours[i]);
					}
				}
			}
		}
	}
}

void Pathfinder::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

	int nodeWidth = mGrid.mWidth / mGrid.mColumns;
	int nodeHeight = mGrid.mHeight / mGrid.mRows;
	int nodeHalfWidth = nodeWidth * 0.5f;
	int nodeHalfHeight = nodeHeight * 0.5f;

	int nodeCenterX = 0;
	int nodeCenterY = 0;

	// Draw grid and nodes center points
	int gridSize = mGrid.mNodes.size();

	if (gridSize > 0)
	{
		for (int i = 0; i < gridSize; ++i)
		{
			nodeCenterX = mGrid.mNodes[i]->mCenterPoint.mX;
			nodeCenterY = mGrid.mNodes[i]->mCenterPoint.mY;

			if (mGrid.mNodes[i]->mWeight < 0)
			{
				gfxDevice.SetPenColor(0.65f, 0.22f, 0.33f, 1.0f);
				MOAIDraw::DrawRectFill(nodeCenterX - nodeHalfWidth, nodeCenterY - nodeHalfHeight, nodeCenterX + nodeHalfWidth, nodeCenterY + nodeHalfHeight);
			}

			gfxDevice.SetPenColor(0.85f, 0.85f, 0.85f, 1.0f);
			MOAIDraw::DrawRectOutline(nodeCenterX - nodeHalfWidth, nodeCenterY - nodeHalfHeight, nodeCenterX + nodeHalfWidth, nodeCenterY + nodeHalfHeight);
			MOAIDraw::DrawEllipseFill(nodeCenterX, nodeCenterY, 1.5f, 1.5f, 50);
		}
	}

	// Draw open list
	int mOpenListSize = mOpenList.size();

	for (int i = 0; i < mOpenListSize; ++i)
	{
		nodeCenterX = mOpenList[i]->mCenterPoint.mX;
		nodeCenterY = mOpenList[i]->mCenterPoint.mY;

		gfxDevice.SetPenColor(0.0f, 0.65f, 0.0f, 1.0f);
		MOAIDraw::DrawEllipseFill(nodeCenterX, nodeCenterY, 4.0f, 4.0f, 50);
	}

	// Draw closed list
	int mClosedListSize = mClosedList.size();

	for (int i = 0; i < mClosedListSize; ++i)
	{
		nodeCenterX = mClosedList[i]->mCenterPoint.mX;
		nodeCenterY = mClosedList[i]->mCenterPoint.mY;

		if (mClosedList[i]->mID == mStartNode->mID)
		{
			gfxDevice.SetPenColor(0.0f, 0.55f, 0.55f, 1.0f);
			MOAIDraw::DrawEllipseFill(nodeCenterX, nodeCenterY, 7.0f, 7.0f, 50);
		}
		else if (mClosedList[i]->mID == mTargetNode->mID)
		{
			gfxDevice.SetPenColor(0.55f, 0.15f, 0.55f, 1.0f);
			MOAIDraw::DrawEllipseFill(nodeCenterX, nodeCenterY, 7.0f, 7.0f, 50);
		}
		else
		{
			gfxDevice.SetPenColor(0.65f, 0.0f, 0.0f, 1.0f);
			MOAIDraw::DrawEllipseFill(nodeCenterX, nodeCenterY, 4.0f, 4.0f, 50);
		}
	}

	// Draw path
	int pathSize = mPath.mPoints.size();

	if (pathSize > 0)
	{
		for (int i = 0; i < pathSize; ++i)
		{
			if (i > 0)
			{
				gfxDevice.SetPenColor(0.65f, 0.65f, 0.65f, 1.0f);
				MOAIDraw::DrawLine(mPath.mPoints[i - 1], mPath.mPoints[i]);
			}
		}
	}
}

bool Pathfinder::PathfindStep()
{
    // returns true if pathfinding process finished

    return true;
}

void Pathfinder::TracePath(const Node* node)
{
	if (node != nullptr)
	{
		const Node* pathNode = node;
		mPath.mPoints.push_back(node->mCenterPoint);

		while (pathNode->mParent != nullptr)
		{
			pathNode = pathNode->mParent;
			mPath.mPoints.insert(mPath.mPoints.begin(), pathNode->mCenterPoint);
		}
	}
}

Node* Pathfinder::GetNodeFromPosition(const USVec2D& position)
{
	int numberOfNodes = mGrid.mNodes.size();
	int nodeWidth = mGrid.mWidth / mGrid.mColumns;
	int nodeHeight = mGrid.mHeight / mGrid.mRows;
	int nodeHalfWidth = nodeWidth * 0.5f;
	int nodeHalfHeight = nodeHeight * 0.5f;

	for (int i = 0; i < numberOfNodes; ++i)
	{
		int distanceX = abs(position.mX - mGrid.mNodes[i]->mCenterPoint.mX);
		int distanceY = abs(position.mY - mGrid.mNodes[i]->mCenterPoint.mY);

		if (mGrid.mNodes[i]->mWeight >= 0 && distanceX <= nodeHalfWidth && distanceY <= nodeHalfHeight)
		{
			return mGrid.mNodes[i];
		}
	}

	return nullptr;
}

Node* Pathfinder::PopNodeWithLowestCost(std::vector<Node*>& list)
{
	int mOpenListSize = list.size();

	if (mOpenListSize > 0)
	{
		Node* nodeToReturn = list[0];
		int indexOfNodeWithLowestCost = 0;

		for (int i = 0; i < mOpenListSize; ++i)
		{
			if (list[i]->GetF() < nodeToReturn->GetF())
			{
				nodeToReturn = list[i];
				indexOfNodeWithLowestCost = i;
			}
		}

		list.erase(list.begin() + indexOfNodeWithLowestCost);

		return nodeToReturn;
	}

	return nullptr;
}

bool Pathfinder::IsNodeInList(const Node* node, const std::vector<Node*>& list)
{
	int listSize = list.size();

	for (int i = 0; i < listSize; ++i)
	{
		if (list[i]->mID == node->mID)
		{
			return true;
		}
	}

	return false;
}

int Pathfinder::GetDistanceBetweenNodes(const Node* nodeA, const Node* nodeB)
{
	int distanceX = abs(nodeA->mCenterPoint.mX - nodeB->mCenterPoint.mX);
	int distanceY = abs(nodeA->mCenterPoint.mY - nodeB->mCenterPoint.mY);

	if (distanceX > distanceY)
	{
		return 14 * distanceY + 10 * (distanceX - distanceY);
	}

	return 14 * distanceX + 10 * (distanceY - distanceX);
}

void Pathfinder::Reset()
{
	mPath.mPoints.clear();
	mOpenList.clear();
	mClosedList.clear();

	// Reset nodes values
	int gridSize = mGrid.mNodes.size();

	if (gridSize > 0)
	{
		for (int i = 0; i < gridSize; ++i)
		{
			mGrid.mNodes[i]->mG = 0;
			mGrid.mNodes[i]->mH = 0;
			mGrid.mNodes[i]->mParent = nullptr;
		}
	}
}













//lua configuration ----------------------------------------------------------------//
void Pathfinder::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity::RegisterLuaFuncs(state);

	luaL_Reg regTable [] = {
		{ "setStartPosition",		_setStartPosition},
		{ "setEndPosition",			_setEndPosition},
        { "pathfindStep",           _pathfindStep},
		{ NULL, NULL }
	};

	luaL_register(state, 0, regTable);
}

int Pathfinder::_setStartPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetStartPosition(pX, pY);
	return 0;
}

int Pathfinder::_setEndPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetEndPosition(pX, pY);
	return 0;
}

int Pathfinder::_pathfindStep(lua_State* L)
{
    MOAI_LUA_SETUP(Pathfinder, "U")

    self->PathfindStep();
    return 0;
}