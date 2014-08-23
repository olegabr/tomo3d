//grid2d.h
/*
* The 2D grid of some coords. It can be used to iterate through all its items
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef TOMO3D_GRID2D_H

#include "grid1d.h"
#include "point2d.h"

template <class ValueX, class ValueY>
class grid2d
{
public :
	typedef grid1d<ValueX> xgrid_type;
	typedef typename xgrid_type::value_type xvalue_type;

	typedef grid1d<ValueY> ygrid_type;
	typedef typename ygrid_type::value_type yvalue_type;

	typedef point2d_t<xvalue_type, yvalue_type> value_type;

	// iterate through the grid
	class const_iterator :
		public boost::iterator_facade<
			const_iterator,                     // Derived
			value_type,        // Value
			boost::forward_traversal_tag,       // CategoryOrTraversal
			value_type const&  // Reference
		>
	{
		typedef typename xgrid_type::const_iterator xconst_iterator;
		typedef typename ygrid_type::const_iterator yconst_iterator;

	public:
		// for begin
		const_iterator(
			xconst_iterator const& xbegin,
			xconst_iterator const& xend,
			yconst_iterator const& ybegin
		) :
			m_xbegin(xbegin),
			m_xend(xend),
			m_xcurr(xbegin),
			m_ybegin(ybegin) {}

		// for end
		const_iterator(yconst_iterator const& yend) :
			m_ybegin(yend) {}

	private:
		friend class boost::iterator_core_access;

		void increment() {
			if (m_xcurr == m_xend) {
				++m_ybegin;
				m_xcurr = m_xbegin;
			} else {
				++m_xcurr;
			}
		}

		value_type dereference() {
			return value_type(*m_xcurr, *m_ybegin);
		}

		bool equal(const_iterator const& ci) {
			return ci.m_ybegin == m_ybegin;
		}

		const xconst_iterator m_xbegin;
		const xconst_iterator m_xend;
		xconst_iterator m_xcurr;
		yconst_iterator m_ybegin;
	};

	grid2d(xgrid_type const& xgrid, ygrid_type const& ygrid) :
		m_xgrid(xgrid),
		m_ygrid(ygrid) {}

	const_iterator begin() const {
		return const_iterator(m_xgrid.begin(), m_xgrid.end(), m_ygrid.begin());
	}

	const_iterator end() const {
		return const_iterator(m_ygrid.end());
	}

	std::size_t size() const {
		return m_xgrid.size() * m_ygrid.size();
	}

private :
	xgrid_type m_xgrid;
	ygrid_type m_ygrid;
};

#define TOMO3D_GRID2D_H
#endif //TOMO3D_GRID2D_H
