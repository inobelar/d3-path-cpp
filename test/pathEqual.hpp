#ifndef D3__PATH__TEST__PATH_EQUAL_HPP
#define D3__PATH__TEST__PATH_EQUAL_HPP

#include "catch/catch.hpp"

#include "d3_path/PathInterface.hpp"

//#include <regex>
#include <sstream> // for std::ostringstream

#include <cmath>

namespace detail {

// FIXME: currently unused
static const std::string reNumber = R"(/[-+]?(?:\d+\.\d+|\d+\.|\.\d+|\d+)(?:[eE][-]?\d+)?/g)";

} // namespace detail

class PathEqual : public Catch::MatcherBase<d3_path::PathInterface>
{
    const std::string& m_str;

public:

    PathEqual(const std::string& str)
        : m_str(str)
    {}

    // Performs the test for this matcher
    bool match(const d3_path::PathInterface& path) const override
    {
        const std::string path_str = path.toString();

        // TODO: implement: normalizePath() and formatNumber()

        return path_str == m_str;
    }

    // Produces a string describing what this matcher does. It should include
    // any provided data and be written as if it were stating a fact (in the
    // output it will be preceded by the value under test).
    virtual std::string describe() const override {
        std::ostringstream ss;
        ss << "Should be equal to: " << m_str;
        return ss.str();
    }
};

// The builder function
inline PathEqual pathEqual(const std::string& str) {
    return PathEqual(str);
}

#endif // D3__PATH__TEST__PATH_EQUAL_HPP
