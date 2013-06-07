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

#ifndef CURI_TESTS_COMMON_H
#define CURI_TESTS_COMMON_H

#pragma warning(push, 0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <catch.hpp>
#pragma GCC diagnostic pop
#pragma warning(pop)

#include <string>

struct URI
{
    size_t allocatedMemory;
    size_t deallocatedMemory;
    std::string scheme;
    std::string userinfo;
    std::string host;
    std::string portStr;
    unsigned int port;
    std::string path;
    std::vector<std::string> pathSegments;
    std::string query;
    std::map<std::string, std::string> queryItems; 
    std::string fragment;

    void clear()
    {
        allocatedMemory = 0;
        deallocatedMemory = 0;
        scheme.clear();
        userinfo.clear();
        host.clear();
        port = 0;
        portStr.clear();
        path.clear();
        pathSegments.clear();
        query.clear();
        fragment.clear();
    }
};

extern "C"
{
    void* test_allocate(void* userData, size_t size);
    void test_deallocate(void* userData, void* ptr, size_t size);
    static int scheme(void* userData, const char* scheme, size_t schemeLen);
    static int userinfo(void* userData, const char* userinfo, size_t userinfoLen);
    static int host(void* userData, const char* host, size_t hostLen);
    static int portStr(void* userData, const char* port, size_t portLen);
    static int path(void* userData, const char* path, size_t pathLen);
    static int pathSegment(void* userData, const char* pathSegment, size_t pathSegmentLen);
    static int query(void* userData, const char* query, size_t queryLen);
    static int queryItem(void* userData, const char* queryItem, size_t queryItemLen);
    static int fragment(void* userData, const char* fragment, size_t fragmentLen);
    static int cancellingCallbackStr(void* userData, const char* str, size_t strLen)
    {
        return 0;
    }
    static int cancellingCallbackTwoStr(void* userData, const char* str1, size_t str1Len, const char* str2, size_t str2Len)
    {
        return 0;
    }
    static int cancellingCallbackUint(void* userData, unsigned int port)
    {
        return 0;
    }
}

inline void* test_allocate(void* userData, size_t size)
{
    static_cast<URI*>(userData)->allocatedMemory += size;
    return malloc(size);
}

inline void test_deallocate(void* userData, void* ptr, size_t size)
{
    static_cast<URI*>(userData)->deallocatedMemory += size;
    free(ptr);
}

inline int scheme(void* userData, const char* scheme, size_t schemeLen)
{
    CAPTURE(scheme);
    CAPTURE(schemeLen);
    static_cast<URI*>(userData)->scheme.assign(scheme,schemeLen);
    return 1;
}

inline int userinfo(void* userData, const char* userinfo, size_t userinfoLen)
{
    CAPTURE(userinfo);
    CAPTURE(userinfoLen);
    static_cast<URI*>(userData)->userinfo.assign(userinfo,userinfoLen);
    return 1;
}

inline int host(void* userData, const char* host, size_t hostLen)
{
    CAPTURE(host);
    CAPTURE(hostLen);
    static_cast<URI*>(userData)->host.assign(host,hostLen);
    return 1;
}

inline int port(void* userData, unsigned int port)
{
    CAPTURE(port);
    static_cast<URI*>(userData)->port = port;
    return 1;
}

inline int portStr(void* userData, const char* portStr, size_t portStrLen)
{
    CAPTURE(portStr);
    CAPTURE(portStrLen);
    static_cast<URI*>(userData)->portStr.assign(portStr,portStrLen);
    return 1;
}

inline int path(void* userData, const char* path, size_t pathLen)
{
    CAPTURE(path);
    CAPTURE(pathLen);
    static_cast<URI*>(userData)->path.assign(path,pathLen);
    return 1;
}

inline int pathSegment(void* userData, const char* pathSegment, size_t pathSegmentLen)
{
    CAPTURE(pathSegment);
    CAPTURE(pathSegmentLen);
    static_cast<URI*>(userData)->pathSegments.push_back(std::string(pathSegment,pathSegmentLen));
    return 1;
}

inline int query(void* userData, const char* query, size_t queryLen)
{
    CAPTURE(query);
    CAPTURE(queryLen);
    static_cast<URI*>(userData)->query.assign(query,queryLen);
    return 1;
}

inline int queryItem(void* userData, const char* queryItemKey, size_t queryItemKeyLen, const char* queryItemValue, size_t queryItemValueLen)
{
    CAPTURE(queryItemKey);
    CAPTURE(queryItemKeyLen);
    CAPTURE(queryItemValue);
    CAPTURE(queryItemValueLen);
    static_cast<URI*>(userData)->queryItems[std::string(queryItemKey,queryItemKeyLen)] = std::string(queryItemValue, queryItemValueLen);
    return 1;
}

inline int fragment(void* userData, const char* fragment, size_t fragmentLen)
{
    CAPTURE(fragment);
    CAPTURE(fragmentLen);
    static_cast<URI*>(userData)->fragment.assign(fragment,fragmentLen);
    return 1;
}

#endif
