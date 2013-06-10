// Copyright (c) 2013 Clodéric Mars

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Common.h"

#include <curi.h>

#include <cstring>

TEST_CASE("ParseQuery/Success", "Valid pathes")
{
    curi_settings settings;
    curi_default_settings(&settings);
    settings.allocate = test_allocate;
    settings.deallocate = test_deallocate;
    settings.scheme_callback = scheme;
    settings.userinfo_callback = userinfo;
    settings.host_callback = host;
    settings.port_callback = port;
    settings.portStr_callback = portStr;
    settings.path_callback = path;
    settings.path_segment_callback = pathSegment;
    settings.query_callback = query;
    settings.query_item_null_callback = queryNullItem;
    settings.query_item_int_callback = queryIntItem;
    settings.query_item_double_callback = queryDoubleItem;
    settings.query_item_str_callback = queryStrItem;
    settings.fragment_callback = fragment;

    URI uri;
    uri.clear();

    SECTION("OnlyKeys", "")
    {
        const std::string pathStr("foo&bar&baz");

        CHECK(curi_status_success == curi_parse_query_nt(pathStr.c_str(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path.empty());
        CHECK(uri.pathSegments.empty());
        CHECK(uri.query == "foo&bar&baz");
        CHECK(uri.queryNullItems.size() == 3);
        CHECK(uri.queryNullItems.find("foo") != uri.queryNullItems.end());
        CHECK(uri.queryNullItems.find("bar") != uri.queryNullItems.end());
        CHECK(uri.queryNullItems.find("baz") != uri.queryNullItems.end());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    SECTION("KeyValues", "")
    {
        const std::string pathStr("foo=2&bar&baz=bazValue&foobar=.5");

        CHECK(curi_status_success == curi_parse_query_nt(pathStr.c_str(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path.empty());
        CHECK(uri.pathSegments.empty());
        CHECK(uri.query == "foo=2&bar&baz=bazValue&foobar=.5");
        CHECK(uri.queryNullItems.size() == 1);
        CHECK(*uri.queryNullItems.begin() == "bar");
        CHECK(uri.queryStrItems.size() == 1);
        CHECK(uri.queryStrItems["baz"] == "bazValue");
        CHECK(uri.queryIntItems.size() == 1);
        CHECK(uri.queryIntItems["foo"] == 2);
        CHECK(uri.queryDoubleItems.size() == 1);
        CHECK(uri.queryDoubleItems["foobar"] == .5);
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();
}

TEST_CASE("ParseQuery/Cancelled", "Canceled parsing of path")
{
    const std::string queryStr("foo=1&bar=bar&baz=3.0&foobar");
    curi_settings settings;

    SECTION("Scheme", "")
    {
        curi_default_settings(&settings);
        settings.scheme_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("Userinfo", "")
    {
        curi_default_settings(&settings);
        settings.userinfo_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("Host", "")
    {
        curi_default_settings(&settings);
        settings.host_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("PortStr", "")
    {
        curi_default_settings(&settings);
        settings.portStr_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("Port", "")
    {
        curi_default_settings(&settings);
        settings.port_callback = cancellingCallbackUint;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("Path", "")
    {
        curi_default_settings(&settings);
        settings.path_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("PathSegment", "")
    {
        curi_default_settings(&settings);
        settings.path_segment_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("Query", "")
    {
        curi_default_settings(&settings);
        settings.query_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }

    SECTION("QueryNullItem", "")
    {
        curi_default_settings(&settings);
        settings.query_item_null_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));

        const std::string noNullItemQuery("foo=1&bar=bar&baz=3.0");
        CHECK(curi_status_success == curi_parse_query(noNullItemQuery.c_str(), noNullItemQuery.length(), &settings, 0));
    }

    SECTION("QueryIntItem", "")
    {
        curi_default_settings(&settings);
        settings.query_item_int_callback = cancellingCallbackStrLongInt;

        CHECK(curi_status_canceled == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));

        const std::string noIntItemQuery("bar=bar&baz=3.0&foobar");
        CHECK(curi_status_success == curi_parse_query(noIntItemQuery.c_str(), noIntItemQuery.length(), &settings, 0));
    }

    SECTION("QueryDoubleItem", "")
    {
        curi_default_settings(&settings);
        settings.query_item_double_callback = cancellingCallbackStrDouble;

        CHECK(curi_status_canceled == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));

        const std::string intItemQuery("baz=3");
        CHECK(curi_status_canceled == curi_parse_query(intItemQuery.c_str(), intItemQuery.length(), &settings, 0));

        const std::string noNumberItemQuery("bar=bar&baz=baz&foobar");
        CHECK(curi_status_success == curi_parse_query(noNumberItemQuery.c_str(), noNumberItemQuery.length(), &settings, 0));
    }

    SECTION("QueryStrItem", "")
    {
        curi_default_settings(&settings);
        settings.query_item_str_callback = cancellingCallbackTwoStr;

        CHECK(curi_status_canceled == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));

        const std::string noStringItemQuery("bar=3&baz=3.5&foobar");
        CHECK(curi_status_canceled == curi_parse_query(noStringItemQuery.c_str(), noStringItemQuery.length(), &settings, 0));
    }

    SECTION("Fragment", "")
    {
        curi_default_settings(&settings);
        settings.fragment_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(queryStr.c_str(), queryStr.length(), &settings, 0));
    }
}

