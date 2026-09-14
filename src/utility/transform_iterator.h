/***************************************************************************
 *   Copyright (C) 2008-2026 by Andrzej Rybczak                            *
 *   andrzej@rybczak.net                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#ifndef NCMPCPP_UTILITY_TRANSFORM_ITERATOR_H
#define NCMPCPP_UTILITY_TRANSFORM_ITERATOR_H

#include <boost/iterator/iterator_adaptor.hpp>
#include <iterator>
#include <type_traits>

/// Like boost::transform_iterator, but operator[] returns the same reference
/// as *(it + n) instead of a proxy object. boost::iterator_facade returns a
/// proxy from operator[] for every non-POD value type. Template argument
/// deduction fails on the proxy, so a comparator with a templated operator()
/// doesn't compile with libc++ >= 22, which passes it[n] to the comparator in
/// std::sort, see https://github.com/llvm/llvm-project/issues/223290.
/// FunctionT must be default constructible and stateless.
template <typename FunctionT, typename IteratorT>
class TransformIterator : public boost::iterator_adaptor<
	TransformIterator<FunctionT, IteratorT>,
	IteratorT,
	std::remove_reference_t<
		std::invoke_result_t<const FunctionT &, typename std::iterator_traits<IteratorT>::reference>
	>,
	boost::use_default,
	std::invoke_result_t<const FunctionT &, typename std::iterator_traits<IteratorT>::reference>
>
{
	friend class boost::iterator_core_access;
	typedef typename TransformIterator::iterator_adaptor_ Base;

public:
	TransformIterator() { }
	explicit TransformIterator(IteratorT it) : Base(it) { }

	typename Base::reference operator[](typename Base::difference_type n) const {
		return *(*this + n);
	}

private:
	typename Base::reference dereference() const {
		return FunctionT()(*this->base());
	}
};

#endif // NCMPCPP_UTILITY_TRANSFORM_ITERATOR_H
