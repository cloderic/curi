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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma warning(push, 0)
#include <catch.hpp>
#pragma warning(pop)
#pragma GCC diagnostic pop

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
};

extern "C"
{
    static int scheme(void* userData, const char* scheme, size_t schemeLen);
}

int scheme(void* userData, const char* scheme, size_t schemeLen)
{
    CAPTURE(scheme);
    CAPTURE(schemeLen);
    static_cast<URI*>(userData)->scheme.assign(scheme,schemeLen);
    return 1;
}

TEST_CASE("Parsing/Scheme/Success", "Successful parsing of URIs scheme")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.scheme = scheme;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("FullExample", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme == "foo");
    }

    SECTION("NumberInScheme", "")
    {
        const std::string uriStr("ssh2:my/taylor/is/rich");

        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme == "ssh2");
    }

    SECTION("PlusInScheme", "")
    {
        const std::string uriStr("foo+bar:yeepee");

        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme == "foo+bar");
    }
    curi_free(curi);
}

TEST_CASE("Parsing/Scheme/Error", "Failing parsing of URIs scheme")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("StartingWithNumber", "")
    {
        const std::string uriStr("3ftp://hello.org");
        
        CHECK(curi_status_error == curi_parse(curi,uriStr.c_str(),uriStr.length()));
    }

    SECTION("Ampersand", "")
    {
        const std::string uriStr("bar&foo://google.com");

        CHECK(curi_status_error == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme.empty());
    }

    SECTION("ExclamationMark", "")
    {
        const std::string uriStr("bar??://google.com");

        CHECK(curi_status_error == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme.empty());
    }

    curi_free(curi);
}

extern "C"
{
    static int userinfo(void* userData, const char* userinfo, size_t userinfoLen);
}

int userinfo(void* userData, const char* userinfo, size_t userinfoLen)
{
    CAPTURE(userinfo);
    CAPTURE(userinfoLen);
    static_cast<URI*>(userData)->userinfo.assign(userinfo,userinfoLen);
    return 1;
}

TEST_CASE("Parsing/Userinfo/Success", "Successful parsing of URIs userinfo")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.userinfo = userinfo;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("FullExample", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.userinfo == "bar");
    }

    SECTION("LocalFile", "")
    {
        const std::string uriStr("file:///foo.xml");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.userinfo.empty());
    }

    curi_free(curi);
}

extern "C"
{
    static int host(void* userData, const char* host, size_t hostLen);
}

int host(void* userData, const char* host, size_t hostLen)
{
    CAPTURE(host);
    CAPTURE(hostLen);
    static_cast<URI*>(userData)->host.assign(host,hostLen);
    return 1;
}

TEST_CASE("Parsing/Host/Success", "Successful parsing of URIs host")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.host = host;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("FullExample", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.host == "example.com");
    }

    SECTION("LocalFile", "")
    {
        const std::string uriStr("file:///foo.xml");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.host.empty());
    }

    curi_free(curi);
}

extern "C"
{
    static int port(void* userData, const char* port, size_t portLen);
}

int port(void* userData, const char* port, size_t portLen)
{
    CAPTURE(port);
    CAPTURE(portLen);
    static_cast<URI*>(userData)->port.assign(port,portLen);
    return 1;
}

TEST_CASE("Parsing/Port/Success", "Successful parsing of URIs port")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.port = port;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("FullExample", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.port == "8042");
    }

    SECTION("LocalFile", "")
    {
        const std::string uriStr("file:///foo.xml");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.port.empty());
    }

    curi_free(curi);
}

extern "C"
{
    static int path(void* userData, const char* path, size_t pathLen);
}

int path(void* userData, const char* path, size_t pathLen)
{
    CAPTURE(path);
    CAPTURE(pathLen);
    static_cast<URI*>(userData)->path.assign(path,pathLen);
    return 1;
}

TEST_CASE("Parsing/Path/Success", "Successful parsing of URIs path")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.path = path;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("FullExample", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.path == "/over/there");
    }

    SECTION("LocalFile", "")
    {
        const std::string uriStr("file:///foo.xml");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.path == "/foo.xml");
    }

    SECTION("LocalFile_Short", "")
    {
        const std::string uriStr("file:foo.xml");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.path == "foo.xml");
    }

    curi_free(curi);
}

extern "C"
{
    static int query(void* userData, const char* query, size_t queryLen);
}

int query(void* userData, const char* query, size_t queryLen)
{
    CAPTURE(query);
    CAPTURE(queryLen);
    static_cast<URI*>(userData)->fragment.assign(query,queryLen);
    return 1;
}

TEST_CASE("Parsing/Query/Success", "Successful parsing of URIs query")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.query = query;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("FullExample", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.fragment == "name=ferret");
    }

    curi_free(curi);
}

extern "C"
{
    static int fragment(void* userData, const char* fragment, size_t fragmentLen);
}

int fragment(void* userData, const char* fragment, size_t fragmentLen)
{
    CAPTURE(fragment);
    CAPTURE(fragmentLen);
    static_cast<URI*>(userData)->fragment.assign(fragment,fragmentLen);
    return 1;
}

TEST_CASE("Parsing/Fragment/Success", "Successful parsing of URIs fragment")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.fragment = fragment;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("FullExample", "")
    {
        const std::string uriStr("foo://bar@example.com:8042/over/there?name=ferret#nose");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.fragment == "nose");
    }

    curi_free(curi);
}

extern "C"
{
    static int cancellingCallback(void* userData, const char* str, size_t strLen)
    {
        return 0;
    }
}

TEST_CASE("Parsing/Scheme/Cancel", "Canceled parsing of URI scheme")
{
    SECTION("Scheme", "")
    {
        curi_callbacks callbacks;
        memset(&callbacks,0,sizeof(callbacks));
        callbacks.scheme = cancellingCallback;

        curi_handle curi = curi_alloc(&callbacks,NULL);

        const std::string uriStr("http://google.com");
        
        CHECK(curi_status_canceled == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        curi_free(curi);
    }
}

