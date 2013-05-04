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
    callbacks.curi_scheme = scheme;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("Simple", "")
    {
        const std::string uriStr("http://google.com");
        
        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme == "http");
    }

    SECTION("OnlyScheme", "")
    {
        const std::string uriStr("ftp");

        CHECK(curi_status_success == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme == "ftp");
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

TEST_CASE("Parsing/Scheme/Error", "Failed parsing of URIs scheme")
{
    curi_callbacks callbacks;
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.curi_scheme = scheme;

    URI uri;
    curi_handle curi = curi_alloc(&callbacks,&uri);

    SECTION("NumberAtFirst", "")
    {
        const std::string uriStr("2foo://google.com");
        
        CHECK(curi_status_error == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        CHECK(uri.scheme.empty());
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
    static int singleElementCancellingCallback(void* userData, const char* str, size_t strLen)
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
        callbacks.curi_scheme = singleElementCancellingCallback;

        curi_handle curi = curi_alloc(&callbacks,NULL);

        const std::string uriStr("http://google.com");
        
        CHECK(curi_status_canceled == curi_parse(curi,uriStr.c_str(),uriStr.length()));

        curi_free(curi);
    }
}

