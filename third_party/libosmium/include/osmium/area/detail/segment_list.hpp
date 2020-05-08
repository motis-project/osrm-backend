#ifndef OSMIUM_AREA_DETAIL_SEGMENT_LIST_HPP
#define OSMIUM_AREA_DETAIL_SEGMENT_LIST_HPP

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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include <osmium/area/problem_reporter.hpp>
#include <osmium/area/detail/node_ref_segment.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/node_ref.hpp>
#include <osmium/osm/relation.hpp>
#include <osmium/osm/way.hpp>

namespace osrm_osmium {

    namespace area {

        namespace detail {

            /**
             * This is a helper class for the area assembler. It models
             * a list of segments.
             */
            class SegmentList {

                using slist_type = std::vector<NodeRefSegment>;

                slist_type m_segments;

                bool m_debug;

            public:

                explicit SegmentList(bool debug) noexcept :
                    m_segments(),
                    m_debug(debug) {
                }

                ~SegmentList() noexcept = default;

                SegmentList(const SegmentList&) = delete;
                SegmentList(SegmentList&&) = delete;

                SegmentList& operator=(const SegmentList&) = delete;
                SegmentList& operator=(SegmentList&&) = delete;

                /// The number of segments in the list.
                size_t size() const noexcept {
                    return m_segments.size();
                }

                bool empty() const noexcept {
                    return m_segments.empty();
                }

                typedef slist_type::const_iterator const_iterator;
                typedef slist_type::iterator iterator;

                iterator begin() noexcept {
                    return m_segments.begin();
                }

                iterator end() noexcept {
                    return m_segments.end();
                }

                const_iterator begin() const noexcept {
                    return m_segments.begin();
                }

                const_iterator end() const noexcept {
                    return m_segments.end();
                }

                /**
                 * Enable or disable debug output to stderr. This is used
                 * for debugging libosmium itself.
                 */
                void enable_debug_output(bool debug = true) noexcept {
                    m_debug = debug;
                }

                /// Clear the list of segments. All segments are removed.
                void clear() {
                    m_segments.clear();
                }

                /// Sort the list of segments.
                void sort() {
                    std::sort(m_segments.begin(), m_segments.end());
                }

                /**
                 * Extract segments from given way and add them to the list.
                 *
                 * Segments connecting two nodes with the same location (ie same
                 * node or different node with same location) are removed.
                 *
                 * XXX should two nodes with same location be reported?
                 */
                void extract_segments_from_way(const osrm_osmium::Way& way, const char* role) {
                    osrm_osmium::NodeRef last_nr;
                    for (const osrm_osmium::NodeRef& nr : way.nodes()) {
                        if (last_nr.location() && last_nr.location() != nr.location()) {
                            m_segments.emplace_back(last_nr, nr, role, &way);
                        }
                        last_nr = nr;
                    }
                }

                /**
                 * Extract all segments from all ways that make up this
                 * multipolygon relation and add them to the list.
                 */
                void extract_segments_from_ways(const osrm_osmium::Relation& relation, const std::vector<size_t>& members, const osrm_osmium::memory::Buffer& in_buffer) {
                    auto member_it = relation.members().begin();
                    for (size_t offset : members) {
                        const osrm_osmium::Way& way = in_buffer.get<const osrm_osmium::Way>(offset);
                        extract_segments_from_way(way, member_it->role());
                        ++member_it;
                    }
                }

                /**
                 * Find duplicate segments (ie same start and end point) in the
                 * list and remove them. This will always remove pairs of the
                 * same segment. So if there are three, for instance, two will
                 * be removed and one will be left.
                 */
                void erase_duplicate_segments() {
                    while (true) {
                        auto it = std::adjacent_find(m_segments.begin(), m_segments.end());
                        if (it == m_segments.end()) {
                            return;
                        }
                        if (m_debug) {
                            std::cerr << "  erase duplicate segment: " << *it << "\n";
                        }
                        m_segments.erase(it, it+2);
                    }
                }

                /**
                 * Find intersection between segments.
                 *
                 * @param problem_reporter Any intersections found are
                 *                         reported to this object.
                 * @returns true if there are intersections.
                 */
                bool find_intersections(osrm_osmium::area::ProblemReporter* problem_reporter) const {
                    if (m_segments.empty()) {
                        return false;
                    }

                    bool found_intersections = false;

                    for (auto it1 = m_segments.begin(); it1 != m_segments.end()-1; ++it1) {
                        const NodeRefSegment& s1 = *it1;
                        for (auto it2 = it1+1; it2 != m_segments.end(); ++it2) {
                            const NodeRefSegment& s2 = *it2;

                            assert(s1 != s2); // erase_duplicate_segments() should have made sure of that

                            if (outside_x_range(s2, s1)) {
                                break;
                            }

                            if (y_range_overlap(s1, s2)) {
                                osrm_osmium::Location intersection = calculate_intersection(s1, s2);
                                if (intersection) {
                                    found_intersections = true;
                                    if (m_debug) {
                                        std::cerr << "  segments " << s1 << " and " << s2 << " intersecting at " << intersection << "\n";
                                    }
                                    if (problem_reporter) {
                                        problem_reporter->report_intersection(s1.way()->id(), s1.first().location(), s1.second().location(), s2.way()->id(), s2.first().location(), s2.second().location(), intersection);
                                    }
                                }
                            }
                        }
                    }

                    return found_intersections;
                }

            }; // class SegmentList

        } // namespace detail

    } // namespace area

} // namespace osrm_osmium

#endif // OSMIUM_AREA_DETAIL_SEGMENT_LIST_HPP
