#pragma once

#include <vector>
#include <list>
#include <functional>

namespace Pathfinding {

class Node;

class Edge {
public:

	enum eFlags {
		CLOSED = (1<<0),
		RIVER = (1<<1),
	};

	Edge() : flags(0) {}
	virtual ~Edge() {}

	Node* target;
	float cost;

	unsigned int flags;
};

class Node {
public:

	enum eFlags {
		MEDKIT = (1<<0),
	};

	Node() : flags(0) {}
	virtual ~Node() {}

	std::vector<Edge> edges;

	unsigned int flags;

	// search data
	float hScore;
	float fScore;
	float gScore;
	Node* previous;

	static bool compareGScore(Node* a, Node* b) {
		return a->gScore < b->gScore;
	}

	static bool compareFScore(Node* a, Node* b) {
		return a->fScore < b->fScore;
	}
};

class Search {
public:
	// Dijkstra's Shortest Path methods
	static bool dijkstra(Node* start, Node* end, std::list<Node*>& path);
	static bool dijkstraFindFlags(Node* start, unsigned int flags, std::list<Node*>& path);

	// A* methods
	typedef std::function<float(Node* a, Node* b)> HeuristicCheck;
	static bool aStar(Node* start, Node* end, std::list<Node*>& path, HeuristicCheck heuristic);
		
private:

	Search() {}
};

}