#include "catch.hpp"

#include <osmium/util/minmax.hpp>
#include <osmium/osm/timestamp.hpp>

TEST_CASE("minmax numeric") {

    SECTION("min") {
        osrm_osmium::min_op<int> x;
        REQUIRE(x() == std::numeric_limits<int>::max());

        x.update(17);
        REQUIRE(x() == 17);

        x.update(10);
        REQUIRE(x() == 10);

        x.update(22);
        REQUIRE(x() == 10);
    }

    SECTION("max") {
        osrm_osmium::max_op<uint32_t> x;
        REQUIRE(x() == 0);

        x.update(17);
        REQUIRE(x() == 17);

        x.update(10);
        REQUIRE(x() == 17);

        x.update(22);
        REQUIRE(x() == 22);
    }

}

TEST_CASE("minmax timestamp") {

    SECTION("min") {
        osrm_osmium::min_op<osrm_osmium::Timestamp> x;

        x.update(osrm_osmium::Timestamp("2010-01-01T00:00:00Z"));
        REQUIRE(x().to_iso() == "2010-01-01T00:00:00Z");

        x.update(osrm_osmium::Timestamp("2015-01-01T00:00:00Z"));
        REQUIRE(x().to_iso() == "2010-01-01T00:00:00Z");

        x.update(osrm_osmium::Timestamp("2000-01-01T00:00:00Z"));
        REQUIRE(x().to_iso() == "2000-01-01T00:00:00Z");
    }

    SECTION("max") {
        osrm_osmium::max_op<osrm_osmium::Timestamp> x;

        x.update(osrm_osmium::Timestamp("2010-01-01T00:00:00Z"));
        REQUIRE(x().to_iso() == "2010-01-01T00:00:00Z");

        x.update(osrm_osmium::Timestamp("2015-01-01T00:00:00Z"));
        REQUIRE(x().to_iso() == "2015-01-01T00:00:00Z");

        x.update(osrm_osmium::Timestamp("2000-01-01T00:00:00Z"));
        REQUIRE(x().to_iso() == "2015-01-01T00:00:00Z");

    }

}

