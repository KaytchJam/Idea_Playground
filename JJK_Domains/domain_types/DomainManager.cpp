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
	sf::Color dColor = d.getColor();
	dColor.a = 0xFF;
	return add(d.getDomainType(), d.getBaseRadius(), dColor, d.getRefinement(), d.getOriginCoords());
}

bool DomainManager::add(DomainType type, float radius, sf::Color color, float refine_val, sf::Vector2f centerCoords) {
	Domain* newmain = NULL;
	switch (type) {
	case DomainType::CLOSED_DOMAIN:
		newmain = new ClosedDomain(radius, color, refine_val, centerCoords);
		break;
	case DomainType::OPEN_DOMAIN:
		newmain = new OpenDomain(radius, color, refine_val, centerCoords);
		break;
	case DomainType::SIMPLE_DOMAIN:
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

	bool in_tug = false;
	if (!d->isSelected()) {
		for (int i = 0; i < domainList.size(); i++) {
			Domain* other = domainList[i];
			if (i != index && !other->isSelected() && !(other->isConsumed()) && d->inRange(*other)) {
				//std::cout << *other << std::endl;
				d->consume(*other);
				d->setOriginPosition(d->getOriginCoords());
				in_tug = true;
			}
		}

		sf::Color line_color = d->getColor();
		if (!in_tug) { 
			d->circle.setFillColor(sf::Color(line_color.r, line_color.g, line_color.b, 0x33)); // 0x33 -> 51
			d->TUG_OF_WAR = false;
		} else {
			d->circle.setFillColor(sf::Color(line_color.r, line_color.g, line_color.b, 0x0));
			d->TUG_OF_WAR = true;
		}
	}
}