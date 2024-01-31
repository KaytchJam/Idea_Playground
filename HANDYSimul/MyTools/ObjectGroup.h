#pragma once
#include <vector>
#include <functional>

// vector wrapper that holds only pointers of some type 'Type', has a bunch of map and cast functions
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

	template <typename new_type> ObjectGroup& map_cast(std::function<void(new_type* nt)> call) {
		for (size_t i = 0; i < list.size(); i++) call((new_type*)list[i]);
		return *this;
	}

	template <typename as_new> ObjectGroup<as_new>* cast_inner() {
		ObjectGroup<as_new>* new_cast = (ObjectGroup<as_new>*) this;
		return new_cast;
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
