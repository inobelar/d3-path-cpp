#ifndef D3__PATH__PATH_HPP
#define D3__PATH__PATH_HPP

#include "d3_path/PathInterface.hpp"

namespace d3_path {

class Path : public PathInterface
{
    number_t _x0, _y0; // start of current subpath
    number_t _x1, _y1; // end of current subpath

    std::string _;

public:

    Path();

    void moveTo(number_t x, number_t y) override;

    void closePath() override;

    void lineTo(number_t x, number_t y) override;

    void quadraticCurveTo(number_t x1, number_t y1, number_t x, number_t y) override;

    void bezierCurveTo(number_t x1, number_t y1, number_t x2, number_t y2, number_t x, number_t y) override;

    void arcTo(number_t x1, number_t y1, number_t x2, number_t y2, number_t r) override;

    void arc(number_t x, number_t y, number_t r, number_t a0, number_t a1, bool ccw = false) override;

    void rect(number_t x, number_t y, number_t w, number_t h) override;

    std::string toString() const override;
};

} // namespace d3_path

#endif // D3__PATH__PATH_HPP
