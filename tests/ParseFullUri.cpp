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

#pragma warning(push, 0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <catch.hpp>
#pragma GCC diagnostic pop
#pragma warning(pop)

#include <curi.h>

#include <string>

#include <cstring>

struct URI
{
    std::string scheme;
    std::string userinfo;
    std::string host;
    std::string port;
    std::string path;
    std::string query;
    std::string fragment;

    void clear()
    {
        scheme.clear();
        userinfo.clear();
        host.clear();
        port.clear();
        path.clear();
        query.clear();
        fragment.clear();
    }
};

extern "C"
{
    static int scheme(void* userData, const char* scheme, size_t schemeLen);
    static int userinfo(void* userData, const char* userinfo, size_t userinfoLen);
    static int host(void* userData, const char* host, size_t hostLen);
    static int port(void* userData, const char* port, size_t portLen);
    static int path(void* userData, const char* path, size_t pathLen);
    static int query(void* userData, const char* query, size_t queryLen);
    static int fragment(void* userData, const char* fragment, size_t fragmentLen);
}

int scheme(void* userData, const char* scheme, size_t schemeLen)
{
    CAPTURE(scheme);
    CAPTURE(schemeLen);
    static_cast<URI*>(userData)->scheme.assign(scheme,schemeLen);
    return 1;
}

int userinfo(void* userData, const char* userinfo, size_t userinfoLen)
{
    CAPTURE(userinfo);
    CAPTURE(userinfoLen);
    static_cast<URI*>(userData)->userinfo.assign(userinfo,userinfoLen);
    return 1;
}

int host(void* userData, const char* host, size_t hostLen)
{
    CAPTURE(host);
    CAPTURE(hostLen);
    static_cast<URI*>(userData)->host.assign(host,hostLen);
    return 1;
}

int port(void* userData, const char* port, size_t portLen)
{
    CAPTURE(port);
    CAPTURE(portLen);
    static_cast<URI*>(userData)->port.assign(port,portLen);
    return 1;
}

int path(void* userData, const char* path, size_t pathLen)
{
    CAPTURE(path);
    CAPTURE(pathLen);
    static_cast<URI*>(userData)->path.assign(path,pathLen);
    return 1;
}

int query(void* userData, const char* query, size_t queryLen)
{
    CAPTURE(query);
    CAPTURE(queryLen);
    static_cast<URI*>(userData)->query.assign(query,queryLen);
    return 1;
}

int fragment(void* userData, const char* fragment, size_t fragmentLen)
{
    CAPTURE(fragment);
    CAPTURE(fragmentLen);
    static_cast<URI*>(userData)->fragment.assign(fragment,fragmentLen);
    return 1;
}

TEST_CASE("ParseFullUri/Success/Full", "Valid full URIs")
{
    curi_settings settings;
    curi_default_settings(&settings);
    settings.scheme_callback = scheme;
    settings.userinfo_callback = userinfo;
    settings.host_callback = host;
    settings.port_callback = port;
    settings.path_callback = path;
    settings.query_callback = query;
    settings.fragment_callback = fragment;

    URI uri;

    SECTION("Simple", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");

        CHECK(curi_status_success == curi_parse_full_uri_nt(uriStr.c_str(), &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.port == "8042");
        CHECK(uri.path == "/over/there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.fragment == "nose");
    }

    SECTION("Simple_Len", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.port == "8042");
        CHECK(uri.path == "/over/there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.fragment == "nose");
    }

    SECTION("Simple_BadLen", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr.c_str(), uriStr.length() * 100, &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.port == "8042");
        CHECK(uri.path == "/over/there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.fragment == "nose");
    }

    SECTION("Simple_NotNullTerminated", "")
    {
        const char* uriStr("foo://bar@example.com:8042/over/there?name=ferret#nosenoisynoisewedontcareabout");
        size_t actualLen = strlen("foo://bar@example.com:8042/over/there?name=ferret#nose");

        CHECK(curi_status_success == curi_parse_full_uri(uriStr, actualLen, &settings, &uri));

        CHECK(uri.scheme == "foo");
        CHECK(uri.userinfo == "bar");
        CHECK(uri.host == "example.com");
        CHECK(uri.port == "8042");
        CHECK(uri.path == "/over/there");
        CHECK(uri.query == "name=ferret");
        CHECK(uri.fragment == "nose");
    }

    uri.clear();

    SECTION("LocalFile", "")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("file:///foo.xml", &settings, &uri));

        CHECK(uri.scheme == "file");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.port.empty());
        CHECK(uri.path == "/foo.xml");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    uri.clear();

    SECTION("RFC_1", "RFC's example #1")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("ftp://ftp.is.co.za/rfc/rfc1808.txt", &settings, &uri));

        CHECK(uri.scheme == "ftp");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "ftp.is.co.za");
        CHECK(uri.port.empty());
        CHECK(uri.path == "/rfc/rfc1808.txt");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    SECTION("RFC_2", "RFC's example #2")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("http://www.ietf.org/rfc/rfc2396.txt", &settings, &uri));

        CHECK(uri.scheme == "http");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "www.ietf.org");
        CHECK(uri.port.empty());
        CHECK(uri.path == "/rfc/rfc2396.txt");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    SECTION("RFC_3", "RFC's example #3")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("ldap://[2001:db8::7]/c=GB?objectClass?one", &settings, &uri));

        CHECK(uri.scheme == "ldap");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "[2001:db8::7]");
        CHECK(uri.port.empty());
        CHECK(uri.path == "/c=GB");
        CHECK(uri.query == "objectClass?one");
        CHECK(uri.fragment.empty());
    }

    SECTION("RFC_4", "RFC's example #4")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("mailto:John.Doe@example.com", &settings, &uri));

        CHECK(uri.scheme == "mailto");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.port.empty());
        CHECK(uri.path == "John.Doe@example.com");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    SECTION("RFC_5", "RFC's example #5")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("news:comp.infosystems.www.servers.unix", &settings, &uri));

        CHECK(uri.scheme == "news");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.port.empty());
        CHECK(uri.path == "comp.infosystems.www.servers.unix");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    SECTION("RFC_6", "RFC's example #6")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("tel:+1-816-555-1212", &settings, &uri));

        CHECK(uri.scheme == "tel");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.port.empty());
        CHECK(uri.path == "+1-816-555-1212");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    SECTION("RFC_7", "RFC's example #7")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("telnet://192.0.2.16:80/", &settings, &uri));

        CHECK(uri.scheme == "telnet");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host == "192.0.2.16");
        CHECK(uri.port == "80");
        CHECK(uri.path == "/");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    SECTION("RFC_8", "RFC's example #8")
    {
        CHECK(curi_status_success == curi_parse_full_uri_nt("urn:oasis:names:specification:docbook:dtd:xml:4.1.2", &settings, &uri));

        CHECK(uri.scheme == "urn");
        CHECK(uri.userinfo.empty());
        CHECK(uri.host.empty());
        CHECK(uri.port.empty());
        CHECK(uri.path == "oasis:names:specification:docbook:dtd:xml:4.1.2");
        CHECK(uri.query.empty());
        CHECK(uri.fragment.empty());
    }

    SECTION("URL decoding", "Very url-encoded heavy URI")
    {
        settings.url_decode = 1;

        CHECK(curi_status_success == curi_parse_full_uri_nt("http://some%20random%20dude@paren(thesis).org/brac%5Bkets%5D%3Alove%7Bthe%7Cpipe%7D?don%27t+you+think#c%3A%5CProgram%20Files", &settings, &uri));

        CHECK(uri.scheme == "http");
        CHECK(uri.userinfo == "some random dude");
        CHECK(uri.host == "paren(thesis).org");
        CHECK(uri.port.empty());
        CHECK(uri.path == "/brac[kets]:love{the|pipe}");
        CHECK(uri.query == "don't you think");
        CHECK(uri.fragment == "c:\\Program Files");

        settings.url_decode = 0;
    }
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

extern "C"
{
    static int cancellingCallback(void* userData, const char* str, size_t strLen)
    {
        return 0;
    }
}
TEST_CASE("ParseFullUri/Cancelled", "Canceled parsing of URI")
{
    SECTION("Scheme", "")
    {
        curi_settings settings;
        curi_default_settings(&settings);
        settings.scheme_callback = cancellingCallback;

        const std::string uriStr("http://google.com");
        
        CHECK(curi_status_canceled == curi_parse_full_uri(uriStr.c_str(), uriStr.length(), &settings, 0));
    }
}

