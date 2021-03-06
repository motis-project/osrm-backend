/*

  This reads an OSM file and writes out the node locations to a cache
  file.

  The code in this example file is released into the Public Domain.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#include <osmium/io/any_input.hpp>

#include <osmium/index/map/dummy.hpp>
#include <osmium/index/map/dense_mmap_array.hpp>
#include <osmium/index/map/dense_file_array.hpp>

#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/visitor.hpp>

typedef osrm_osmium::index::map::Dummy<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location> index_neg_type;
//typedef osrm_osmium::index::map::DenseMmapArray<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location> index_pos_type;
typedef osrm_osmium::index::map::DenseFileArray<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location> index_pos_type;

typedef osrm_osmium::handler::NodeLocationsForWays<index_pos_type, index_neg_type> location_handler_type;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " OSM_FILE CACHE_FILE\n";
        return 1;
    }

    std::string input_filename(argv[1]);
    osrm_osmium::io::Reader reader(input_filename, osrm_osmium::osm_entity_bits::node);

    int fd = open(argv[2], O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        std::cerr << "Can not open node cache file '" << argv[2] << "': " << strerror(errno) << "\n";
        return 1;
    }

    index_pos_type index_pos {fd};
    index_neg_type index_neg;
    location_handler_type location_handler(index_pos, index_neg);
    location_handler.ignore_errors();

    osrm_osmium::apply(reader, location_handler);
    reader.close();

    return 0;
}

