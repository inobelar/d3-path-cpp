#ifndef D3__PATH__TEST__PATH_EQUAL_HPP
#define D3__PATH__TEST__PATH_EQUAL_HPP

#include "catch/catch.hpp"

#include "d3_path/Path.hpp"


#include "_regex_replace.hpp"
#include <sstream> // for std::ostringstream
#include <cmath>   // for std::abs(), std::round()
#include <iomanip> // for std::setprecision()

// IMPORTANT NOTE: used concrete d3_path::Path, instead of (more generic)
// d3_path::PathInterface because, otherwise it's NOT printed in failed tests.
namespace Catch {
    template<>
    struct StringMaker<d3_path::Path> {
        static std::string convert( const d3_path::Path& path ) {
            return path.toString();
        }
    };
}

namespace detail {

inline std::string toFixed(double value, int n) {
    std::ostringstream out;
    out << std::setprecision(n)  << value;
    return out.str();
}

static const std::string reNumber = R"([-+]?(?:\d+\.\d+|\d+\.|\.\d+|\d+)(?:[eE][-]?\d+)?)";

std::string formatNumber(const std::smatch& match) {
    const double s = std::stod( match.str() );
    return (std::abs(s - std::round(s)) < 1e-6) ? std::to_string( std::round(s) ) : toFixed(s, 6);
}

std::string normalizePath(const std::string& path) {
    return utils::regex_replace(path, std::regex(reNumber), formatNumber);
}

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

        const std::string actual   = detail::normalizePath(path_str);
        const std::string expected = detail::normalizePath(m_str   );

        return (actual == expected);
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
