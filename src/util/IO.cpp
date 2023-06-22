// -----------------------------------------------------------------------------
// This file is part of DeepDrill
//
// A Mandelbrot generator based on perturbation and series approximation
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "IO.h"
#include <algorithm>
#include <fstream>
#include <vector>

namespace dd {

string lowercased(const string& s)
{
    string result;
    for (auto c : s) { result += tolower(c); }
    return result;
}

string uppercased(const string& s)
{
    string result;
    for (auto c : s) { result += toupper(c); }
    return result;
}

/*
string extractPath(const string &s)
{
    auto idx = s.rfind('/');
    auto pos = 0;
    auto len = idx != string::npos ? idx + 1 : 0;
    return s.substr(pos, len);
}

string extractName(const string &s)
{
    auto idx = s.rfind('/');
    auto pos = idx != string::npos ? idx + 1 : 0;
    auto len = string::npos;
    return s.substr(pos, len);
}

string extractSuffix(const string &s)
{
    auto idx = s.rfind('.');
    auto pos = idx != string::npos ? idx + 1 : 0;
    auto len = string::npos;
    return s.substr(pos, len);
}

string stripPath(const string &s)
{
    auto idx = s.rfind('/');
    auto pos = idx != string::npos ? idx + 1 : 0;
    auto len = string::npos;
    return s.substr(pos, len);
}

string stripName(const string &s)
{
    auto idx = s.rfind('/');
    auto pos = 0;
    auto len = idx != string::npos ? idx : 0;
    return s.substr(pos, len);
}

string stripSuffix(const string &s)
{
    auto idx = s.rfind('.');
    auto pos = 0;
    auto len = idx != string::npos ? idx : string::npos;
    return s.substr(pos, len);
}
*/

string join(const std::vector<string> &v, const string &delim1)
{
    return join(v, delim1, delim1);
}

string join(const std::vector<string> &v, const string &delim1, const string &delim2)
{
    std::string result;

    if (isize size = isize(v.size()); size > 0) {

        // Add the first element
        result = v[0];

        if (size > 1) {

            // Append all middle elements
            for (isize i = 1; i < size - 1; i++) result += delim1 + v[i];

            // Append the last element
            result += delim2 + v[size - 1];
        }
    }

    return result;
}

isize countFiles(const fs::path &suffix)
{
    return countFiles(fs::current_path(), suffix);
}

isize countFiles(const fs::path &path, const fs::path &suffix)
{
    isize count = 0;

    for (const auto &file : fs::directory_iterator(path)) {

        if (file.path().extension() == suffix) {
            count++;
        }
    }

    return count;
}

bool matchingStreamHeader(std::istream &stream, const u8 *header, isize len)
{
    stream.seekg(0, std::ios::beg);
    
    for (isize i = 0; i < len; i++) {
        int c = stream.get();
        if (c != (int)header[i]) {
            stream.seekg(0, std::ios::beg);
            return false;
        }
    }
    stream.seekg(0, std::ios::beg);
    return true;
}

bool
matchingBufferHeader(const u8 *buffer, const u8 *header, isize len)
{
    assert(buffer != nullptr);
    assert(header != nullptr);
    
    for (isize i = 0; i < len; i++) {
        if (header[i] != buffer[i])
            return false;
    }

    return true;
}

bool compareFiles(const fs::path &path1, const fs::path &path2)
{
    std::ifstream stream1(path1, std::ifstream::binary|std::ifstream::ate);
    std::ifstream stream2(path2, std::ifstream::binary|std::ifstream::ate);

    if (stream1.fail() || stream2.fail()) {
        return false;
    }

    if (stream1.tellg() != stream2.tellg()) {
        return false;
    }

    stream1.seekg(0, std::ifstream::beg);
    stream2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(stream1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(stream2.rdbuf()));
}

std::ostream &
dec::operator()(std::ostream &os) const
{
    os << std::dec << value;
    return os;
};

std::ostream &
hex::operator()(std::ostream &os) const
{
    os << std::hex << "0x" << std::setw(digits) << std::setfill('0') << value;
    return os;
};

std::ostream &
tab::operator()(std::ostream &os) const {
    os << std::setw(pads) << std::right << std::setfill(' ') << str << " : ";
    return os;
}

std::ostream &
bol::operator()(std::ostream &os) const {
    os << (value ? str1 : str2);
    return os;
}

const string &bol::yes = "yes";
const string &bol::no = "no";

}
