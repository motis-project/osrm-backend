#include "catch.hpp"

#include <osmium/geom/geos.hpp>
#include <osmium/geom/wkb.hpp>

#include "helper.hpp"
#include "area_helper.hpp"
#include "wnl_helper.hpp"

TEST_CASE("WKB_Geometry_with_GEOS") {

SECTION("point") {
    osrm_osmium::geom::WKBFactory<> wkb_factory(osrm_osmium::geom::wkb_type::wkb, osrm_osmium::geom::out_type::hex);
    osrm_osmium::geom::GEOSFactory<> geos_factory;

    std::string wkb {wkb_factory.create_point(osrm_osmium::Location(3.2, 4.2))};

    std::unique_ptr<geos::geom::Point> geos_point = geos_factory.create_point(osrm_osmium::Location(3.2, 4.2));
    REQUIRE(geos_to_wkb(geos_point.get()) == wkb);
}


SECTION("linestring") {
    osrm_osmium::geom::WKBFactory<> wkb_factory(osrm_osmium::geom::wkb_type::wkb, osrm_osmium::geom::out_type::hex);
    osrm_osmium::geom::GEOSFactory<> geos_factory;

    osrm_osmium::memory::Buffer buffer(10000);
    auto &wnl = create_test_wnl_okay(buffer);

    {
        std::string wkb = wkb_factory.create_linestring(wnl);
        std::unique_ptr<geos::geom::LineString> geos = geos_factory.create_linestring(wnl);
        REQUIRE(geos_to_wkb(geos.get()) == wkb);
    }

    {
        std::string wkb = wkb_factory.create_linestring(wnl, osrm_osmium::geom::use_nodes::unique, osrm_osmium::geom::direction::backward);
        std::unique_ptr<geos::geom::LineString> geos = geos_factory.create_linestring(wnl, osrm_osmium::geom::use_nodes::unique, osrm_osmium::geom::direction::backward);
        REQUIRE(geos_to_wkb(geos.get()) == wkb);
    }

    {
        std::string wkb = wkb_factory.create_linestring(wnl, osrm_osmium::geom::use_nodes::all);
        std::unique_ptr<geos::geom::LineString> geos = geos_factory.create_linestring(wnl, osrm_osmium::geom::use_nodes::all);
        REQUIRE(geos_to_wkb(geos.get()) == wkb);
    }

    {
        std::string wkb = wkb_factory.create_linestring(wnl, osrm_osmium::geom::use_nodes::all, osrm_osmium::geom::direction::backward);
        std::unique_ptr<geos::geom::LineString> geos = geos_factory.create_linestring(wnl, osrm_osmium::geom::use_nodes::all, osrm_osmium::geom::direction::backward);
        REQUIRE(geos_to_wkb(geos.get()) == wkb);
    }
}

SECTION("area_1outer_0inner") {
    osrm_osmium::geom::WKBFactory<> wkb_factory(osrm_osmium::geom::wkb_type::wkb, osrm_osmium::geom::out_type::hex);
    osrm_osmium::geom::GEOSFactory<> geos_factory;

    osrm_osmium::memory::Buffer buffer(10000);
    const osrm_osmium::Area& area = create_test_area_1outer_0inner(buffer);

    std::string wkb = wkb_factory.create_multipolygon(area);
    std::unique_ptr<geos::geom::MultiPolygon> geos = geos_factory.create_multipolygon(area);
    REQUIRE(geos_to_wkb(geos.get()) == wkb);
}

SECTION("area_1outer_1inner") {
    osrm_osmium::geom::WKBFactory<> wkb_factory(osrm_osmium::geom::wkb_type::wkb, osrm_osmium::geom::out_type::hex);
    osrm_osmium::geom::GEOSFactory<> geos_factory;

    osrm_osmium::memory::Buffer buffer(10000);
    const osrm_osmium::Area& area = create_test_area_1outer_1inner(buffer);

    std::string wkb = wkb_factory.create_multipolygon(area);
    std::unique_ptr<geos::geom::MultiPolygon> geos = geos_factory.create_multipolygon(area);
    REQUIRE(geos_to_wkb(geos.get()) == wkb);
}

SECTION("area_2outer_2inner") {
    osrm_osmium::geom::WKBFactory<> wkb_factory(osrm_osmium::geom::wkb_type::wkb, osrm_osmium::geom::out_type::hex);
    osrm_osmium::geom::GEOSFactory<> geos_factory;

    osrm_osmium::memory::Buffer buffer(10000);
    const osrm_osmium::Area& area = create_test_area_2outer_2inner(buffer);

    std::string wkb = wkb_factory.create_multipolygon(area);
    std::unique_ptr<geos::geom::MultiPolygon> geos = geos_factory.create_multipolygon(area);
    REQUIRE(geos_to_wkb(geos.get()) == wkb);
}

}

