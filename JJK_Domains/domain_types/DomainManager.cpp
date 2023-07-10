#include "DomainManager.h"

DomainManager::DomainManager() {
}

void DomainManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (int i = 0; i < domainList.size(); i++) {
		domainList[i]->draw(target, states);
	}
}

DomainManager::~DomainManager() {
	for (int i = domainList.size() - 1; i >= 0; i--) {
		delete domainList[i];
		domainList.pop_back();
	}
}

bool DomainManager::add(ClosedDomain& d) {
	ClosedDomain* newmain = new ClosedDomain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getCenterCoords());
	sf::Color doubleCheck = newmain->getColor();
	doubleCheck.a = 0xFF;
	newmain->setColor(doubleCheck);
	domainList.push_back(newmain);
}