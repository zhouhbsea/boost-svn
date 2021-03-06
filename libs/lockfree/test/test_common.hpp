//  Copyright (C) 2011 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include "test_helpers.hpp"

#include <boost/array.hpp>
#include <boost/thread.hpp>

namespace impl
{

using boost::array;
using namespace boost;
using namespace std;

using boost::lockfree::detail::atomic;

template <bool Bounded = false>
struct queue_stress_tester
{
    static const unsigned int buckets = 1<<13;
#ifndef BOOST_LOCKFREE_STRESS_TEST
    static const long node_count =  5000;
#else
    static const long node_count = 500000;
#endif
    const int reader_threads;
    const int writer_threads;

    static_hashed_set<long, buckets> data;
    static_hashed_set<long, buckets> dequeued;
    array<std::set<long>, buckets> returned;

    boost::lockfree::detail::atomic<int> push_count, pop_count;

    queue_stress_tester(int reader, int writer):
        reader_threads(reader), writer_threads(writer), push_count(0), pop_count(0)
    {}

    template <typename queue>
    void add_items(queue & stk)
    {
        for (long i = 0; i != node_count; ++i) {
            long id = generate_id<long>();

            bool inserted = data.insert(id);
            assert(inserted);

            if (Bounded)
                while(stk.bounded_push(id) == false)
                    /*thread::yield()*/;
            else
                while(stk.push(id) == false)
                    /*thread::yield()*/;
            ++push_count;
        }
    }

    atomic<bool> running;

    template <typename queue>
    void get_items(queue & stk)
    {
        for (;;) {
            long id;

            bool got = stk.pop(id);
            if (got) {
                bool erased = data.erase(id);
                bool inserted = dequeued.insert(id);
                assert(erased);
                assert(inserted);
                ++pop_count;
            } else
                if (!running.load())
                    return;
        }
    }

    template <typename queue>
    void run(queue & stk)
    {
        BOOST_WARN(stk.is_lock_free());

        running.store(true);

        thread_group writer;
        thread_group reader;

        BOOST_REQUIRE(stk.empty());

        for (int i = 0; i != reader_threads; ++i)
            reader.create_thread(boost::bind(&queue_stress_tester::template get_items<queue>, this, boost::ref(stk)));

        for (int i = 0; i != writer_threads; ++i)
            writer.create_thread(boost::bind(&queue_stress_tester::template add_items<queue>, this, boost::ref(stk)));

        using namespace std;
        cout << "threads created" << endl;

        writer.join_all();

        cout << "writer threads joined, waiting for readers" << endl;

        running = false;
        reader.join_all();

        cout << "reader threads joined" << endl;

        BOOST_REQUIRE_EQUAL(data.count_nodes(), 0);
        BOOST_REQUIRE(stk.empty());

        BOOST_REQUIRE_EQUAL(push_count, pop_count);
        BOOST_REQUIRE_EQUAL(push_count, writer_threads * node_count);
    }
};

}

using impl::queue_stress_tester;
