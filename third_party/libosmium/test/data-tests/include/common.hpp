#ifndef COMMON_HPP
#define COMMON_HPP

#include <osmium/index/map/dummy.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>

#include <osmium/geom/wkt.hpp>
#include <osmium/handler.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/io/xml_input.hpp>
#include <osmium/visitor.hpp>

typedef osrm_osmium::index::map::Dummy<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location> index_neg_type;
typedef osrm_osmium::index::map::SparseMemArray<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location> index_pos_type;
typedef osrm_osmium::handler::NodeLocationsForWays<index_pos_type, index_neg_type> location_handler_type;

#include "check_basics_handler.hpp"
#include "check_wkt_handler.hpp"

#include "testdata-testcases.hpp"

#endif // COMMON_HPP
