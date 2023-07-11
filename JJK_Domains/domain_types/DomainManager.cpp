#include "DomainManager.h"

DomainManager::DomainManager() {
}

void DomainManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (int i = 0; i < domainList.size(); i++) {
		domainList[i]->draw(target, states);
	}
}

DomainManager::~DomainManager() {
	for (int i = (int) domainList.size() - 1; i >= 0; i--) {
		delete domainList[i];
		domainList.pop_back();
	}
}

bool DomainManager::add(ClosedDomain& d) {
	ClosedDomain* newmain = new ClosedDomain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getOriginCoords());
	sf::Color doubleCheck = newmain->getColor();
	doubleCheck.a = 0xFF;
	newmain->setColor(doubleCheck);
	domainList.push_back(newmain);
	return true;
}

bool DomainManager::add(float radius, sf::Color color, float refine_val, sf::Vector2f centerCoords) {
	ClosedDomain* newmain = new ClosedDomain(radius, color, refine_val, centerCoords);
	domainList.push_back(newmain);
	return true;
}

void DomainManager::remove(int index) {
	ClosedDomain* d = domainList[index];
	delete d;
	domainList.erase(domainList.begin() + index);
	this->domainList = domainList;
}

void DomainManager::overlapSearch(int index) {
	ClosedDomain* d = domainList[index];
	if (!d->isSelected()) {
		for (int i = 0; i < domainList.size(); i++) {
			ClosedDomain* other = domainList[i];
			if (i != index && !other->isSelected() && !(other->isConsumed()) && d->inRange(*other)) {
				std::cout << *other << std::endl;
				d->consume(*other);
				d->setOriginPosition(d->getOriginCoords());
			}
		}
	}

}