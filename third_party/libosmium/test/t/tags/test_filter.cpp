#include "catch.hpp"

#include <algorithm>

#include <osmium/builder/attr.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/osm/tag.hpp>
#include <osmium/tags/taglist.hpp>
#include <osmium/tags/filter.hpp>
#include <osmium/tags/regex_filter.hpp>

template <class TFilter>
void check_filter(const osrm_osmium::TagList& tag_list, const TFilter filter, const std::vector<bool>& reference) {
    REQUIRE(tag_list.size() == reference.size());
    auto t_it = tag_list.begin();
    for (auto it = reference.begin(); it != reference.end(); ++t_it, ++it) {
        REQUIRE(filter(*t_it) == *it);
    }

    typename TFilter::iterator fi_begin(filter, tag_list.begin(), tag_list.end());
    typename TFilter::iterator fi_end(filter, tag_list.end(), tag_list.end());

    REQUIRE(std::distance(fi_begin, fi_end) == std::count(reference.begin(), reference.end(), true));
}

const osrm_osmium::TagList& make_tag_list(osrm_osmium::memory::Buffer& buffer, std::initializer_list<std::pair<const char*, const char*>> tags) {
    auto pos = osrm_osmium::builder::add_tag_list(buffer, osrm_osmium::builder::attr::_tags(tags));
    return buffer.get<osrm_osmium::TagList>(pos);
}


TEST_CASE("Filter") {

    SECTION("KeyFilter_matches_some_tags") {
        osrm_osmium::tags::KeyFilter filter(false);
        filter.add(true, "highway").add(true, "railway");

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },  // match
            { "name", "Main Street" }, // no match
            { "source", "GPS" }        // no match
        });

        std::vector<bool> results = { true, false, false };

        check_filter(tag_list, filter, results);
    }

    SECTION("KeyFilter_iterator_filters_tags") {
        osrm_osmium::tags::KeyFilter filter(false);
        filter.add(true, "highway").add(true, "source");

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tl = make_tag_list(buffer, {
            { "highway", "primary" },  // match
            { "name", "Main Street" }, // no match
            { "source", "GPS" }        // no match
        });

        osrm_osmium::tags::KeyFilter::iterator it(filter, tl.begin(), tl.end());
        const osrm_osmium::tags::KeyFilter::iterator end(filter, tl.end(), tl.end());

        REQUIRE(2 == std::distance(it, end));

        REQUIRE(it != end);
        REQUIRE(std::string("highway") == it->key());
        REQUIRE(std::string("primary") == it->value());
        ++it;
        REQUIRE(std::string("source") == it->key());
        REQUIRE(std::string("GPS") == it->value());
        REQUIRE(++it == end);
    }

    SECTION("KeyValueFilter_matches_some_tags") {
        osrm_osmium::tags::KeyValueFilter filter(false);

        filter.add(true, "highway", "residential").add(true, "highway", "primary").add(true, "railway");

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },
            { "railway", "tram" },
            { "source", "GPS" }
        });

        std::vector<bool> results = {true, true, false};

        check_filter(tag_list, filter, results);
    }

    SECTION("KeyValueFilter_ordering_matters") {
        osrm_osmium::tags::KeyValueFilter filter1(false);
        filter1.add(true, "highway").add(false, "highway", "road");

        osrm_osmium::tags::KeyValueFilter filter2(false);
        filter2.add(false, "highway", "road").add(true, "highway");

        osrm_osmium::memory::Buffer buffer(10240);

        const osrm_osmium::TagList& tag_list1 = make_tag_list(buffer, {
            { "highway", "road" },
            { "name", "Main Street" }
        });

        const osrm_osmium::TagList& tag_list2 = make_tag_list(buffer, {
            { "highway", "primary" },
            { "name", "Main Street" }
        });

        check_filter(tag_list1, filter1, {true, false});
        check_filter(tag_list1, filter2, {false, false});
        check_filter(tag_list2, filter2, {true, false});
    }

    SECTION("KeyValueFilter_matches_against_taglist_with_any") {
        osrm_osmium::tags::KeyValueFilter filter(false);

        filter.add(true, "highway", "primary").add(true, "name");

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },
            { "railway", "tram" },
            { "source", "GPS" }
        });

        REQUIRE( osrm_osmium::tags::match_any_of(tag_list, filter));
        REQUIRE(!osrm_osmium::tags::match_all_of(tag_list, filter));
        REQUIRE(!osrm_osmium::tags::match_none_of(tag_list, filter));
    }

    SECTION("KeyValueFilter_matches_against_taglist_with_all") {
        osrm_osmium::tags::KeyValueFilter filter(false);

        filter.add(true, "highway", "primary").add(true, "name");

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },
            { "name", "Main Street" }
        });

        REQUIRE( osrm_osmium::tags::match_any_of(tag_list, filter));
        REQUIRE( osrm_osmium::tags::match_all_of(tag_list, filter));
        REQUIRE(!osrm_osmium::tags::match_none_of(tag_list, filter));
    }

    SECTION("KeyValueFilter_matches_against_taglist_with_none") {
        osrm_osmium::tags::KeyValueFilter filter(false);

        filter.add(true, "highway", "road").add(true, "source");

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },
            { "name", "Main Street" }
        });

        REQUIRE(!osrm_osmium::tags::match_any_of(tag_list, filter));
        REQUIRE(!osrm_osmium::tags::match_all_of(tag_list, filter));
        REQUIRE( osrm_osmium::tags::match_none_of(tag_list, filter));
    }

    SECTION("KeyValueFilter_matches_against_taglist_with_any_called_with_rvalue") {
        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },
            { "railway", "tram" },
            { "source", "GPS" }
        });

        REQUIRE(osrm_osmium::tags::match_any_of(tag_list,
                                           osrm_osmium::tags::KeyValueFilter().add(true, "highway", "primary").add(true, "name")));
    }

    SECTION("RegexFilter_matches_some_tags") {
        osrm_osmium::tags::RegexFilter filter(false);
        filter.add(true, "highway", std::regex(".*_link"));

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list1 = make_tag_list(buffer, {
            { "highway", "primary_link" },
            { "source", "GPS" }
        });
        const osrm_osmium::TagList& tag_list2 = make_tag_list(buffer, {
            { "highway", "primary" },
            { "source", "GPS" }
        });

        check_filter(tag_list1, filter, {true, false});
        check_filter(tag_list2, filter, {false, false});
    }

    SECTION("RegexFilter_matches_some_tags_with_lvalue_regex") {
        osrm_osmium::tags::RegexFilter filter(false);
        std::regex r(".*straße");
        filter.add(true, "name", r);

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },
            { "name", "Hauptstraße" }
        });

        check_filter(tag_list, filter, {false, true});
    }

    SECTION("KeyPrefixFilter_matches_some_tags") {
        osrm_osmium::tags::KeyPrefixFilter filter(false);
        filter.add(true, "name:");

        osrm_osmium::memory::Buffer buffer(10240);
        const osrm_osmium::TagList& tag_list = make_tag_list(buffer, {
            { "highway", "primary" },
            { "name:de", "Hauptstraße" }
        });

        check_filter(tag_list, filter, {false, true});
    }

}
