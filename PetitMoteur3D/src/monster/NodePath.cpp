#include "NodePath.h"

using namespace physx;
using namespace std;

class indexOutOfBound {};

//NODE

PathNode::PathNode(PxVec3 pos) {
	position = pos;
}

PathNode::PathNode(int x, int y, int z) {
	position = PxVec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

const PxVec3& PathNode::Position() const {
	return position;
}

//SUBPATH

SubPath::SubPath() {
	nodes = {};
}

SubPath::SubPath(const std::vector<PathNode>& _nodes) {
	nodes = _nodes;
}

vector<PathNode>& SubPath::get() {
	return nodes;
}

const PathNode& SubPath::getNode(const int& index) {
	if (index < nodes.size()) {
		return nodes[index];
	}
	throw indexOutOfBound();
}

const int& SubPath::getSize() {
	return static_cast<int>(nodes.size());
};

//PATH

Path::Path() {
	resetProgress();
	paths = {};
}

Path::Path(const vector<vector<SubPath>>& _paths) {
	resetProgress();
	paths = _paths;
}

const std::vector<std::vector<SubPath>>& Path::getPath() {
	return paths;
}

void Path::SetProgressTo(int p, int subP, int n) {
	currentPath = p;
	currentSubPath = subP;
	currentNode = n;
}

void Path::SetProgressTo(const Path& path) {
	currentPath = path.currentPath;
	currentSubPath = path.currentSubPath;
	currentNode = path.currentNode;
}

int Path::getCurrentPath() {
	return currentPath;
}
int Path::getCurrentSubPath() {
	return currentSubPath;
}
int Path::getCurrentNode() {
	return currentNode;
}

const PathNode& Path::nextNode(const int& nextSubPath) {
	if (currentNode + 1  < paths[currentPath][currentSubPath].getSize()) {
		//next node is in current subPath
		return paths[currentPath][currentSubPath].getNode(currentNode + 1);
	}
	else if (currentPath + 1 < paths.size()) {
			//next node is in next path
			return paths[currentPath+1][nextSubPath].getNode(0);
	}
	else {
		//next node is to loop the path
		return paths[0][0].getNode(0);
	}
}

const PathNode& Path::currNode() {
	return paths[currentPath][currentSubPath].getNode(currentNode);
}

bool Path::hasNodeBeenReached(const physx::PxVec3& position) {
	PxVec3 positionDiff = currNode().Position() - position;
	if (positionDiff.magnitudeSquared() < 10.0f) {
		return true;
	}
	return false;
}

void Path::advanceNextNode(const int& nextSubPath) {
	if (currentNode + 1 < paths[currentPath][currentSubPath].getSize()) {
		//next node is in current path
		currentNode++;
	}
	else if (currentPath + 1 < paths.size()) {
		//next node is in next path
		currentPath++;
		currentSubPath = min(static_cast<int>(paths[currentPath].size() - 1), nextSubPath);
		currentNode = 0;
	}
	else {
		//next node is to loop the path
		resetProgress();
	}
}

PxVec3 Path::getDirectionToCurrentNode(const PxVec3& position, const int& nextSubPath) {
	const PxVec3 nextPosition = currNode().Position();
	PxVec3 positionDiff = nextPosition - position;
	return positionDiff.getNormalized();
}

void Path::resetProgress() {
	SetProgressTo(0, 0, 0);
}

