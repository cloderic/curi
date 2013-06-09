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
    settings.query_item_callback = queryItem;
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
        CHECK(uri.queryItems.size() == 3);
        CHECK(uri.queryItems.find("foo") != uri.queryItems.end());
        CHECK(uri.queryItems.find("bar") != uri.queryItems.end());
        CHECK(uri.queryItems.find("baz") != uri.queryItems.end());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    SECTION("KeyValues", "")
    {
        const std::string pathStr("foo=fooValue&bar=barValue&baz=bazValue");

        CHECK(curi_status_success == curi_parse_query_nt(pathStr.c_str(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path.empty());
        CHECK(uri.pathSegments.empty());
        CHECK(uri.query == "foo=fooValue&bar=barValue&baz=bazValue");
        CHECK(uri.queryItems.size() == 3);
        CHECK(uri.queryItems["foo"] == "fooValue");
        CHECK(uri.queryItems["bar"] == "barValue");
        CHECK(uri.queryItems["baz"] == "bazValue");
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();
}

TEST_CASE("ParseQuery/Cancelled", "Canceled parsing of path")
{
    const std::string pathStr("foo=1&bar=2&baz=3");
    curi_settings settings;

    SECTION("Scheme", "")
    {
        curi_default_settings(&settings);
        settings.scheme_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Userinfo", "")
    {
        curi_default_settings(&settings);
        settings.userinfo_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Host", "")
    {
        curi_default_settings(&settings);
        settings.host_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("PortStr", "")
    {
        curi_default_settings(&settings);
        settings.portStr_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Port", "")
    {
        curi_default_settings(&settings);
        settings.port_callback = cancellingCallbackUint;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Path", "")
    {
        curi_default_settings(&settings);
        settings.path_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("PathSegment", "")
    {
        curi_default_settings(&settings);
        settings.path_segment_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Query", "")
    {
        curi_default_settings(&settings);
        settings.query_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("QueryItem", "")
    {
        curi_default_settings(&settings);
        settings.query_item_callback = cancellingCallbackTwoStr;

        CHECK(curi_status_canceled == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Fragment", "")
    {
        curi_default_settings(&settings);
        settings.fragment_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_query(pathStr.c_str(), pathStr.length(), &settings, 0));
    }
}

