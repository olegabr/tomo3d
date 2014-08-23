//grid1d.h
/*
* The 1D grid of some coords. It can be used to iterate through all its items
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef TOMO3D_GRID1D_H

#include <boost/iterator/iterator_facade.hpp>
#include <algorithm>
#include <cmath>

template <class Value>
class grid1d
{
public :
	typedef Value value_type;

	// iterate through the grid
	class const_iterator :
		public boost::iterator_facade<
			const_iterator,                     // Derived
			Value,                              // Value
			boost::random_access_traversal_tag, // CategoryOrTraversal
			Value const&                        // Reference
		>
	{
	public:
		// for begin
		const_iterator(Value start, Value step) :
			m_curr(start),
			m_step(step) {}

		// for end
		explicit const_iterator(Value end) :
			m_curr(end),
			m_step(0) {}

	private:
		friend class boost::iterator_core_access;

		void increment() { m_curr += m_step; }
		void decrement() { m_curr -= m_step; }

		Value dereference() { return m_curr; }

		void advance(std::size_t n) { m_curr += n*m_step; }
		difference_type distance_to(const_iterator const& ci) {
			return
				static_cast<difference_type>(
					std::floor(
						(ci.m_curr - m_curr) / m_step
					)
				);
		}

		bool equal(const_iterator const& ci) {
			return std::abs<Value>(ci.m_curr - m_curr) < m_step;
		}

		Value m_curr;
		Value m_step;
	};

	grid1d(value_type const& start, value_type const& end, value_type const& step) :
		m_start(start),
		m_end(end),
		m_step(step) {}

	const_iterator begin() const {
		return const_iterator(m_start, m_step);
	}

	const_iterator end() const {
		return const_iterator(m_end);
	}

	std::size_t size() const {
		return
			static_cast<std::size_t>(
				std::floor(
					(m_end - m_start) / m_step
				)
			);
	}

private :
	value_type m_start;
	value_type m_end;
	value_type m_step;
};

#define TOMO3D_GRID1D_H
#endif //TOMO3D_GRID1D_H
