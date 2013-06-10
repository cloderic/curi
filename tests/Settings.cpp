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

#define CATCH_CONFIG_MAIN // Telling catch to generate a main

#pragma warning(push, 0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <catch.hpp>
#pragma GCC diagnostic pop
#pragma warning(pop)

#include <curi.h>

#include <cstring>

TEST_CASE("Settings/Default", "default settings")
{
    curi_settings settings;
    curi_default_settings(&settings);

    CHECK(settings.allocate);
    CHECK(settings.deallocate);
    CHECK(!settings.scheme_callback);
    CHECK(!settings.userinfo_callback);
    CHECK(!settings.host_callback);
    CHECK(!settings.portStr_callback);
    CHECK(!settings.path_callback);
    CHECK(!settings.path_segment_callback);
    CHECK(!settings.query_callback);
    CHECK(!settings.query_item_str_callback);
    CHECK(settings.query_item_separator == '&');
    CHECK(settings.query_item_key_separator == '=');
    CHECK(!settings.fragment_callback);
    CHECK(settings.url_decode == 0);
}