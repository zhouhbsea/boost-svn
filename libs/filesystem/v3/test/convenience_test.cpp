//  libs/filesystem/test/convenience_test.cpp  -----------------------------------------//

//  Copyright Beman Dawes, 2002
//  Copyright Vladimir Prus, 2002
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

#define BOOST_FILESYSTEM_VERSION 3

#include <boost/config/warning_disable.hpp>

//  See deprecated_test for tests of deprecated features
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;
using fs::path;
namespace sys = boost::system;

#include <boost/detail/lightweight_test.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <iostream>

namespace
{
  template< typename F >
    bool throws_fs_error(F func)
  {
    try { func(); }

    catch (const fs::filesystem_error &)
    {
      return true;
    }
    return false;
  }

    void create_recursive_iterator(const fs::path & ph)
    {
      fs::recursive_directory_iterator it(ph);
    }
}

//  ------------------------------------------------------------------------------------//

int main(int, char*[])
{

//  create_directories() tests  --------------------------------------------------------//

  BOOST_TEST(!fs::create_directories(""));  // should be harmless
  BOOST_TEST(!fs::create_directories("/")); // ditto

  fs::remove_all("xx");  // make sure slate is blank
  BOOST_TEST(!fs::exists("xx")); // reality check

  BOOST_TEST(fs::create_directories("xx"));
  BOOST_TEST(fs::exists("xx"));
  BOOST_TEST(fs::is_directory("xx"));

  BOOST_TEST(fs::create_directories("xx/yy/zz"));
  BOOST_TEST(fs::exists("xx"));
  BOOST_TEST(fs::exists("xx/yy"));
  BOOST_TEST(fs::exists("xx/yy/zz"));
  BOOST_TEST(fs::is_directory("xx"));
  BOOST_TEST(fs::is_directory("xx/yy"));
  BOOST_TEST(fs::is_directory("xx/yy/zz"));

  path is_a_file("xx/uu");
  {
    std::ofstream f(is_a_file.string().c_str());
    BOOST_TEST(!!f);
  }
  BOOST_TEST(throws_fs_error(
    boost::bind(fs::create_directories, is_a_file)));
  BOOST_TEST(throws_fs_error(
    boost::bind(fs::create_directories, is_a_file / "aa")));

// recursive_directory_iterator tests ----------------------------------------//

  sys::error_code ec;
  fs::recursive_directory_iterator it("/no-such-path", ec);
  BOOST_TEST(ec);

  BOOST_TEST(throws_fs_error(
    boost::bind(create_recursive_iterator, "/no-such-path")));

  fs::remove("xx/uu");

#ifdef BOOST_WINDOWS_API
  // These tests depends on ordering of directory entries, and that's guaranteed
  // on Windows but not necessarily on other operating systems
  {
    std::ofstream f("xx/yya");
    BOOST_TEST(!!f);
  }

  for (it = fs::recursive_directory_iterator("xx");
        it != fs::recursive_directory_iterator(); ++it)
    { std::cout << it->path() << '\n'; }

  it = fs::recursive_directory_iterator("xx");
  BOOST_TEST(it->path() == "xx/yy");
  BOOST_TEST(it.level() == 0);
  ++it;
  BOOST_TEST(it->path() == "xx/yy/zz");
  BOOST_TEST(it.level() == 1);
  it.pop();
  BOOST_TEST(it->path() == "xx/yya");
  BOOST_TEST(it.level() == 0);
  it++;
  BOOST_TEST(it == fs::recursive_directory_iterator());

  it = fs::recursive_directory_iterator("xx");
  BOOST_TEST(it->path() == "xx/yy");
  it.no_push();
  ++it;
  BOOST_TEST(it->path() == "xx/yya");
  ++it;
  BOOST_TEST(it == fs::recursive_directory_iterator());

  fs::remove("xx/yya");
#endif

  it = fs::recursive_directory_iterator("xx/yy/zz");
  BOOST_TEST(it == fs::recursive_directory_iterator());
  
  it = fs::recursive_directory_iterator("xx");
  BOOST_TEST(it->path() == "xx/yy");
  BOOST_TEST(it.level() == 0);
  ++it;
  BOOST_TEST(it->path() == "xx/yy/zz");
  BOOST_TEST(it.level() == 1);
  it++;
  BOOST_TEST(it == fs::recursive_directory_iterator());

  it = fs::recursive_directory_iterator("xx");
  BOOST_TEST(it->path() == "xx/yy");
  it.no_push();
  ++it;
  BOOST_TEST(it == fs::recursive_directory_iterator());

  it = fs::recursive_directory_iterator("xx");
  BOOST_TEST(it->path() == "xx/yy");
  ++it;
  it.pop();
  BOOST_TEST(it == fs::recursive_directory_iterator());

  ec.clear();
  BOOST_TEST(!ec);
  // check that two argument failed constructor creates the end iterator 
  BOOST_TEST(fs::recursive_directory_iterator("nosuchdir", ec)
    == fs::recursive_directory_iterator());
  BOOST_TEST(ec);

  return ::boost::report_errors();
}
