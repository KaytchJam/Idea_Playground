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

	ObjectGroup& add(Type& item) {
		list.push_back(&item);
		return *this;
	}

	ObjectGroup& add(Type* ptr) {
		list.push_back(ptr);
		return *this;
	}

	ObjectGroup& add_all(std::initializer_list<Type*> data) {
		for (Type* item : data) {
			list.push_back(item);
		}

		return *this;
	}

	ObjectGroup& applyAll(void** args, size_t num_args, void (*call)(Type* t, size_t idx, void** func_args, size_t func_n_args)) {
		for (size_t i = 0; i < list.size(); i++) call(list[i], i, args, num_args);
		return *this;
	}

	ObjectGroup& map_mut(void (*call)(Type* t)) {
		for (size_t i = 0; i < list.size(); i++) call(list[i]);
		return *this;
	}

	ObjectGroup& map_capture(std::function<void(Type* t)> call) {
		for (size_t i = 0; i < list.size(); i++) call(list[i]);
		return *this;
	}

	Type* get(size_t index) {
		return list[index];
	}

	ObjectGroup& clear() {
		list.clear();
		return *this;
	}

	size_t size() { return list.size(); }
};
