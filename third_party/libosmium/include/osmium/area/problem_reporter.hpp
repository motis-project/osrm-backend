#ifndef OSMIUM_AREA_PROBLEM_REPORTER_HPP
#define OSMIUM_AREA_PROBLEM_REPORTER_HPP

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

#include <osmium/osm/item_type.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/types.hpp>

namespace osrm_osmium {

    namespace area {

        /**
         * When assembling a multipolygon/area from a multipolygon relation
         * or a closed way several problems can be detected. This includes
         * intersections between lines, wrong role attributes on relation
         * members etc. These problems are reported by the area::Assembler
         * class to the ProblemReporter class or one of its child classes.
         *
         * This is the parent class which does nothing with the reports.
         * Child classes are expected to implement different ways of
         * reporting the problems.
         */
        class ProblemReporter {

        protected:

            // Type of object we are currently working on
            osrm_osmium::item_type m_object_type;

            // ID of the relation/way we are currently working on
            osrm_osmium::object_id_type m_object_id;

        public:

            ProblemReporter() = default;

            virtual ~ProblemReporter() = default;

            /**
             * Set the object the next problem reports will be on.
             *
             * @param object_type The type of the object.
             * @param object_id The ID of the object.
             */
            void set_object(osrm_osmium::item_type object_type, osrm_osmium::object_id_type object_id) noexcept {
                m_object_type = object_type;
                m_object_id = object_id;
            }

// Disable "unused-parameter" warning, so that the compiler will not complain.
// We can't remove the parameter names, because then doxygen will complain.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

            /**
             * Report a duplicate node, ie. two nodes with the same location.
             *
             * @param node_id1       ID of the first node.
             * @param node_id2       ID of the second node.
             * @param location       Location of both nodes.
             */
            virtual void report_duplicate_node(osrm_osmium::object_id_type node_id1, osrm_osmium::object_id_type node_id2, osrm_osmium::Location location) {
            }

            /**
             * Report an intersection between two segments.
             *
             * @param way1_id        ID of the first involved way.
             * @param way1_seg_start Location where the segment of the first way with the intersection starts
             * @param way1_seg_end   Location where the segment of the first way with the intersection ends
             * @param way2_id        ID of the second involved way.
             * @param way2_seg_start Location where the segment of the second way with the intersection starts
             * @param way2_seg_end   Location where the segment of the second way with the intersection ends
             * @param intersection   Location of the intersection. This might be slightly off the correct location due to rounding.
             */
            virtual void report_intersection(osrm_osmium::object_id_type way1_id, osrm_osmium::Location way1_seg_start, osrm_osmium::Location way1_seg_end,
                                             osrm_osmium::object_id_type way2_id, osrm_osmium::Location way2_seg_start, osrm_osmium::Location way2_seg_end, osrm_osmium::Location intersection) {
            }

            /**
             * Report an open ring.
             *
             * @param end1           Location of the first open end.
             * @param end2           Location of the second open end.
             */
            virtual void report_ring_not_closed(osrm_osmium::Location end1, osrm_osmium::Location end2) {
            }

            /**
             * Report a segment that should have role "outer", but has a different role.
             *
             * @param way_id         ID of the way this segment is in.
             * @param seg_start      Start of the segment with the wrong role.
             * @param seg_end        End of the segment with the wrong role.
             */
            virtual void report_role_should_be_outer(osrm_osmium::object_id_type way_id, osrm_osmium::Location seg_start, osrm_osmium::Location seg_end) {
            }

            /**
             * Report a segment that should have role "inner", but has a different role.
             *
             * @param way_id         ID of the way this segment is in.
             * @param seg_start      Start of the segment with the wrong role.
             * @param seg_end        End of the segment with the wrong role.
             */
            virtual void report_role_should_be_inner(osrm_osmium::object_id_type way_id, osrm_osmium::Location seg_start, osrm_osmium::Location seg_end) {
            }

#pragma GCC diagnostic pop

        }; // class ProblemReporter

    } // namespace area

} // namespace osrm_osmium

#endif // OSMIUM_AREA_PROBLEM_REPORTER_HPP
