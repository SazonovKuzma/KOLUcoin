#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-koly");
    BOOST_CHECK(GetBoolArg("-koly"));
    BOOST_CHECK(GetBoolArg("-koly", false));
    BOOST_CHECK(GetBoolArg("-koly", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-kolyo"));
    BOOST_CHECK(!GetBoolArg("-kolyo", false));
    BOOST_CHECK(GetBoolArg("-kolyo", true));

    ResetArgs("-koly=0");
    BOOST_CHECK(!GetBoolArg("-koly"));
    BOOST_CHECK(!GetBoolArg("-koly", false));
    BOOST_CHECK(!GetBoolArg("-koly", true));

    ResetArgs("-koly=1");
    BOOST_CHECK(GetBoolArg("-koly"));
    BOOST_CHECK(GetBoolArg("-koly", false));
    BOOST_CHECK(GetBoolArg("-koly", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nokoly");
    BOOST_CHECK(!GetBoolArg("-koly"));
    BOOST_CHECK(!GetBoolArg("-koly", false));
    BOOST_CHECK(!GetBoolArg("-koly", true));

    ResetArgs("-nokoly=1");
    BOOST_CHECK(!GetBoolArg("-koly"));
    BOOST_CHECK(!GetBoolArg("-koly", false));
    BOOST_CHECK(!GetBoolArg("-koly", true));

    ResetArgs("-koly -nokoly");  // -koly should win
    BOOST_CHECK(GetBoolArg("-koly"));
    BOOST_CHECK(GetBoolArg("-koly", false));
    BOOST_CHECK(GetBoolArg("-koly", true));

    ResetArgs("-koly=1 -nokoly=1");  // -koly should win
    BOOST_CHECK(GetBoolArg("-koly"));
    BOOST_CHECK(GetBoolArg("-koly", false));
    BOOST_CHECK(GetBoolArg("-koly", true));

    ResetArgs("-koly=0 -nokoly=0");  // -koly should win
    BOOST_CHECK(!GetBoolArg("-koly"));
    BOOST_CHECK(!GetBoolArg("-koly", false));
    BOOST_CHECK(!GetBoolArg("-koly", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--koly=1");
    BOOST_CHECK(GetBoolArg("-koly"));
    BOOST_CHECK(GetBoolArg("-koly", false));
    BOOST_CHECK(GetBoolArg("-koly", true));

    ResetArgs("--nokoly=1");
    BOOST_CHECK(!GetBoolArg("-koly"));
    BOOST_CHECK(!GetBoolArg("-koly", false));
    BOOST_CHECK(!GetBoolArg("-koly", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-koly", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-koly", "eleven"), "eleven");

    ResetArgs("-koly -bar");
    BOOST_CHECK_EQUAL(GetArg("-koly", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-koly", "eleven"), "");

    ResetArgs("-koly=");
    BOOST_CHECK_EQUAL(GetArg("-koly", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-koly", "eleven"), "");

    ResetArgs("-koly=11");
    BOOST_CHECK_EQUAL(GetArg("-koly", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-koly", "eleven"), "11");

    ResetArgs("-koly=eleven");
    BOOST_CHECK_EQUAL(GetArg("-koly", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-koly", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-koly", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-koly", 0), 0);

    ResetArgs("-koly -bar");
    BOOST_CHECK_EQUAL(GetArg("-koly", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-koly=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-koly", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-koly=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-koly", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--koly");
    BOOST_CHECK_EQUAL(GetBoolArg("-koly"), true);

    ResetArgs("--koly=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-koly", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nokoly");
    BOOST_CHECK(!GetBoolArg("-koly"));
    BOOST_CHECK(!GetBoolArg("-koly", true));
    BOOST_CHECK(!GetBoolArg("-koly", false));

    ResetArgs("-nokoly=1");
    BOOST_CHECK(!GetBoolArg("-koly"));
    BOOST_CHECK(!GetBoolArg("-koly", true));
    BOOST_CHECK(!GetBoolArg("-koly", false));

    ResetArgs("-nokoly=0");
    BOOST_CHECK(GetBoolArg("-koly"));
    BOOST_CHECK(GetBoolArg("-koly", true));
    BOOST_CHECK(GetBoolArg("-koly", false));

    ResetArgs("-koly --nokoly");
    BOOST_CHECK(GetBoolArg("-koly"));

    ResetArgs("-nokoly -koly"); // koly always wins:
    BOOST_CHECK(GetBoolArg("-koly"));
}

BOOST_AUTO_TEST_SUITE_END()
