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

TEST_CASE("ParseFullUri/Success/Full", "Valid full URIs")
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

    SECTION("Simple", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");

        CHECK(curi_status_success == curi_parse_full_uri_nt(uriStr.c_str(), &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.portStr == "8042");
        CHECK(uri.port == 8042);
        CHECK(uri.path == "/over/there");
        CHECK(uri.pathSegments.size() == 2);
        CHECK(uri.pathSegments[0] == "over");
        CHECK(uri.pathSegments[1] == "there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.queryItems.size() == 1);
        CHECK(uri.queryItems.begin()->first == "name");
        CHECK(uri.queryItems.begin()->second == "ferret");
        CHECK(uri.fragment == "nose");
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("Simple_Len", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.portStr == "8042");
        CHECK(uri.port == 8042);
        CHECK(uri.path == "/over/there");
        CHECK(uri.pathSegments.size() == 2);
        CHECK(uri.pathSegments[0] == "over");
        CHECK(uri.pathSegments[1] == "there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.queryItems.size() == 1);
        CHECK(uri.queryItems.begin()->first == "name");
        CHECK(uri.queryItems.begin()->second == "ferret");
        CHECK(uri.fragment == "nose");
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("Simple_BadLen", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length() * 100, &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.portStr == "8042");
        CHECK(uri.port == 8042);
        CHECK(uri.path == "/over/there");
        CHECK(uri.pathSegments.size() == 2);
        CHECK(uri.pathSegments[0] == "over");
        CHECK(uri.pathSegments[1] == "there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.queryItems.size() == 1);
        CHECK(uri.queryItems.begin()->first == "name");
        CHECK(uri.queryItems.begin()->second == "ferret");
        CHECK(uri.fragment == "nose");
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("Simple_NotNullTerminated", "")
    {
        const char* uriStr("foo://bar@example.com:8042/over/there?name=ferret#nosenoisynoisewedontcareabout");
        size_t actualLen = strlen("foo://bar@example.com:8042/over/there?name=ferret#nose");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr, actualLen, &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.portStr == "8042");
        CHECK(uri.port == 8042);
        CHECK(uri.path == "/over/there");
        CHECK(uri.pathSegments.size() == 2);
        CHECK(uri.pathSegments.size() == 2);
        CHECK(uri.pathSegments[0] == "over");
        CHECK(uri.pathSegments[1] == "there");
        CHECK(uri.pathSegments[1] == "there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.queryItems.size() == 1);
        CHECK(uri.queryItems.begin()->first == "name");
        CHECK(uri.queryItems.begin()->second == "ferret");
        CHECK(uri.fragment == "nose");
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("LocalFile", "")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("file:///foo.xml", &settings, &uri));

        CHECK(uri.scheme == "file");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "/foo.xml");
        CHECK(uri.pathSegments.size() == 1);
        CHECK(uri.pathSegments[0] == "foo.xml");
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_1", "RFC's example #1")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("ftp://ftp.is.co.za/rfc/rfc1808.txt", &settings, &uri));

        CHECK(uri.scheme == "ftp");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "ftp.is.co.za");
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "/rfc/rfc1808.txt");
        CHECK(uri.pathSegments.size() == 2);
        CHECK(uri.pathSegments[0] == "rfc");
        CHECK(uri.pathSegments[1] == "rfc1808.txt");
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_2", "RFC's example #2")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("http://www.ietf.org/rfc/rfc2396.txt", &settings, &uri));

        CHECK(uri.scheme == "http");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "www.ietf.org");
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "/rfc/rfc2396.txt");
        CHECK(uri.pathSegments.size() == 2);
        CHECK(uri.pathSegments[0] == "rfc");
        CHECK(uri.pathSegments[1] == "rfc2396.txt");
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_3", "RFC's example #3")
    {
        curi_settings rfc3_settings = settings;
        rfc3_settings.query_item_separator = '?';

        CHECK(curi_status_success == curi_parse_full_uri_nt("ldap://[2001:db8::7]/c=GB?objectClass?one", &rfc3_settings, &uri));

        CHECK(uri.scheme == "ldap");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "[2001:db8::7]");
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "/c=GB");
        CHECK(uri.pathSegments.size() == 1);
        CHECK(uri.pathSegments[0] == "c=GB");
        CHECK(uri.query == "objectClass?one");
        CHECK(uri.queryItems.size() == 2);
        CHECK(uri.queryItems.begin()->first == "objectClass");
        CHECK(uri.queryItems.begin()->second.empty());
        CHECK((++uri.queryItems.begin())->first == "one");
        CHECK((++uri.queryItems.begin())->second.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_4", "RFC's example #4")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("mailto:John.Doe@example.com", &settings, &uri));

        CHECK(uri.scheme == "mailto");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "John.Doe@example.com");
        CHECK(uri.pathSegments.size() == 1);
        CHECK(uri.pathSegments[0] == "John.Doe@example.com");
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_5", "RFC's example #5")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("news:comp.infosystems.www.servers.unix", &settings, &uri));

        CHECK(uri.scheme == "news");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "comp.infosystems.www.servers.unix");
        CHECK(uri.pathSegments.size() == 1);
        CHECK(uri.pathSegments[0] == "comp.infosystems.www.servers.unix");
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_6", "RFC's example #6")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("tel:+1-816-555-1212", &settings, &uri));

        CHECK(uri.scheme == "tel");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "+1-816-555-1212");
        CHECK(uri.pathSegments.size() == 1);
        CHECK(uri.pathSegments[0] == "+1-816-555-1212");
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_7", "RFC's example #7")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("telnet://192.0.2.16:80/", &settings, &uri));

        CHECK(uri.scheme == "telnet");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "192.0.2.16");
        CHECK(uri.portStr == "80");
        CHECK(uri.port == 80);
        CHECK(uri.path == "/");
        CHECK(uri.pathSegments.empty());
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("RFC_8", "RFC's example #8")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("urn:oasis:names:specification:docbook:dtd:xml:4.1.2", &settings, &uri));

        CHECK(uri.scheme == "urn");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "oasis:names:specification:docbook:dtd:xml:4.1.2");
        CHECK(uri.pathSegments.size() == 1);
        CHECK(uri.pathSegments[0] == "oasis:names:specification:docbook:dtd:xml:4.1.2");
        CHECK(uri.query.empty());
        CHECK(uri.queryItems.empty());
        CHECK(uri.fragment.empty());
        CHECK(uri.allocatedMemory == 0);
        CHECK(uri.deallocatedMemory == 0);
    }

    uri.clear();

    SECTION("URL decoding", "Very url-encoded heavy URI")
    {
        settings.url_decode = 1;

        CHECK(curi_status_success == curi_parse_full_uri_nt("http://some%20random%20dude@paren(thesis).org/brac%5Bkets%5D%3Alove%7Bthe%7Cpipe%7D?don%27t+you+think#c%3A%5CProgram%20Files", &settings, &uri));

        CHECK(uri.scheme == "http");
        CHECK(uri.userinfo == "some random dude");
        CHECK(uri.host == "paren(thesis).org");
        CHECK(uri.portStr.empty());
        CHECK(uri.port == 0);
        CHECK(uri.path == "/brac[kets]:love{the|pipe}");
        CHECK(uri.pathSegments.size() == 1);
        CHECK(uri.pathSegments[0] == "brac[kets]:love{the|pipe}");
        CHECK(uri.query == "don't you think");
        CHECK(uri.queryItems.size() == 1);
        CHECK(uri.queryItems.begin()->first == "don't you think");
        CHECK(uri.queryItems.begin()->second.empty());
        CHECK(uri.fragment == "c:\\Program Files");

        CHECK(uri.allocatedMemory == sizeof(char)*(
            strlen("some%20random%20dude") + 1 + 
            strlen("paren(thesis).org") + 1 + 
            strlen("/brac%5Bkets%5D%3Alove%7Bthe%7Cpipe%7D") + 1 +
            strlen("brac%5Bkets%5D%3Alove%7Bthe%7Cpipe%7D") + 1 +
            strlen("don%27t+you+think") + 1 +
            strlen("don%27t+you+think") + 1 +
            + 1 +
            strlen("c%3A%5CProgram%20Files") + 1));

        CHECK(uri.deallocatedMemory == uri.allocatedMemory);

        settings.url_decode = 0;
    }

    uri.clear();
};

TEST_CASE("ParseFullUri/Success/Scheme", "Valid URIs, scheme focus")
{
    curi_settings settings;
    curi_default_settings(&settings);
    settings.scheme_callback = scheme;

    URI uri;

    SECTION("NumberInScheme", "")
    {
        const std::string uriStr("ssh2:my/taylor/is/rich");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.scheme == "ssh2");
    }

    SECTION("PlusInScheme", "")
    {
        const std::string uriStr("foo+bar:yeepee");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.scheme == "foo+bar");
    }
}

TEST_CASE("ParseFullUri/Success/Path", "Valid URIs, path focus")
{
    curi_settings settings;
    curi_default_settings(&settings);
    settings.path_callback = path;

    URI uri;

    SECTION("UrlDecode", "")
    {
        settings.url_decode = 1;

        const std::string uriStr("ftp://liz@taylor/is%20f%23%26ing%20very/rich");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.path == "/is f#&ing very/rich");

        settings.url_decode = 0;
    }
}

TEST_CASE("ParseFullUri/Success/Query", "Valid URIs, query focus")
{
    curi_settings settings;
    curi_default_settings(&settings);
    settings.query_callback = query;
    settings.query_item_callback = queryItem;

    URI uri;

    SECTION("MultipleItems", "")
    {
        const std::string uriStr("ssh2:my/taylor/is/rich?tutu=foo&toto=bar&titi=baz");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.query == "tutu=foo&toto=bar&titi=baz");
        CHECK(uri.queryItems.size() == 3);
        CHECK(uri.queryItems["tutu"] == "foo");
        CHECK(uri.queryItems["toto"] == "bar");
        CHECK(uri.queryItems["titi"] == "baz");
    }
}

TEST_CASE("ParseFullUri/Error/Scheme", "Bad URIs, scheme focus")
{
    curi_settings settings;
    curi_default_settings(&settings);

    URI uri;

    SECTION("StartingWithNumber", "")
    {
        const std::string uriStr("3ftp://hello.org");
        
        CHECK(curi_status_error == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));
    }

    SECTION("Ampersand", "")
    {
        const std::string uriStr("bar&foo://google.com");

        CHECK(curi_status_error == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.scheme.empty());
    }

    SECTION("InterrogationMark", "")
    {
        const std::string uriStr("bar??://google.com");

        CHECK(curi_status_error == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.scheme.empty());
    }
}

TEST_CASE("ParseFullUri/Cancelled", "Canceled parsing of URI")
{
    const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
    curi_settings settings;

    SECTION("Scheme", "")
    {
        curi_default_settings(&settings);
        settings.scheme_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("Userinfo", "")
    {
        curi_default_settings(&settings);
        settings.userinfo_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("Host", "")
    {
        curi_default_settings(&settings);
        settings.host_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("PortStr", "")
    {
        curi_default_settings(&settings);
        settings.portStr_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("Port", "")
    {
        curi_default_settings(&settings);
        settings.port_callback = cancellingCallbackUint;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("Path", "")
    {
        curi_default_settings(&settings);
        settings.path_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("PathSegment", "")
    {
        curi_default_settings(&settings);
        settings.path_segment_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("Query", "")
    {
        curi_default_settings(&settings);
        settings.query_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("QueryItem", "")
    {
        curi_default_settings(&settings);
        settings.query_item_callback = cancellingCallbackTwoStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }

    SECTION("Fragment", "")
    {
        curi_default_settings(&settings);
        settings.fragment_callback = cancellingCallbackStr;

        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }
}

