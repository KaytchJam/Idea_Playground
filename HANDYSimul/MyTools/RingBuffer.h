#pragma once
#include <stdlib.h>
#include <iostream>
#include <vector>

template <typename Type>
class RingBuffer {
private:
	unsigned int m_first = 0;
	unsigned int m_last = 0;
	unsigned int m_capacity;
	bool m_full;

	std::vector<Type> m_buffer;

	class rbIterator {
	public:
		unsigned int m_index = 0;
		RingBuffer* m_parent;
		Type m_data;
		rbIterator(unsigned int index, RingBuffer* parent) : m_index(index), m_parent(parent), m_data(parent->m_buffer[index]) {}
	
		rbIterator& operator++() {
			bool c1 = (m_index == m_parent->m_buffer.size() - 1 || m_index == m_parent->m_last);
			m_index = (m_index + 1) * !c1 + m_parent->m_first * c1;
			m_data = m_parent->m_buffer[m_index];
			return *this;
		}

		rbIterator& operator++(int) {
			bool c1 = (m_index == m_parent->m_buffer.size() - 1 || m_index == m_parent->m_last);
			m_index = (m_index + 1) * !c1 + m_parent->m_first * c1 * !m_parent->m_full;
			m_data = m_parent->m_buffer[m_index];
			return *this;
		}

		/*rbIterator& operator--(int) {
			m_index = (m_index - 1) * !(m_index > 0 || m_index == m_parent->m_last);
			m_data = m_parent->m_buffer[m_index];
			return *this;
		}*/

		bool operator==(rbIterator other) {
			return other.m_parent == this->m_parent && other.m_index == this->m_index;
		}

		bool operator!=(rbIterator other) {
			return !(*this == other);
		}

		float getData() { return m_data; }
	};

public:
	// Data structure of finite size that 'loops' on itself
	RingBuffer(unsigned int INITIAL_SIZE) {
		m_buffer = std::vector<Type>(INITIAL_SIZE);
		m_capacity = INITIAL_SIZE;
		m_full = false;
	}

	~RingBuffer() {}

	// Insert an element at the end of the buffer
	void insert(Type val) {
		m_buffer[m_last] = val;

		m_capacity = m_capacity - 1 * !m_full;
		m_full = m_capacity == 0;
		m_last = (m_last + 1) * !(m_last == m_buffer.size() - 1);
		m_first = (m_first + m_full) * !(m_first == m_buffer.size() - 1);
	}

	// Iterate from the first to last position in the ringbuffer, and apply some function "func"
	void stream(void* callback, void (*func)(Type, void*)) {
		int idx = m_first;
		while (idx != m_last) {
			func(m_buffer[idx], callback);
			idx = (idx + 1) * !(idx == m_buffer.size() - 1);
		}
	}

	// return the first element in the ringbuffer
	Type front() {
		return m_buffer[m_first];
	}

	// look at the last item in the ringbuffer
	Type peek() {
		bool c1 = m_last != 0;
		return m_buffer[m_last - 1 * c1 + (m_buffer.size() - 1) * !c1];
	}

	// reset the ringbuffer
	void reset() {
		m_capacity = (unsigned int) m_buffer.size();
		m_full = false;
		m_first = 0;
		m_last = 0;
	}


	// Return the total number of spaces in this buffer
	int size() { return (int) m_buffer.size(); }
	// Return the number of elements that can be inserted into this buffer before wrapping
	int capacity() { return m_capacity; }
	// Return whether the buffer is full or not
	bool full() { return m_full;  }

	typedef rbIterator iterator;

	rbIterator begin() {
		return rbIterator(m_first, this);
	}

	rbIterator end() {
		return rbIterator(m_last, this);
	}

};
