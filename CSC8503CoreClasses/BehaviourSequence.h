#pragma once
#include "BehaviourNodeWithChildren.h"

class BehaviourSequence : public BehaviourNodeWithChildren {
public:
	BehaviourSequence(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
	~BehaviourSequence() {}
	BehaviourState Execute(float dt) override {
		for (auto& i : childNodes) {
			std::cout << "EXECUTING" << std::endl;
			BehaviourState nodeState = i->Execute(dt);
			switch (nodeState) {
				case Success: continue;
				case Failure:
				case Ongoing:
				{
					currentState = nodeState;
					return nodeState;
				}
			}
		}
		return Success;
	}
};