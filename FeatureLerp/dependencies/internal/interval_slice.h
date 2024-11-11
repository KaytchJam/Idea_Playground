#pragma once
#include <ranges>
#include <vector>
#include <iostream>
#include <sstream>

struct numeric_range {
	size_t m_start;
	size_t m_end; // exclusive

	size_t min() const {
		return this->m_start;
	}

	size_t max() const {
		return this->m_end;
	}

	bool in_range(size_t s) const {
		return s > this->m_start && s < this->m_end;
	}

	// Returns the size of the range, m_end - m_start
	size_t size() const {
		return m_end - m_start;
	}

	numeric_range(size_t end) : m_start(0), m_end(end) {}
	numeric_range(size_t start, size_t end) : m_start(start), m_end(end) {}

	struct numeric_range_iterator {
	private:
		size_t m_at = 0;
		size_t m_end = 0;
	public:
		using Category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using Pointer = size_t;
		using value_type = size_t;
		using Reference = size_t; // fake reference, we just copy the value

		numeric_range_iterator() {}
		numeric_range_iterator(size_t start, size_t end) : m_at(start), m_end(end) {}

		Reference operator*() const {
			return this->m_at;
		}

		value_type operator->() const {
			return this->m_at;
		}

		numeric_range_iterator& operator++() {
			++(this->m_at);
			return *this;
		}

		numeric_range_iterator operator++(int) {
			numeric_range_iterator temp = *this;
			++(*this);
			return temp;
		}

		numeric_range_iterator& operator--() {
			--(this->m_at);
			return *this;
		}

		numeric_range_iterator operator--(int) {
			numeric_range_iterator temp = *this;
			--(*this);
			return temp;
		}

		friend bool operator<(const numeric_range_iterator& it1, const numeric_range_iterator& it2) {
			return it1.m_at < it2.m_at;
		}

		friend bool operator==(const numeric_range_iterator& nrit1, const numeric_range_iterator& nrit2) {
			return nrit1.m_at == nrit2.m_at;
		}

		friend bool operator!=(const numeric_range_iterator& nrit1, const numeric_range_iterator& nrit2) {
			return nrit1.m_at != nrit2.m_at;
		}
	};

	typedef numeric_range_iterator iterator;

	iterator begin() const {
		return numeric_range_iterator(this->m_start, this->m_end);
	}

	iterator end() const {
		return numeric_range_iterator(this->m_end, this->m_end);
	}

	friend std::ostream& operator<<(std::ostream& os, const numeric_range& nr) {
		return os << "(" << nr.min() << ", " << nr.max() << ")";
	}
};

template<typename T>
class NonContiguousSlice {
private:
	std::span<T> m_buffer;
	std::vector<numeric_range> m_intervals;
	size_t m_total_indices = 0;
public:
	NonContiguousSlice() {}

	NonContiguousSlice(std::span<T> view) : m_buffer(view), m_total_indices(view.size()) {
		this->m_intervals = view.size() > 0
			? std::vector<numeric_range>(1, numeric_range(0, view.size()))
			: std::vector<numeric_range>();
	}

	/*NonContiguousSlice(std::span<T> view, size_t start, size_t end) : m_buffer(view) {
		this->m_intervals = view.size() > 0
			? std::vector<numeric_range>(1, numeric_range())
	}*/

	size_t size() const {
		return m_total_indices;
	}


	size_t max_index() const {
		return this->m_intervals[this->m_intervals.size() - 1].max();
	}

	size_t min_index() const {
		return this->m_intervals[0].min();
	}

	size_t total_intervals() const {
		return this->m_intervals.size();
	}

	bool in_range(const size_t index) const {
		std::vector<numeric_range>::const_iterator iter = std::ranges::lower_bound(
			this->m_intervals, index, std::ranges::less(), &numeric_range::max
		);

		return iter != this->m_intervals.end() || iter->in_range(index);
	}

	T& top() {
		return this->m_buffer[this->m_intervals[0].min()];
	}

	T& back() {
		return this->m_buffer[this->m_intervals[this->m_intervals.size() - 1].max() - 1];
	}


	struct NCSliceIterator {
	private:
		size_t m_interval_index = 0;
		NonContiguousSlice<T>* m_parent;
		numeric_range::iterator m_nr_iter;

		numeric_range::iterator current_interval_begin() const {
			return this->m_parent->m_intervals[this->m_interval_index].begin();
		}

		numeric_range::iterator current_interval_end() const {
			return this->m_parent->m_intervals[this->m_interval_index].end();
		}

	public:
		using Category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using Pointer = T*;
		using value_type = std::pair<size_t, T>;
		using Reference = std::pair<size_t, T&>;

		// Construct an empty NCSliceIterator
		NCSliceIterator() : m_parent(nullptr), m_interval_index(0), m_nr_iter(numeric_range::iterator()) {}

		NCSliceIterator(NonContiguousSlice<T>* parent) 
			: m_parent(parent), m_interval_index(0), m_nr_iter(this->current_interval_begin()) {}

		NCSliceIterator(NonContiguousSlice<T>* parent, size_t interval) 
			: m_parent(parent), m_interval_index(interval), m_nr_iter(this->current_interval_begin()) {}

		NCSliceIterator(NonContiguousSlice<T>* parent, size_t start_interval, numeric_range::iterator iter) 
			: m_parent(parent), m_interval_index(start_interval), m_nr_iter(iter) {}

		size_t current_interval() const {
			return this->m_interval_index;
		}

		Reference operator*() const {
			return std::make_pair(
				*this->m_nr_iter, 
				std::ref(this->m_parent->m_buffer[*(this->m_nr_iter)]) 
			);
		}

		Pointer operator->() const {
			return &(this->m_parent->m_buffer[*(this->m_nr_iter)]);
		}

		NCSliceIterator& operator++() {
			++(this->m_nr_iter);
			if (this->m_nr_iter == this->current_interval_end()) {
				++this->m_interval_index;
				if (this->m_interval_index < this->m_parent->total_intervals()) {
					this->m_nr_iter = this->current_interval_begin();
				}
			}

			return *this;
		}

		NCSliceIterator operator++(int) {
			NCSliceIterator temp = *this;
			++(*this);
			return temp;
		}

		NCSliceIterator& operator--() {
			--(this->m_nr_iter);
			if (this->m_nr_iter < this->current_interval_begin()) {
				if (this->m_interval_index > 0) {
					--this->m_interval_index;
					this->m_nr_iter = --this->current_interval_end();
				}
			}

			return *this;
		}

		NCSliceIterator operator--(int) {
			NCSliceIterator temp = *this;
			--(*this);
			return temp;
		}

		friend bool operator==(const NCSliceIterator& ssit1, const NCSliceIterator& ssit2) {
			return ssit1.m_parent == ssit2.m_parent 
				&& ((ssit1.m_interval_index >= ssit1.m_parent->total_intervals() && ssit2.m_interval_index >= ssit2.m_parent->total_intervals())
				|| (ssit1.m_interval_index == ssit2.m_interval_index && ssit1.m_nr_iter == ssit2.m_nr_iter));
		}

		friend bool operator!=(const NCSliceIterator& ssit1, const NCSliceIterator& ssit2) {
			return !(ssit1 == ssit2);
		}
	};

	typedef NCSliceIterator iterator;

	iterator begin() {
		return this->m_intervals.size() > 0
			? NCSliceIterator(this, 0, this->m_intervals[0].begin())
			: NCSliceIterator(this, 0, numeric_range::iterator());
	}

	iterator end() {
		return NCSliceIterator(this, this->m_intervals.size(), numeric_range::iterator());
	}

	iterator rbegin() {
		return this->m_intervals.size() > 0
			? NCSliceIterator(this, this->m_intervals.size() - 1, --this->m_intervals[this->m_intervals.size() - 1].end())
			: NCSliceIterator(this, 0, numeric_range::iterator());
	}

	NonContiguousSlice<T>::iterator remove(NonContiguousSlice<T>::iterator&& iter) {
		NonContiguousSlice<T>::iterator it = this->end();
		const size_t index = (*iter).first;

		if (iter != this->end()) {
			const size_t target_idx = iter.current_interval();
			const numeric_range nr = this->m_intervals[target_idx];

			// could do some branchless checks instead for the (index == nr.max() - 1 || index == nr.min())
			// anything outside these conditions means you are out of range
			if (index == nr.min() && index == nr.max() - 1) {
				this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				this->m_total_indices--;

				if (target_idx < this->m_intervals.size()) {
					it = NCSliceIterator(this, target_idx);
				}

			} else if (index == nr.min()) {
				std::vector<numeric_range>::iterator after = this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				after = this->m_intervals.insert(after, numeric_range(index + 1, nr.max()));
				this->m_total_indices--;
				it = NCSliceIterator(this, target_idx);

			} else if (index == nr.max() - 1) {
				std::vector<numeric_range>::iterator after = this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				after = this->m_intervals.insert(after, numeric_range(nr.min(), index));
				this->m_total_indices--;

				if (target_idx + 1 < this->m_intervals.size()) {
					it = NCSliceIterator(this, target_idx + 1);
				}

			} else if (index > nr.min()) {
				std::vector<numeric_range>::iterator after = this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				after = this->m_intervals.insert(after, numeric_range(nr.min(), index));
				this->m_intervals.insert(after + 1, numeric_range(index + 1, nr.max()));
				this->m_total_indices--;
				it = NCSliceIterator(this, target_idx + 1);
			}
		}

		return it;
	}

	// remove an index from the range
	NonContiguousSlice<T>::iterator remove(size_t index) {
		// binary search to find target range
		std::vector<numeric_range>::iterator iter = std::ranges::upper_bound(
			this->m_intervals, index, std::ranges::less(), &numeric_range::max
		);

		NonContiguousSlice<T>::iterator it = this->end();
		// final checks to determine if we can split an existing range into two
		// we know by definition of upper_bound that the input index is less than iter->max()
		if (iter != this->m_intervals.end()) {
			const size_t target_idx = std::distance(this->m_intervals.begin(), iter);
			const numeric_range nr = this->m_intervals[target_idx];

			// could do some branchless checks instead for the (index == nr.max() - 1 || index == nr.min())
			// anything outside these conditions means you are out of range
			if (index == nr.min() && index == nr.max() - 1) {
				this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				this->m_total_indices--;

				if (target_idx < this->m_intervals.size()) {
					it = NCSliceIterator(this, target_idx);
				}

			} else if (index == nr.min()) {
				std::vector<numeric_range>::iterator after = this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				after = this->m_intervals.insert(after, numeric_range(index + 1, nr.max()));
				this->m_total_indices--;
				it = NCSliceIterator(this, target_idx);

			} else if (index == nr.max() - 1) {
				std::vector<numeric_range>::iterator after = this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				after = this->m_intervals.insert(after, numeric_range(nr.min(), index));
				this->m_total_indices--;

				if (target_idx + 1 < this->m_intervals.size()) {
					it = NCSliceIterator(this, target_idx + 1);
				}

			} else if (index > nr.min()) {
				std::vector<numeric_range>::iterator after = this->m_intervals.erase(this->m_intervals.begin() + target_idx);
				after = this->m_intervals.insert(after, numeric_range(nr.min(), index));
				this->m_intervals.insert(after + 1, numeric_range(index + 1, nr.max()));
				this->m_total_indices--;
				it = NCSliceIterator(this, target_idx + 1);
			}
		}

		return it;
	}

	NonContiguousSlice& remove_all(const std::initializer_list<size_t> const& indices) {
		for (size_t i : indices) this->remove(i);
		return *this;
	}

	NonContiguousSlice& remove_if(std::function<bool(const T&)> pred) {
		for (NonContiguousSlice<T>::iterator it = this->begin(); it != this->end();) {
			if (pred((*it).second)) it = this->remove((*it).first);
			else it++;
		}

		return *this;
	}

	void print_intervals() {
		for (const numeric_range& nr : this->m_intervals) {
			std::cout << nr << " ";
		}
		std::cout << std::endl;
	}

	std::string ranges_to_string() const {
		std::stringstream ss;
		std::vector<numeric_range>::const_iterator it = this->m_intervals.begin();

		if (it != this->m_intervals.end()) ss << "(" << it->min() << ", " << (it++)->max() << ")";

		while (it != this->m_intervals.end()) {
			ss << ", (" << it->min() << ", " << it->max() << ")";
			++it;
		}

		return ss.str();
	}

	// O(N) indexing operation is CRAZY brother
	/*T& operator[](const size_t index) {
		assert(index < this->size() && "Index out of range");

		const size_t num_intervals = this->m_intervals.size();
		size_t interval_idx = 0;
		size_t size_total = this->m_intervals[interval_idx].size();

		while (index < size_total) {
			interval_idx++;

		}
	}*/

	// NonContiguousSlice& remove_range(numeric_range r) {
	// std::vector<numeric_range>::iterator iter = std::ranges::lower_bound(
	//		this->m_intervals,
	//		std::ranges::less(),
	//		[](const numeric_range& nr) -> size_t { return nr.max(); }
	// 
	//	// Given this iterator, we loop till we hit the first element with a minimum bound greater than our max
	//	// During our loop we "cut off" parts from every range we meet on the way
	// }

	/*NonContiguousSlice& reset() {
		this->m_intervals = std::vector<numeric_range>(1, numeric_range(0, this->m_buffer.size()));
	}*/

};

template<typename T>
using NCSlice = NonContiguousSlice<T>;

static_assert(std::forward_iterator<numeric_range::iterator>);
static_assert(std::ranges::forward_range<numeric_range>);
static_assert(std::incrementable<numeric_range::iterator>);
static_assert(std::sentinel_for<numeric_range::iterator, numeric_range::iterator>);
static_assert(std::bidirectional_iterator<numeric_range::iterator>);
//static_assert(std::totally_ordered<numeric_range::iterator>);

static_assert(std::input_iterator<NCSlice<int>::iterator>);
static_assert(std::indirectly_readable<NCSlice<int>::iterator>);
static_assert(std::incrementable<NCSlice<int>::iterator>);
static_assert(std::sentinel_for<NCSlice<int>::iterator, NCSlice<int>::iterator>);

static_assert(std::regular<NCSlice<int>::iterator>);
static_assert(std::semiregular<NCSlice<int>::iterator>);
static_assert(std::equality_comparable<NCSlice<int>::iterator>);

static_assert(std::forward_iterator<NCSlice<int>::iterator>);
static_assert(std::ranges::forward_range<NonContiguousSlice<int>>);
static_assert(std::bidirectional_iterator<NCSlice<int>::iterator>);
