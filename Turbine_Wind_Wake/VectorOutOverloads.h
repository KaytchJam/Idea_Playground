#pragma once

#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& strm, const std::vector<char>& v) {
    strm << "{";
    if (v.size() > 0) strm << v[0];
    for (int i = 1; i < v.size(); i++) {
        strm << ", " << v[i];
    }
    strm << "}";
    return strm;
}


std::ostream& operator<<(std::ostream& strm, const std::vector<int>& v) {
    strm << "{";
    if (v.size() > 0) strm << v[0];
    for (int i = 1; i < v.size(); i++) {
        strm << "," << v[i];
    }
    strm << "}";
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const std::vector<std::vector<int> *>& v) {
    strm << "{";
    if (v.size() > 0) strm << *v[0];
    for (int i = 1; i < v.size(); i++) {
        strm << "," << *v[i];
    }
    strm << "}";
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const std::vector<std::vector<int>>& v) {
    strm << "{";
    if (v.size() > 0) strm << v[0];
    for (int i = 1; i < v.size(); i++) {
        strm << "," << v[i];
    }
    strm << "}";
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const std::vector<std::vector<char>>& v) {
    strm << "{";
    if (v.size() > 0) strm << v[0];
    for (int i = 1; i < v.size(); i++) {
        strm << "," << v[i];
    }
    strm << "}";
    return strm;
}