#ifndef OSMIUM_HANDLER_NODE_LOCATIONS_FOR_WAYS_HPP
#define OSMIUM_HANDLER_NODE_LOCATIONS_FOR_WAYS_HPP

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

#include <type_traits>

#include <osmium/handler.hpp>
#include <osmium/index/index.hpp>
#include <osmium/index/map/dummy.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/node_ref.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/osm/way.hpp>

#include <osmium/index/node_locations_map.hpp>

namespace osrm_osmium {

    namespace handler {

        typedef osrm_osmium::index::map::Dummy<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location> dummy_type;

        /**
         * Handler to retrieve locations from nodes and add them to ways.
         *
         * @tparam TStoragePosIDs Class that handles the actual storage of the node locations
         *                        (for positive IDs). It must support the set(id, value) and
         *                        get(id) methods.
         * @tparam TStorageNegIDs Same but for negative IDs.
         */
        template <typename TStoragePosIDs, typename TStorageNegIDs = dummy_type>
        class NodeLocationsForWays : public osrm_osmium::handler::Handler {

            static_assert(std::is_base_of<osrm_osmium::index::map::Map<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location>, TStoragePosIDs>::value, "Index class must be derived from osrm_osmium::index::map::Map<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location>");

            static_assert(std::is_base_of<osrm_osmium::index::map::Map<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location>, TStorageNegIDs>::value, "Index class must be derived from osrm_osmium::index::map::Map<osrm_osmium::unsigned_object_id_type, osrm_osmium::Location>");

        public:

            typedef TStoragePosIDs index_pos_type;
            typedef TStorageNegIDs index_neg_type;

        private:

            /// Object that handles the actual storage of the node locations (with positive IDs).
            TStoragePosIDs& m_storage_pos;

            /// Object that handles the actual storage of the node locations (with negative IDs).
            TStorageNegIDs& m_storage_neg;

            bool m_ignore_errors {false};

            bool m_must_sort {false};

            // It is okay to have this static dummy instance, even when using several threads,
            // because it is read-only.
            static dummy_type& get_dummy() {
                static dummy_type instance;
                return instance;
            }

        public:

            explicit NodeLocationsForWays(TStoragePosIDs& storage_pos,
                                          TStorageNegIDs& storage_neg = get_dummy()) :
                m_storage_pos(storage_pos),
                m_storage_neg(storage_neg) {
            }

            NodeLocationsForWays(const NodeLocationsForWays&) = delete;
            NodeLocationsForWays& operator=(const NodeLocationsForWays&) = delete;

            NodeLocationsForWays(NodeLocationsForWays&&) = default;
            NodeLocationsForWays& operator=(NodeLocationsForWays&&) = default;

            ~NodeLocationsForWays() noexcept = default;

            void ignore_errors() {
                m_ignore_errors = true;
            }

            /**
             * Store the location of the node in the storage.
             */
            void node(const osrm_osmium::Node& node) {
                m_must_sort = true;
                const osrm_osmium::object_id_type id = node.id();
                if (id >= 0) {
                    m_storage_pos.set(static_cast<osrm_osmium::unsigned_object_id_type>( id), node.location());
                } else {
                    m_storage_neg.set(static_cast<osrm_osmium::unsigned_object_id_type>(-id), node.location());
                }
            }

            /**
             * Get location of node with given id.
             */
            osrm_osmium::Location get_node_location(const osrm_osmium::object_id_type id) const {
                if (id >= 0) {
                    return m_storage_pos.get(static_cast<osrm_osmium::unsigned_object_id_type>( id));
                } else {
                    return m_storage_neg.get(static_cast<osrm_osmium::unsigned_object_id_type>(-id));
                }
            }

            /**
             * Retrieve locations of all nodes in the way from storage and add
             * them to the way object.
             */
            void way(osrm_osmium::Way& way) {
                if (m_must_sort) {
                    m_storage_pos.sort();
                    m_storage_neg.sort();
                    m_must_sort = false;
                }
                bool error = false;
                for (auto& node_ref : way.nodes()) {
                    try {
                        node_ref.set_location(get_node_location(node_ref.ref()));
                        if (!node_ref.location()) {
                            error = true;
                        }
                    } catch (osrm_osmium::not_found&) {
                        error = true;
                    }
                }
                if (error && !m_ignore_errors) {
                    throw osrm_osmium::not_found("location for one or more nodes not found in node location index");
                }
            }

            /**
             * Call clear on the location indexes. Makes the
             * NodeLocationsForWays handler unusable. Used to explicitly free
             * memory if thats needed.
             */
            void clear() {
                m_storage_pos.clear();
                m_storage_neg.clear();
            }

        }; // class NodeLocationsForWays

    } // namespace handler

} // namespace osrm_osmium

#endif // OSMIUM_HANDLER_NODE_LOCATIONS_FOR_WAYS_HPP
