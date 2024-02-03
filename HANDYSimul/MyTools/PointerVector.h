#pragma once
#include <vector>
#include <functional>

// Vector wrapper that holds only pointers of some type 'Type', has a bunch of map and cast functions.
// Note that this data structure does NOT take ownership of the pointers it stores, so on deallocation
// the pointers are not deleted. Such an action will require manually calling 'delete items' or iterating
// through yourself.
template <typename Type>
class PointerVector {
	std::vector<Type*> list;
	bool has_ownership = false;

public:
	PointerVector<Type>() {}
	PointerVector<Type>(size_t size) { list = std::vector<Type*>(size); }
	PointerVector<Type>(std::vector<Type*> li) { list = li; }
	PointerVector<Type>(std::initializer_list<Type*> data) { list = std::vector<Type*>(data); }

	~PointerVector() { if (this->has_ownership) this->delete_all(); }

	// add a reference, its pointer is taken
	PointerVector& add(Type& item) {
		list.push_back(&item);
		return *this;
	}

	// add a pointer directly
	PointerVector& add(Type* ptr) {
		list.push_back(ptr);
		return *this;
	}

	// Determines whether this PointerVector has ownership of the pointers it contains or not.
	// If it does indeed have ownership, then upon PointerVector destruction, the pointers contained
	// within will be destroyed as well.
	PointerVector& set_ownership(bool owns) { 
		this->has_ownership = owns; 
		return *this; 
	}

	// add a bunch of item pointers via the initializer list
	PointerVector& add_all(std::initializer_list<Type*> data) {
		for (Type* item : data) list.push_back(item);
		return *this;
	}

	// c style map function, pass in values via the void ptr
	PointerVector& applyAll(void** args, size_t num_args, void (*call)(Type* t, size_t idx, void** func_args, size_t func_n_args)) {
		for (size_t i = 0; i < list.size(); i++) call(list[i], i, args, num_args);
		return *this;
	}

	// map function with no capturing
	PointerVector& map_mut(void (*call)(Type* t)) {
		for (size_t i = 0; i < list.size(); i++) call(list[i]);
		return *this;
	}

	// map function with capturing
	PointerVector& map_capture(std::function<void(Type* t)> call) {
		for (size_t i = 0; i < list.size(); i++) call(list[i]);
		return *this;
	}

	// iterate through the list and apply a function 'call' on every item within the list, casted to 'temp_type'
	template <typename temp_type> PointerVector& map_cast(std::function<void(temp_type* tt)> call) {
		for (size_t i = 0; i < list.size(); i++) call((temp_type*)list[i]);
		return *this;
	}

	// return a pointer to this object group
	template <typename as_new> PointerVector<as_new>& cast_inner() {
		return *(PointerVector<as_new>*) this;
	}

	// get an item at a valid index in the list
	Type* get(size_t index) {
		return list[index];
	}

	// simply clears the list without deleting the pointers
	PointerVector& empty() {
		list.clear();
		return *this;
	}

	//  deletes all pointers stored in the Object Group, but note that it does not clear the list
	PointerVector& delete_all() {
		for (size_t i = 0; i < list.size(); i++) delete list[i];
		return *this;
	}

	// deletes the pointers in the list and clears it, equivalent to calling this->delete_all().empty();
	PointerVector& delete_all_and_empty() {
		for (size_t i = 0; i < list.size(); i++) delete list[i];
		list.clear();
		return *this;
	}

	// get an item at a valid index in the list
	/*Type*& operator[](int index) {
		return list[index];
	}*/

	// returns the size of the list
	size_t size() { return list.size(); }
};
