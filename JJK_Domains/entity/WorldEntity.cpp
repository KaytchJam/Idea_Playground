#include "WorldEntity.h"

size_t WorldEntity::next_entity_id = 0;
WorldEntity::WorldEntity() : entity_id(next_entity_id++) {}