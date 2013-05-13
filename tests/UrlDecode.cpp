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

static const size_t asciiReferenceCount = 102;
static const char* asciiReference[asciiReferenceCount * 2] = {
    "\0","%00",
    "\b","%08",
    "\t","%09",
    "\n","%0A",
    "\v","%0B",
    "\r","%0D",
    " ","%20",
    " ","+",
    "!","%21",
    "\"","%22",
    "#","%23", 
    "$","%24", 
    "%","%25", 
    "&","%26", 
    "'","%27", 
    "(","%28", 
    ")","%29", 
    "*","%2A", 
    "+","%2B", 
    ",","%2C", 
    "-","%2D", 
    ".","%2E", 
    "/","%2F", 
    "0","%30", 
    "1","%31", 
    "2","%32", 
    "3","%33", 
    "4","%34", 
    "5","%35", 
    "6","%36", 
    "7","%37", 
    "8","%38", 
    "9","%39", 
    ":","%3A", 
    ";","%3B", 
    "<","%3C", 
    "=","%3D", 
    ">","%3E", 
    "?","%3F", 
    "@","%40", 
    "A","%41", 
    "B","%42", 
    "C","%43", 
    "D","%44", 
    "E","%45", 
    "F","%46", 
    "G","%47", 
    "H","%48", 
    "I","%49", 
    "J","%4A", 
    "K","%4B", 
    "L","%4C", 
    "M","%4D", 
    "N","%4E", 
    "O","%4F", 
    "P","%50", 
    "Q","%51", 
    "R","%52", 
    "S","%53", 
    "T","%54", 
    "U","%55", 
    "V","%56", 
    "W","%57", 
    "X","%58", 
    "Y","%59", 
    "Z","%5A", 
    "[","%5B", 
    "\\","%5C",
    "]","%5D", 
    "^","%5E", 
    "_","%5F", 
    "`","%60", 
    "a","%61", 
    "b","%62", 
    "c","%63", 
    "d","%64", 
    "e","%65", 
    "f","%66", 
    "g","%67", 
    "h","%68", 
    "i","%69", 
    "j","%6A", 
    "k","%6B", 
    "l","%6C", 
    "m","%6D", 
    "n","%6E", 
    "o","%6F", 
    "p","%70", 
    "q","%71", 
    "r","%72", 
    "s","%73", 
    "t","%74", 
    "u","%75", 
    "v","%76", 
    "w","%77", 
    "x","%78", 
    "y","%79", 
    "z","%7A", 
    "{","%7B", 
    "|","%7C", 
    "}","%7D", 
    "~","%7E"};

TEST_CASE("UrlDecode/Single", "Unit characters")
{
    char output;

    for (size_t i(0) ; i < asciiReferenceCount ; ++i)
    {
        char expectedOutput = asciiReference[2*i][0];
        std::string input = asciiReference[2*i+1];

        std::ostringstream oss;
        oss << i << ": " << input << "=" << expectedOutput; 
        SECTION(oss.str(), "")
        {   
            CAPTURE(expectedOutput);
            
            CAPTURE(input);

            CHECK(curi_url_decode(input.c_str(),input.length(),&output,1,0) == curi_status_success);
            CAPTURE(output);
            CHECK(expectedOutput == output);
        }
    }
}

TEST_CASE("UrlDecode/Full", "Full url encoded strings")
{
    SECTION("Einstein","")
    {
        static const std::string input = "Two%20things%20are%20infinite%3A%20the%20universe%20and%20human%20stupidity%3B%20and%20I'm%20not%20sure%20about%20the%20the%20universe.";
        static const std::string expectedOutput = "Two things are infinite: the universe and human stupidity; and I'm not sure about the the universe.";
        char* output = new char[input.length()];

        size_t outputLen;
        CHECK(curi_url_decode(input.c_str(),input.length(),output,input.length(),&outputLen) == curi_status_success);
        
        CHECK(outputLen == expectedOutput.length());
        CHECK(expectedOutput == std::string(output,outputLen));

        delete[] output;
    }

    SECTION("Crowds control","")
    {
        static const std::string input = "http%3A%2F%2Fwww.crowdscontrol.net%2Fgroup-navigation-state-of-the-art-report%2F";
        static const std::string expectedOutput = "http://www.crowdscontrol.net/group-navigation-state-of-the-art-report/";
        char* output = new char[input.length()];

        size_t outputLen;
        CHECK(curi_url_decode_nt(input.c_str(),output,input.length(),&outputLen) == curi_status_success);
        
        CHECK(outputLen == expectedOutput.length());
        CHECK(expectedOutput == std::string(output,outputLen));

        delete[] output;
    }

    SECTION("42","")
    {
        static const std::string input = "Do+you+know+what+is+%22The+answer+to+life+the+universe+and+everything%22%3F";
        static const std::string expectedOutput = "Do you know what is \"The answer to life the universe and everything\"?";
        char* output = new char[input.length()];

        size_t outputLen;
        CHECK(curi_url_decode_nt(input.c_str(),output,input.length(),&outputLen) == curi_status_success);
        
        CHECK(outputLen == expectedOutput.length());
        CHECK(expectedOutput == std::string(output,outputLen));

        delete[] output;
    }
}

