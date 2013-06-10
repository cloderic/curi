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

TEST_CASE("ParsePath/Success", "Valid pathes")
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
    settings.fragment_callback = fragment;

    URI uri;
    uri.clear();

    SECTION("Absolute", "")
    {
        const std::string pathStr("/foo/bar/baz");

        CHECK(curi_status_success == curi_parse_path_nt(pathStr.c_str(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "/foo/bar/baz");
        CHECK(uri.pathSegments.size() == 3);
        CHECK(uri.pathSegments[0] == "foo");
        CHECK(uri.pathSegments[1] == "bar");
        CHECK(uri.pathSegments[2] == "baz");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("Relative", "")
    {
        const std::string pathStr("foo/bar/baz");

        CHECK(curi_status_success == curi_parse_path_nt(pathStr.c_str(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "foo/bar/baz");
        CHECK(uri.pathSegments.size() == 3);
        CHECK(uri.pathSegments[0] == "foo");
        CHECK(uri.pathSegments[1] == "bar");
        CHECK(uri.pathSegments[2] == "baz");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("EmptySegment", "")
    {
        const std::string pathStr("/foo//bar/baz");

        CHECK(curi_status_success == curi_parse_path_nt(pathStr.c_str(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "/foo//bar/baz");
        CHECK(uri.pathSegments.size() == 3);
        CHECK(uri.pathSegments[0] == "foo");
        CHECK(uri.pathSegments[1] == "bar");
        CHECK(uri.pathSegments[2] == "baz");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("UrlDecode", "")
    {
        settings.url_decode = 1;

        const std::string pathStr("liz+taylor/is%20f%23%26ing%20very/rich");

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "liz taylor/is f#&ing very/rich");
        CHECK(uri.pathSegments.size() == 3);
        CHECK(uri.pathSegments[0] == "liz taylor");
        CHECK(uri.pathSegments[1] == "is f#&ing very");
        CHECK(uri.pathSegments[2] == "rich");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == sizeof(char)*(
            strlen("liz+taylor/is%20f%23%26ing%20very/rich") + 1 + 
            strlen("liz+taylor") + 1 + 
            strlen("is%20f%23%26ing%20very") + 1 + 
            strlen("rich") + 1));
        CHECK(uri.deallocatedMemory == uri.deallocatedMemory);

        settings.url_decode = 0;
    }

    uri.clear();
}

TEST_CASE("ParsePath/RetrieveOnlySegments", "Valid pathes")
{
    curi_settings settings;
    curi_default_settings(&settings);
    settings.allocate = test_allocate;
    settings.deallocate = test_deallocate;
    settings.path_segment_callback = pathSegment;

    URI uri;
    uri.clear();

    SECTION("UrlDecode", "")
    {
        settings.url_decode = 1;

        const std::string pathStr("liz+taylor/is%20f%23%26ing%20very/rich");

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, &uri));

        CHECK(uri.scheme.empty());
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path.empty());
        CHECK(uri.pathSegments.size() == 3);
        CHECK(uri.pathSegments[0] == "liz taylor");
        CHECK(uri.pathSegments[1] == "is f#&ing very");
        CHECK(uri.pathSegments[2] == "rich");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == sizeof(char)*(
            strlen("liz+taylor") + 1 + 
            strlen("is%20f%23%26ing%20very") + 1 + 
            strlen("rich") + 1));
        CHECK(uri.deallocatedMemory == uri.deallocatedMemory);

        settings.url_decode = 0;
    }

     uri.clear();
}

TEST_CASE("ParsePath/Cancelled", "Canceled parsing of path")
{
    const std::string pathStr("/foo/bar/baz");
    curi_settings settings;

    SECTION("Scheme", "")
    {
        curi_default_settings(&settings);
        settings.scheme_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Userinfo", "")
    {
        curi_default_settings(&settings);
        settings.userinfo_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Host", "")
    {
        curi_default_settings(&settings);
        settings.host_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("PortStr", "")
    {
        curi_default_settings(&settings);
        settings.portStr_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Port", "")
    {
        curi_default_settings(&settings);
        settings.port_callback = cancellingCallbackUint;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Path", "")
    {
        curi_default_settings(&settings);
        settings.path_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("PathSegment", "")
    {
        curi_default_settings(&settings);
        settings.path_segment_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Query", "")
    {
        curi_default_settings(&settings);
        settings.query_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("QueryItem", "")
    {
        curi_default_settings(&settings);
        settings.query_item_str_callback = cancellingCallbackTwoStr;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }

    SECTION("Fragment", "")
    {
        curi_default_settings(&settings);
        settings.fragment_callback = cancellingCallbackStr;

        CHECK(curi_status_success == curi_parse_path(pathStr.c_str(), pathStr.length(), &settings, 0));
    }
}

