#pragma once

class WorldEntity {
private:
	const size_t entity_id;
	static size_t next_entity_id;
public:
	WorldEntity();
	size_t getID() const { return entity_id; }
};
