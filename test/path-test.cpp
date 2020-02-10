#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"


#include "pathEqual.hpp"

#include "../src/d3_path/path.hpp"


TEST_CASE("path is an instanceof path") {
    auto p = d3_path::path();
    REQUIRE( std::is_same< decltype(p), d3_path::Path>::value == true);
    REQUIRE_THAT( p, pathEqual("") );
}

TEST_CASE("path.moveTo(x, y) appends an M command") {
    auto p = d3_path::path(); p.moveTo(150, 50);
    REQUIRE_THAT(p, pathEqual("M150,50") );
    p.lineTo(200, 100);
    REQUIRE_THAT(p, pathEqual("M150,50L200,100") );
    p.moveTo(100, 50);
    REQUIRE_THAT(p, pathEqual("M150,50L200,100M100,50") );
}

TEST_CASE("path.closePath() appends a Z command") {
    auto p = d3_path::path(); p.moveTo(150, 50);
    REQUIRE_THAT(p, pathEqual("M150,50") );
    p.closePath();
    REQUIRE_THAT(p, pathEqual("M150,50Z") );
    p.closePath();
    REQUIRE_THAT(p, pathEqual("M150,50ZZ") );
}

TEST_CASE("path.closePath() does nothing if the path is empty") {
    auto p = d3_path::path();
    REQUIRE_THAT(p, pathEqual("") );
    p.closePath();
    REQUIRE_THAT(p, pathEqual("") );
}

TEST_CASE("path.lineTo(x, y) appends an L command") {
    auto p = d3_path::path(); p.moveTo(150, 50);
    REQUIRE_THAT(p, pathEqual("M150,50") );
    p.lineTo(200, 100);
    REQUIRE_THAT(p, pathEqual("M150,50L200,100") );
    p.lineTo(100, 50);
    REQUIRE_THAT(p, pathEqual("M150,50L200,100L100,50") );
}

TEST_CASE("path.quadraticCurveTo(x1, y1, x, y) appends a Q command") {
    auto p = d3_path::path(); p.moveTo(150, 50);
    REQUIRE_THAT(p, pathEqual("M150,50") );
    p.quadraticCurveTo(100, 50, 200, 100);
    REQUIRE_THAT(p, pathEqual("M150,50Q100,50,200,100") );
}

TEST_CASE("path.bezierCurveTo(x1, y1, x, y) appends a C command") {
    auto p = d3_path::path(); p.moveTo(150, 50);
    REQUIRE_THAT(p, pathEqual("M150,50") );
    p.bezierCurveTo(100, 50, 0, 24, 200, 100);
    REQUIRE_THAT(p, pathEqual("M150,50C100,50,0,24,200,100") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) throws an error if the radius is negative") {
    auto p = d3_path::path(); p.moveTo(150, 100);
    REQUIRE_THROWS_WITH( p.arc(100, 100, -50, 0, M_PI / 2), Catch::Matchers::Contains("negative radius") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) may append only an M command if the radius is zero") {
    auto p = d3_path::path(); p.arc(100, 100, 0, 0, M_PI / 2);
    REQUIRE_THAT(p, pathEqual("M100,100") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) may append only an L command if the radius is zero") {
    auto p = d3_path::path(); p.moveTo(0, 0); p.arc(100, 100, 0, 0, M_PI / 2);
    REQUIRE_THAT(p, pathEqual("M0,0L100,100") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) may append only an M command if the angle is zero") {
    auto p = d3_path::path(); p.arc(100, 100, 0, 0, 0);
    REQUIRE_THAT(p, pathEqual("M100,100") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) may append only an M command if the angle is near zero") {
    auto p = d3_path::path(); p.arc(100, 100, 0, 0, 1e-16);
    REQUIRE_THAT(p, pathEqual("M100,100") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) may append an M command if the path was empty") {
    auto p = d3_path::path(); p.arc(100, 100, 50, 0, M_PI * 2);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
    p = d3_path::path(); p.arc(0, 50, 50, -M_PI / 2, 0);
    REQUIRE_THAT(p, pathEqual("M0,0A50,50,0,0,1,50,50") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) may append an L command if the arc doesn’t start at the current point") {
    auto p = d3_path::path(); p.moveTo(100, 100); p.arc(100, 100, 50, 0, M_PI * 2);
    REQUIRE_THAT(p, pathEqual("M100,100L150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) appends a single A command if the angle is less than π") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, M_PI / 2);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,0,1,100,150") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) appends a single A command if the angle is less than τ") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, M_PI * 1);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100") );
}

TEST_CASE("path.arc(x, y, radius, startAngle, endAngle) appends two A commands if the angle is greater than τ") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, M_PI * 2);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, π/2, false) draws a small clockwise arc") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, M_PI / 2, false);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,0,1,100,150") );
}

TEST_CASE("path.arc(x, y, radius, -π/2, 0, false) draws a small clockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arc(100, 100, 50, -M_PI / 2, 0, false);
    REQUIRE_THAT(p, pathEqual("M100,50A50,50,0,0,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, ε, true) draws an anticlockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 1e-16, true);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,0,50,100A50,50,0,1,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, ε, false) draws nothing") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 1e-16, false);
    REQUIRE_THAT(p, pathEqual("M150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, -ε, true) draws nothing") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, -1e-16, true);
    REQUIRE_THAT(p, pathEqual("M150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, -ε, false) draws a clockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, -1e-16, false);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, τ, true) draws an anticlockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 2 * M_PI, true);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,0,50,100A50,50,0,1,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, τ, false) draws a clockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 2 * M_PI, false);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, τ + ε, true) draws an anticlockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 2 * M_PI + 1e-13, true);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,0,50,100A50,50,0,1,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, τ - ε, false) draws a clockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 2 * M_PI - 1e-13, false);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, τ, 0, true) draws an anticlockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 2 * M_PI, true);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,0,50,100A50,50,0,1,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, τ, 0, false) draws a clockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 2 * M_PI, false);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, 13π/2, false) draws a clockwise circle") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 13 * M_PI / 2, false);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,50,100A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 13π/2, 0, false) draws a big clockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 150); p.arc(100, 100, 50, 13 * M_PI / 2, 0, false);
    REQUIRE_THAT(p, pathEqual("M100,150A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, π/2, 0, false) draws a big clockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 150); p.arc(100, 100, 50, M_PI / 2, 0, false);
    REQUIRE_THAT(p, pathEqual("M100,150A50,50,0,1,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 3π/2, 0, false) draws a small clockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arc(100, 100, 50, 3 * M_PI / 2, 0, false);
    REQUIRE_THAT(p, pathEqual("M100,50A50,50,0,0,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 15π/2, 0, false) draws a small clockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arc(100, 100, 50, 15 * M_PI / 2, 0, false);
    REQUIRE_THAT(p, pathEqual("M100,50A50,50,0,0,1,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 0, π/2, true) draws a big anticlockwise arc") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, M_PI / 2, true);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,0,100,150") );
}

TEST_CASE("path.arc(x, y, radius, -π/2, 0, true) draws a big anticlockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arc(100, 100, 50, -M_PI / 2, 0, true);
    REQUIRE_THAT(p, pathEqual("M100,50A50,50,0,1,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, -13π/2, 0, true) draws a big anticlockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arc(100, 100, 50, -13 * M_PI / 2, 0, true);
    REQUIRE_THAT(p, pathEqual("M100,50A50,50,0,1,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, -13π/2, 0, false) draws a big clockwise arc") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, -13 * M_PI / 2, false);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,1,100,50") );
}

TEST_CASE("path.arc(x, y, radius, 0, 13π/2, true) draws a big anticlockwise arc") {
    auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, 13 * M_PI / 2, true);
    REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,1,0,100,150") );
}

TEST_CASE("path.arc(x, y, radius, π/2, 0, true) draws a small anticlockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 150); p.arc(100, 100, 50, M_PI / 2, 0, true);
    REQUIRE_THAT(p, pathEqual("M100,150A50,50,0,0,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, 3π/2, 0, true) draws a big anticlockwise arc") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arc(100, 100, 50, 3 * M_PI / 2, 0, true);
    REQUIRE_THAT(p, pathEqual("M100,50A50,50,0,1,0,150,100") );
}

TEST_CASE("path.arc(x, y, radius, π/2, 0, truthy) draws a small anticlockwise arc") {
    for (const double trueish : std::initializer_list<double>{1, /*"1",*/ true, 10, /*"3", "string"*/}) {
        auto p = d3_path::path(); p.moveTo(100, 150); p.arc(100, 100, 50, M_PI / 2, 0, trueish);
        REQUIRE_THAT(p, pathEqual("M100,150A50,50,0,0,0,150,100") );
    }
}

TEST_CASE("path.arc(x, y, radius, 0, π/2, falsy) draws a small clockwise arc") {
    for (const double falseish : std::initializer_list<double>{0, NULL, false/*undefined*/}) {
        auto p = d3_path::path(); p.moveTo(150, 100); p.arc(100, 100, 50, 0, M_PI / 2, falseish);
        REQUIRE_THAT(p, pathEqual("M150,100A50,50,0,0,1,100,150") );
    }
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) throws an error if the radius is negative") {
    auto p = d3_path::path(); p.moveTo(150, 100);
    REQUIRE_THROWS_WITH( p.arcTo(270, 39, 163, 100, -53), Catch::Matchers::Contains("negative radius") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) appends an M command if the path was empty") {
    auto p = d3_path::path(); p.arcTo(270, 39, 163, 100, 53);
    REQUIRE_THAT(p, pathEqual("M270,39") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) does nothing if the previous point was ⟨x1,y1⟩") {
    auto p = d3_path::path(); p.moveTo(270, 39); p.arcTo(270, 39, 163, 100, 53);
    REQUIRE_THAT(p, pathEqual("M270,39") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) appends an L command if the previous point, ⟨x1,y1⟩ and ⟨x2,y2⟩ are collinear") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arcTo(101, 51, 102, 52, 10);
    REQUIRE_THAT(p, pathEqual("M100,50L101,51") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) appends an L command if ⟨x1,y1⟩ and ⟨x2,y2⟩ are coincident") {
    auto p = d3_path::path(); p.moveTo(100, 50); p.arcTo(101, 51, 101, 51, 10);
    REQUIRE_THAT(p, pathEqual("M100,50L101,51") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) appends an L command if the radius is zero") {
    auto p = d3_path::path(); p.moveTo(270, 182), p.arcTo(270, 39, 163, 100, 0);
    REQUIRE_THAT(p, pathEqual("M270,182L270,39") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) appends L and A commands if the arc does not start at the current point") {
    auto p = d3_path::path(); p.moveTo(270, 182), p.arcTo(270, 39, 163, 100, 53);
    REQUIRE_THAT(p, pathEqual("M270,182L270,130.222686A53,53,0,0,0,190.750991,84.179342") );
    p = d3_path::path(); p.moveTo(270, 182), p.arcTo(270, 39, 363, 100, 53);
    REQUIRE_THAT(p, pathEqual("M270,182L270,137.147168A53,53,0,0,1,352.068382,92.829799") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) appends only an A command if the arc starts at the current point") {
    auto p = d3_path::path(); p.moveTo(100, 100), p.arcTo(200, 100, 200, 200, 100);
    REQUIRE_THAT(p, pathEqual("M100,100A100,100,0,0,1,200,200") );
}

TEST_CASE("path.arcTo(x1, y1, x2, y2, radius) sets the last point to be the end tangent of the arc") {
    auto p = d3_path::path(); p.moveTo(100, 100), p.arcTo(200, 100, 200, 200, 50); p.arc(150, 150, 50, 0, M_PI);
    REQUIRE_THAT(p, pathEqual("M100,100L150,100A50,50,0,0,1,200,150A50,50,0,1,1,100,150") );
}

TEST_CASE("path.rect(x, y, w, h) appends M, h, v, h, and Z commands") {
    auto p = d3_path::path(); p.moveTo(150, 100), p.rect(100, 200, 50, 25);
    REQUIRE_THAT(p, pathEqual("M150,100M100,200h50v25h-50Z") );
}
