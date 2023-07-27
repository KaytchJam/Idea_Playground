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

bool DomainManager::add(Domain& d) {
	Domain* newmain = NULL;
	switch (d.getDomainType()) {
	case DomainType::CLOSED_DOMAIN:
		newmain = new ClosedDomain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getOriginCoords());
		break;
	case DomainType::OPEN_DOMAIN:
		newmain = new OpenDomain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getOriginCoords());
		break;
	case DomainType::SIMPLE_DOMAIN:
		newmain = new Domain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getOriginCoords());
		// newmain = new SimpleDomain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getOriginCoords());
		break;
	default:
		std::cout << "Not a valid Domain, could not add to list." << std::endl;
		return false;
	}

	sf::Color doubleCheck = newmain->getColor();
	doubleCheck.a = 0xFF;
	newmain->setColor(doubleCheck);
	domainList.push_back(newmain);
	return true;
}

bool DomainManager::add(DomainType type, float radius, sf::Color color, float refine_val, sf::Vector2f centerCoords) {
	Domain* newmain = NULL;
	switch (type) {
	case CLOSED_DOMAIN:
		newmain = new ClosedDomain(radius, color, refine_val, centerCoords);
		break;
	case OPEN_DOMAIN:
		newmain = new OpenDomain(radius, color, refine_val, centerCoords);
		// newmain = new OpenDomain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getOriginCoords());
		break;
	case SIMPLE_DOMAIN:
		newmain = new Domain(radius, color, refine_val, centerCoords);
		// newmain = new SimpleDomain(d.getBaseRadius(), d.getColor(), d.getRefinement(), d.getOriginCoords());
		break;
	default:
		std::cout << "Not a valid domain type, could not add to list." << std::endl;
		return false;
	}

	domainList.push_back(newmain);
	return true;
}

void DomainManager::remove(int index) {
	Domain* d = domainList[index];
	delete d;
	domainList.erase(domainList.begin() + index);
	this->domainList = domainList;
}

void DomainManager::overlapSearch(int index) {
	Domain* d = domainList[index];

	bool t = false;
	if (!d->isSelected()) {
		for (int i = 0; i < domainList.size(); i++) {
			Domain* other = domainList[i];
			if (i != index && !other->isSelected() && !(other->isConsumed()) && d->inRange(*other)) {
				//std::cout << *other << std::endl;
				d->consume(*other);
				d->setOriginPosition(d->getOriginCoords());
				t = true;
			}
		}

		sf::Color line_color = d->getColor();
		if (!t) { 
			d->circle.setFillColor(sf::Color(line_color.r, line_color.g, line_color.b, 51)); 
			d->TUG_OF_WAR = false;
		}
		else {
			d->circle.setFillColor(sf::Color(line_color.r, line_color.g, line_color.b, 0));
			d->TUG_OF_WAR = true;
		}
			
	}
}