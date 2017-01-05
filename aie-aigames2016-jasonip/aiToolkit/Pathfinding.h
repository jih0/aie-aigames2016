#pragma once

#include <vector>
#include <list>

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
	float gScore;
	Node* previous;

	static bool compareGScore(Node* a, Node* b) {
		return a->gScore < b->gScore;
	}
};

class Search {
public:

	static bool dijkstra(Node* start, Node* end, std::list<Node*>& path);
	static bool dijkstraFindFlags(Node* start, unsigned int flags, std::list<Node*>& path);

private:

	Search() {}
};

}