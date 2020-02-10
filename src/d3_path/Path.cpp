#include "d3_path/Path.hpp"

// -----------------------------------------------------------------------------

#include <limits> // for std::numeric_limits<T>::quiet_NaN()

constexpr auto NULL_NUMBER = std::numeric_limits<d3_path::Path::number_t>::quiet_NaN();

// -----------------------------------------------------------------------------

#include <sstream>

template <typename T>
static std::string to_str(const T& value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

// -----------------------------------------------------------------------------

#include <cmath> // for std::isnan(), std::abs(), std::sqrt(), std::tan(), std::acos(), std::cos(), std::sin()

constexpr d3_path::Path::number_t pi = M_PI;
constexpr d3_path::Path::number_t tau = 2 * pi;
constexpr d3_path::Path::number_t epsilon = 1e-6;
constexpr d3_path::Path::number_t tauEpsilon = tau - epsilon;

#include <exception> // for std::runtime_error()

namespace d3_path {

Path::Path()
    : _x0( NULL_NUMBER )
    , _y0( NULL_NUMBER )
    , _x1( NULL_NUMBER )
    , _y1( NULL_NUMBER )
{ }

void Path::moveTo(PathInterface::number_t x, PathInterface::number_t y)
{
    this->_ += "M" + to_str(this->_x0 = this->_x1 = x) + "," + to_str(this->_y0 = this->_y1 = y);
}

void Path::closePath()
{
    if ( std::isnan( this->_x1 ) == false ) {
        this->_x1 = this->_x0; this->_y1 = this->_y0;
        this->_ += "Z";
    }
}

void Path::lineTo(PathInterface::number_t x, PathInterface::number_t y)
{
    this->_ += "L" + to_str(this->_x1 = x) + "," + to_str(this->_y1 = y);
}

void Path::quadraticCurveTo(PathInterface::number_t x1, PathInterface::number_t y1, PathInterface::number_t x, PathInterface::number_t y)
{
    this->_ += "Q" + to_str(x1) + "," + to_str(y1) + "," + to_str(this->_x1 = x) + "," + to_str(this->_y1 = y);
}

void Path::bezierCurveTo(PathInterface::number_t x1, PathInterface::number_t y1, PathInterface::number_t x2, PathInterface::number_t y2, PathInterface::number_t x, PathInterface::number_t y)
{
    this->_ += "C" + to_str(x1) + "," + to_str(y1) + "," + to_str(x2) + "," + to_str(y2) + "," + to_str(this->_x1 = x) + "," + to_str(this->_y1 = y);
}

void Path::arcTo(PathInterface::number_t x1, PathInterface::number_t y1, PathInterface::number_t x2, PathInterface::number_t y2, PathInterface::number_t r)
{
    const number_t
            x0 = this->_x1,
            y0 = this->_y1,
            x21 = x2 - x1,
            y21 = y2 - y1,
            x01 = x0 - x1,
            y01 = y0 - y1,
            l01_2 = x01 * x01 + y01 * y01;

    // Is the radius negative? Error.
    if (r < 0) throw std::runtime_error("negative radius: " + to_str(r));

    // Is this path empty? Move to (x1,y1).
    if ( std::isnan( this->_x1 ) == true) {
        this->_ += "M" + to_str(this->_x1 = x1) + "," + to_str(this->_y1 = y1);
    }

    // Or, is (x1,y1) coincident with (x0,y0)? Do nothing.
    else if (!(l01_2 > epsilon));

    // Or, are (x0,y0), (x1,y1) and (x2,y2) collinear?
    // Equivalently, is (x1,y1) coincident with (x2,y2)?
    // Or, is the radius zero? Line to (x1,y1).
    else if (!(std::abs(y01 * x21 - y21 * x01) > epsilon) || !r) {
        this->_ += "L" + to_str(this->_x1 = x1) + "," + to_str(this->_y1 = y1);
    }

    // Otherwise, draw an arc!
    else {
        const number_t
                x20 = x2 - x0,
                y20 = y2 - y0,
                l21_2 = x21 * x21 + y21 * y21,
                l20_2 = x20 * x20 + y20 * y20,
                l21 = std::sqrt(l21_2),
                l01 = std::sqrt(l01_2),
                l = r * std::tan((pi - std::acos((l21_2 + l01_2 - l20_2) / (2 * l21 * l01))) / 2),
                t01 = l / l01,
                t21 = l / l21;

        // If the start tangent is not coincident with (x0,y0), line to.
        if (std::abs(t01 - 1) > epsilon) {
            this->_ += "L" + to_str(x1 + t01 * x01) + "," + to_str(y1 + t01 * y01);
        }

        this->_ += "A" + to_str(r) + "," + to_str(r) + ",0,0," + to_str((int)(y01 * x20 > x01 * y20)) + "," + to_str(this->_x1 = x1 + t21 * x21) + "," + to_str(this->_y1 = y1 + t21 * y21);
    }
}

void Path::arc(PathInterface::number_t x, PathInterface::number_t y, PathInterface::number_t r, PathInterface::number_t a0, PathInterface::number_t a1, bool ccw)
{
    const number_t
            dx = r * std::cos(a0),
            dy = r * std::sin(a0),
            x0 = x + dx,
            y0 = y + dy,
            cw = !ccw;
   number_t
            da = ccw ? a0 - a1 : a1 - a0;

    // Is the radius negative? Error.
    if (r < 0) throw std::runtime_error("negative radius: " + to_str(r));

    // Is this path empty? Move to (x0,y0).
    if ( std::isnan( this->_x1 ) == true ) {
        this->_ += "M" + to_str(x0) + "," + to_str(y0);
    }

    // Or, is (x0,y0) not coincident with the previous point? Line to (x0,y0).
    else if ( std::abs(this->_x1 - x0) > epsilon || std::abs(this->_y1 - y0) > epsilon) {
        this->_ += "L" + to_str(x0) + "," + to_str(y0);
    }

    // Is this arc empty? We’re done.
    if (!r) return;

    // Does the angle go the wrong way? Flip the direction.
    if (da < 0) da = std::fmod(da , tau) + tau;

    // Is this a complete circle? Draw two arcs to complete the circle.
    if (da > tauEpsilon) {
        this->_ += "A" + to_str(r) + "," + to_str(r) + ",0,1," + to_str(cw) + "," + to_str(x - dx) + "," + to_str(y - dy) + "A" + to_str(r) + "," + to_str(r) + ",0,1," + to_str(cw) + "," + to_str(this->_x1 = x0) + "," + to_str(this->_y1 = y0);
    }

    // Is this arc non-empty? Draw an arc!
    else if (da > epsilon) {
        this->_ += "A" + to_str(r) + "," + to_str(r) + ",0," + to_str((int)(da >= pi)) + "," + to_str(cw) + "," + to_str(this->_x1 = x + r * std::cos(a1)) + "," + to_str(this->_y1 = y + r * std::sin(a1));
    }
}

void Path::rect(PathInterface::number_t x, PathInterface::number_t y, PathInterface::number_t w, PathInterface::number_t h)
{
    this->_ += "M" + to_str(this->_x0 = this->_x1 = x) + "," + to_str(this->_y0 = this->_y1 = +y) + "h" + to_str(w) + "v" + to_str(h) + "h" + to_str(-w) + "Z";
}

std::string Path::toString() const
{
    return _;
}

} // namespace d3_path
