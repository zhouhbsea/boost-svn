//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_UNIQUE_COPY_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_UNIQUE_COPY_HPP_INCLUDED

#include <boost/concept_check.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/concepts.hpp>
#include <algorithm>

namespace boost
{
    /// \brief template function unique_copy
    ///
    /// range-based version of the unique_copy std algorithm
    ///
    /// \pre SinglePassRange is a model of the SinglePassRangeConcept
    /// \pre OutputIterator is a model of the OutputIteratorConcept
    /// \pre BinaryPredicate is a model of the BinaryPredicateConcept
	template< class SinglePassRange, class OutputIterator >
	inline OutputIterator
    unique_copy( const SinglePassRange& rng, OutputIterator out_it )
	{
        boost::function_requires< SinglePassRangeConcept<SinglePassRange> >();
        return std::unique_copy(boost::begin(rng), boost::end(rng), out_it);
	}
    /// \overload
    template< class SinglePassRange, class OutputIterator, class BinaryPredicate >
    inline OutputIterator
    unique_copy( const SinglePassRange& rng, OutputIterator out_it,
                 BinaryPredicate pred )
    {
        boost::function_requires< SinglePassRangeConcept<SinglePassRange> >();
        return std::unique_copy(boost::begin(rng), boost::end(rng), out_it, pred);
    }
}

#endif // include guard
