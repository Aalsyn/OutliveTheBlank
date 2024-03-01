#ifndef nodePath_h
#define nodePath_h

class PathNode {
	physx::PxVec3 position;
public:
	PathNode() = delete;
	PathNode(physx::PxVec3 pos);
	PathNode(int x, int y, int z);
	~PathNode() = default;
	const physx::PxVec3& Position() const;
};

class SubPath {
	std::vector<PathNode> nodes;
public:
	SubPath();
	SubPath(const std::vector<PathNode>& nodes);
	~SubPath() = default;
	std::vector<PathNode>& get();
	const PathNode& getNode(const int& index);
	const int getSize();
};

class Path {
	std::vector<std::vector<SubPath>> paths;
	int currentPath;
	int currentSubPath;
	int currentNode;
	const PathNode& nextNode(const int& nextSubPath=0);
	const PathNode& currNode();

public:
	Path();
	Path(const std::vector<std::vector<SubPath>>& paths);
	~Path() = default;
	bool hasNodeBeenReached(const physx::PxVec3& position);
	void advanceNextNode(const int& nextSubPath =0);
	physx::PxVec3 getDirectionToCurrentNode(const physx::PxVec3& position, const int& nextSubPath = 0);
	void resetProgress();
	const std::vector<std::vector<SubPath>>& getPath();
	void SetProgressTo(int p, int subP, int n);
	void SetProgressTo(const Path& path);
	int getCurrentPath();
	int getCurrentSubPath();
	int getCurrentNode();
};

#endif

