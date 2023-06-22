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
