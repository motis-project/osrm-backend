/*

  The code in this file is released into the Public Domain.

*/

#include <cstdint>
#include <vector>

#include <osmium/io/any_input.hpp>
#include <osmium/io/any_output.hpp>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT-FILE OUTPUT-FILE\n";
        exit(1);
    }

    std::string input_filename = argv[1];
    std::string output_filename = argv[2];

    osrm_osmium::io::Reader reader(input_filename);
    osrm_osmium::io::File output_file(output_filename, "pbf");
    osrm_osmium::io::Header header;
    osrm_osmium::io::Writer writer(output_file, header, osrm_osmium::io::overwrite::allow);

    while (osrm_osmium::memory::Buffer buffer = reader.read()) {
        writer(std::move(buffer));
    }

    writer.close();
    reader.close();
}

