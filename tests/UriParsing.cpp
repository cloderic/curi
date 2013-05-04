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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma warning(push, 0)
#include <catch.hpp>
#pragma warning(pop)
#pragma GCC diagnostic pop

#include <curi.h>

TEST_CASE("UriParsing/AllocFree", "curi_alloc and curi_free testing")
{
    SECTION("void user data", "")
    {
        curi_handle curi = curi_alloc(0);

        CHECK(curi != 0);

        curi_free(curi);
    }

    SECTION("int user data", "")
    {
        int userData = 3;
        curi_handle curi = curi_alloc(&userData);

        CHECK(curi != 0);

        curi_free(curi);
    }
}