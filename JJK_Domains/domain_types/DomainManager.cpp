#include "DomainManager.h"

DomainManager::DomainManager() {
}

DomainManager::~DomainManager() {
	for (int i = domainList.size() - 1; i >= 0; i--) {
		delete domainList[i];
		domainList.pop_back();
	}
}