#include "Pathfinding.h"

#include <set>

namespace Pathfinding {

bool Search::dijkstra(Node* start, Node* end, std::list<Node*>& path) {

	std::list<Node*> openList;
	std::set<Node*> closedList;

	start->previous = nullptr;
	start->gScore = 0.f;

	end->previous = nullptr;

	openList.push_front(start);

	// do search
	while (openList.empty() == false) {

		openList.sort(Node::compareGScore);

		Node* current = openList.front();

		if (current == end)
			break;

		openList.pop_front();
		closedList.insert(current);

		// add all connections to openList, if they don't exist
		for (auto edge : current->edges) {
			Node* target = edge.target;
			float gScore = current->gScore += edge.cost;

			// is it already closed?
			if (closedList.find(target) == closedList.end()) {

				auto iter = std::find(openList.begin(), openList.end(), target);
				if (iter == openList.end()) {
					// add to the open list
					target->previous = current;
					target->gScore = gScore;
					openList.push_back(target); // doesn't matter where we put it in openList; we sort anyway
				}
				else if (gScore < target->gScore) {
					target->gScore = gScore;
					target->previous = current;
				}
			}
		}
	}

	if (end->previous != nullptr) {
		// path found!

		path.clear();

		while (end != nullptr) {
			path.push_front(end);
			end = end->previous;
		}

		return true;
	}

	return false;

}

bool Search::dijkstraFindFlags(Node* start, unsigned int flags, std::list<Node*>& path) {

	std::list<Node*> openList;
	std::set<Node*> closedList;

	start->previous = nullptr;
	start->gScore = 0.f;

	Node* end = nullptr;

	openList.push_front(start);

	// do search
	while (openList.empty() == false) {

		openList.sort(Node::compareGScore);

		Node* current = openList.front();

		if ((current->flags & flags) == flags) {
			end = current;
			break;
		}

		openList.pop_front();
		closedList.insert(current);

		// add all connections to openList, if they don't exist
		for (auto edge : current->edges) {
			Node* target = edge.target;
			float gScore = current->gScore += edge.cost;

			// is it already closed?
			if (closedList.find(target) == closedList.end()) {

				auto iter = std::find(openList.begin(), openList.end(), target);
				if (iter == openList.end()) {
					// add to the open list
					target->previous = current;
					target->gScore = gScore;
					openList.push_back(target); // doesn't matter where we put it in openList; we sort anyway
				}
				else if (gScore < target->gScore) {
					target->gScore = gScore;
					target->previous = current;
				}
			}
		}
	}

	if (end->previous != nullptr) {
		// path found!

		path.clear();

		while (end != nullptr) {
			path.push_front(end);
			end = end->previous;
		}

		return true;
	}

	return false;

}

bool Search::aStar(Node* start, Node* end, std::list<Node*>& path, HeuristicCheck heuristic) {

	std::list<Node*> openList;
	std::set<Node*> closedList;

	start->previous = nullptr;
	start->gScore = 0.f;
	start->hScore = heuristic(start, end);
	start->fScore = start->gScore + start->fScore;

	end->previous = nullptr;

	openList.push_front(start);

	// do search
	while (openList.empty() == false) {

		// compare using fScore
		openList.sort(Node::compareFScore);

		Node* current = openList.front();

		if (current == end)
			break;

		openList.pop_front();
		closedList.insert(current);

		// add all connections to openList, if they don't exist
		for (auto edge : current->edges) {
			Node* target = edge.target;
			float gScore = current->gScore += edge.cost;

			// is it already closed?
			if (closedList.find(target) == closedList.end()) {

				auto iter = std::find(openList.begin(), openList.end(), target);
				if (iter == openList.end()) {
					// add to the open list
					target->previous = current;
					target->gScore = gScore;

					// include heuristic and final cost
					target->hScore = heuristic(target, end);
					target->fScore = gScore + target->hScore;

					openList.push_back(target); // doesn't matter where we put it in openList; we sort anyway
				}
				else if (gScore < target->gScore) {
					target->gScore = gScore;

					// update final cost
					target->fScore = gScore + target->hScore;

					target->previous = current;
				}
			}
		}
	}

	if (end->previous != nullptr) {
		// path found!

		path.clear();

		while (end != nullptr) {
			path.push_front(end);
			end = end->previous;
		}

		return true;
	}

	return false;

}

}