#pragma once
#include <vector>
#include <functional>

template <typename Type>
class ObjectGroup {
	std::vector<Type*> list;

public:
	ObjectGroup<Type>() {}
	ObjectGroup<Type>(size_t size) { list = std::vector<Type*>(size); }
	ObjectGroup<Type>(std::vector<Type*> li) { list = li; }
	ObjectGroup<Type>(std::initializer_list<Type*> data) { list = std::vector<Type*>(data); }

	ObjectGroup& add(Type* ptr) {
		list.push_back(ptr);
		return *this;
	}

	void applyAll(void** args, size_t num_args, void (*call)(Type* t, size_t idx, void** func_args, size_t func_n_args)) {
		for (size_t i = 0; i < list.size(); i++) call(list[i], i, args, num_args);
	}

	void map_mut(void (*call)(Type* t)) {
		for (size_t i = 0; i < list.size(); i++) call(list[i]);
	}

	void map_capture(std::function<void(Type* t)> call) {
		for (size_t i = 0; i < list.size(); i++) call(list[i]);
	}

	Type* get(size_t index) {
		return list[index];
	}

	size_t size() { return list.size(); }
};
