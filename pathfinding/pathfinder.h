#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include <moaicore/MOAIEntity2D.h>

#include "grid.h"
#include "path.h"
#include <vector>

class Pathfinder: public virtual MOAIEntity2D
{
public:
	Pathfinder();
	~Pathfinder();

	virtual void DrawDebug();

	void SetStartPosition(float x, float y) { m_StartPosition = USVec2D(x, y); UpdatePath(); }
	void SetEndPosition(float x, float y) { m_EndPosition = USVec2D(x, y); UpdatePath(); }
	const USVec2D& GetStartPosition() const { return m_StartPosition;}
	const USVec2D& GetEndPosition() const { return m_EndPosition;}

    bool PathfindStep();

	void TracePath(const Node* node);
	Path GetPath() const { return mPath; }
private:
	void UpdatePath();

	Node* GetNodeFromPosition(const USVec2D& position);
	Node* PopNodeWithLowestCost(std::vector<Node*>& list);
	bool IsNodeInList(const Node* node, const std::vector<Node*>& list);
	int GetDistanceBetweenNodes(const Node* nodeA, const Node* nodeB);

	void Reset();
private:
	USVec2D m_StartPosition;
	USVec2D m_EndPosition;

	Grid mGrid;
	Path mPath;

	std::vector<Node*> mOpenList;
	std::vector<Node*> mClosedList;

	Node* mStartNode;
	Node* mTargetNode;

	// Lua configuration
public:
	DECL_LUA_FACTORY(Pathfinder)
public:
	virtual void RegisterLuaFuncs(MOAILuaState& state);
private:
	static int _setStartPosition(lua_State* L);
	static int _setEndPosition(lua_State* L);
    static int _pathfindStep(lua_State* L);
};


#endif