#ifndef OSMIUM_HANDLER_DISK_STORE_HPP
#define OSMIUM_HANDLER_DISK_STORE_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013-2016 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <cstddef>

#include <osmium/handler.hpp>
#include <osmium/index/map.hpp>
#include <osmium/io/detail/read_write.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/memory/item_iterator.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/relation.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/visitor.hpp>

namespace osrm_osmium {

    namespace handler {

        /**
         *
         * Note: This handler will only work if either all object IDs are
         *       positive or all object IDs are negative.
         */
        class DiskStore : public osrm_osmium::handler::Handler {

            typedef osrm_osmium::index::map::Map<unsigned_object_id_type, size_t> offset_index_type;

            size_t m_offset = 0;
            int m_data_fd;

            offset_index_type& m_node_index;
            offset_index_type& m_way_index;
            offset_index_type& m_relation_index;

        public:

            explicit DiskStore(int data_fd, offset_index_type& node_index, offset_index_type& way_index, offset_index_type& relation_index) :
                m_data_fd(data_fd),
                m_node_index(node_index),
                m_way_index(way_index),
                m_relation_index(relation_index) {
            }

            DiskStore(const DiskStore&) = delete;
            DiskStore& operator=(const DiskStore&) = delete;

            ~DiskStore() noexcept = default;

            void node(const osrm_osmium::Node& node) {
                m_node_index.set(node.positive_id(), m_offset);
                m_offset += node.byte_size();
            }

            void way(const osrm_osmium::Way& way) {
                m_way_index.set(way.positive_id(), m_offset);
                m_offset += way.byte_size();
            }

            void relation(const osrm_osmium::Relation& relation) {
                m_relation_index.set(relation.positive_id(), m_offset);
                m_offset += relation.byte_size();
            }

            // XXX
            void operator()(const osrm_osmium::memory::Buffer& buffer) {
                osrm_osmium::io::detail::reliable_write(m_data_fd, buffer.data(), buffer.committed());

                osrm_osmium::apply(buffer.begin(), buffer.end(), *this);
            }

        }; // class DiskStore

    } // namespace handler

} // namespace osrm_osmium

#endif // OSMIUM_HANDLER_DISK_STORE_HPP
