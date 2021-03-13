#ifndef CINDEX
#define CINDEX

// This is a circular Index, created with a maximum value once reached it'll loop
// back to it's lowest value (1). For use when you want to loop through a vector
// from a non-zero starting position
class cIndex {
public:
	// max is inclusive
	cIndex() : m_max(6), m_index(1) {};
	cIndex(unsigned int i, unsigned int max) : m_max(max) {
		if (i > m_max) m_index = m_max;
		if (i < 1) m_index = 1;
		m_index = i;
	};

	cIndex operator-(const int& i) const {
		cIndex returnVal(m_index, m_max);
		for (int k = 0; k < i; ++k) {
			returnVal--;
		}
		return returnVal;
	}

	cIndex operator+(const int& i) const {
		cIndex returnVal(m_index, m_max);
		for (int k = 0; k < i; ++k) {
			returnVal++;
		}
		return returnVal;
	}

	cIndex& operator++() {
		m_index == m_max ? m_index = 1 : m_index++;
		return *this;
	}
	cIndex operator++(int) {
		cIndex temp = *this;
		++*this;
		return temp;
	}

	cIndex& operator--() {
		m_index == 1 ? m_index = m_max : m_index--;
		return *this;
	}
	cIndex operator--(int) {
		cIndex temp = *this;
		++*this;
		return temp;
	}

	int getIndex() const { return m_index - 1; };
private:

	unsigned int m_max;
	unsigned int m_index;
};

#endif
