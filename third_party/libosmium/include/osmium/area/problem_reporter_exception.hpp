#ifndef OSMIUM_AREA_PROBLEM_REPORTER_EXCEPTION_HPP
#define OSMIUM_AREA_PROBLEM_REPORTER_EXCEPTION_HPP

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

#include <sstream>
#include <stdexcept>

#include <osmium/area/problem_reporter_stream.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/types.hpp>

namespace osrm_osmium {

    namespace area {

        class ProblemReporterException : public ProblemReporterStream {

            std::stringstream m_sstream;

        public:

            ProblemReporterException() :
                ProblemReporterStream(m_sstream) {
            }

            ~ProblemReporterException() override = default;

            void report_duplicate_node(osrm_osmium::object_id_type node_id1, osrm_osmium::object_id_type node_id2, osrm_osmium::Location location) override {
                m_sstream.str();
                ProblemReporterStream::report_duplicate_node(node_id1, node_id2, location);
                throw std::runtime_error(m_sstream.str());
            }

            void report_intersection(osrm_osmium::object_id_type way1_id, osrm_osmium::Location way1_seg_start, osrm_osmium::Location way1_seg_end,
                                     osrm_osmium::object_id_type way2_id, osrm_osmium::Location way2_seg_start, osrm_osmium::Location way2_seg_end, osrm_osmium::Location intersection) override {
                m_sstream.str();
                ProblemReporterStream::report_intersection(way1_id, way1_seg_start, way1_seg_end, way2_id, way2_seg_start, way2_seg_end, intersection);
                throw std::runtime_error(m_sstream.str());
            }

            void report_ring_not_closed(osrm_osmium::Location end1, osrm_osmium::Location end2) override {
                m_sstream.str();
                ProblemReporterStream::report_ring_not_closed(end1, end2);
                throw std::runtime_error(m_sstream.str());
            }

            void report_role_should_be_outer(osrm_osmium::object_id_type way_id, osrm_osmium::Location seg_start, osrm_osmium::Location seg_end) override {
                m_sstream.str();
                ProblemReporterStream::report_role_should_be_outer(way_id, seg_start, seg_end);
                throw std::runtime_error(m_sstream.str());
            }

            void report_role_should_be_inner(osrm_osmium::object_id_type way_id, osrm_osmium::Location seg_start, osrm_osmium::Location seg_end) override {
                m_sstream.str();
                ProblemReporterStream::report_role_should_be_inner(way_id, seg_start, seg_end);
                throw std::runtime_error(m_sstream.str());
            }

        }; // class ProblemReporterException

    } // namespace area

} // namespace osrm_osmium

#endif // OSMIUM_AREA_PROBLEM_REPORTER_EXCEPTION_HPP
