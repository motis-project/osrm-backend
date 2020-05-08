#ifndef OSMIUM_OSM_NODE_REF_HPP
#define OSMIUM_OSM_NODE_REF_HPP

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

#include <cstdint>
#include <cstdlib>
#include <iosfwd>

#include <osmium/memory/item.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/types.hpp>

namespace osrm_osmium {

    /**
     * This reference to a node contains a node ID and a (possibly empty)
     * location.
     */
    class NodeRef : public osrm_osmium::memory::detail::ItemHelper {

        osrm_osmium::object_id_type m_ref;
        osrm_osmium::Location m_location;

    public:

        constexpr NodeRef(const osrm_osmium::object_id_type ref = 0, const osrm_osmium::Location& location = Location()) noexcept :
            m_ref(ref),
            m_location(location) {
        }

        /**
         * Get reference ID of this NodeRef.
         */
        constexpr osrm_osmium::object_id_type ref() const noexcept {
            return m_ref;
        }

        /**
         * Get absolute value of the reference ID of this NodeRef.
         */
        osrm_osmium::unsigned_object_id_type positive_ref() const noexcept {
            return static_cast<osrm_osmium::unsigned_object_id_type>(std::abs(m_ref));
        }

        /**
         * Get reference to location in this NodeRef. Can be used to update it.
         */
        osrm_osmium::Location& location() noexcept {
            return m_location;
        }

        /**
         * Get location of this NodeRef.
         */
        constexpr osrm_osmium::Location location() const noexcept {
            return m_location;
        }

        /**
         * Get longitude of the location in this NodeRef.
         *
         * @throws osrm_osmium::invalid_location if the location is not set.
         */
        double lon() const {
            return m_location.lon();
        }

        /**
         * Get latitude of the location in this NodeRef.
         *
         * @throws osrm_osmium::invalid_location if the location is not set.
         */
        double lat() const {
            return m_location.lat();
        }

        /**
         * Get internal x value of the location in this NodeRef.
         */
        constexpr int32_t x() const noexcept {
            return m_location.x();
        }

        /**
         * Get internal y value of the location in this NodeRef.
         */
        constexpr int32_t y() const noexcept {
            return m_location.y();
        }

        /**
         * Set the referenced ID.
         *
         * @returns Reference to this NodeRef for chaining calls.
         */
        NodeRef& set_ref(const osrm_osmium::object_id_type ref) noexcept {
            m_ref = ref;
            return *this;
        }

        /**
         * Set the location.
         *
         * @returns Reference to this NodeRef for chaining calls.
         */
        NodeRef& set_location(const osrm_osmium::Location& location) noexcept {
            m_location = location;
            return *this;
        }

    }; // class NodeRef

    /**
     * Compare two NodeRefs. They are equal if they reference the same Node ID.
     */
    inline constexpr bool operator==(const NodeRef& lhs, const NodeRef& rhs) noexcept {
        return lhs.ref() == rhs.ref();
    }

    /**
     * Compare two NodeRefs. They are not equal if they reference different
     * Node IDs.
     */
    inline constexpr bool operator!=(const NodeRef& lhs, const NodeRef& rhs) noexcept {
        return ! (lhs == rhs);
    }

    /**
     * Compare two NodeRefs. NodeRefs are ordered according to the Node ID
     * they reference.
     */
    inline constexpr bool operator<(const NodeRef& lhs, const NodeRef& rhs) noexcept {
        return lhs.ref() < rhs.ref();
    }

    /**
     * Compare two NodeRefs. NodeRefs are ordered according to the Node ID
     * they reference.
     */
    inline constexpr bool operator>(const NodeRef& lhs, const NodeRef& rhs) noexcept {
        return rhs < lhs;
    }

    /**
     * Compare two NodeRefs. NodeRefs are ordered according to the Node ID
     * they reference.
     */
    inline constexpr bool operator<=(const NodeRef& lhs, const NodeRef& rhs) noexcept {
        return ! (rhs < lhs);
    }

    /**
     * Compare two NodeRefs. NodeRefs are ordered according to the Node ID
     * they reference.
     */
    inline constexpr bool operator>=(const NodeRef& lhs, const NodeRef& rhs) noexcept {
        return ! (lhs < rhs);
    }

    /**
     * Output a NodeRef to a stream.
     */
    template <typename TChar, typename TTraits>
    inline std::basic_ostream<TChar, TTraits>& operator<<(std::basic_ostream<TChar, TTraits>& out, const osrm_osmium::NodeRef& nr) {
        return out << "<" << nr.ref() << " " << nr.location() << ">";
    }

    /**
     * Functor to compare NodeRefs by Location instead of ID.
     */
    struct location_equal {

        constexpr bool operator()(const NodeRef& lhs, const NodeRef& rhs) const noexcept {
            return lhs.location() == rhs.location();
        }

        using first_argument_type = NodeRef;
        using second_argument_type = NodeRef;
        using result_type = bool;

    }; // struct location_equal

    /**
     * Functor to compare NodeRefs by Location instead of ID.
     */
    struct location_less {

        constexpr bool operator()(const NodeRef& lhs, const NodeRef& rhs) const noexcept {
            return lhs.location() < rhs.location();
        }

        using first_argument_type = NodeRef;
        using second_argument_type = NodeRef;
        using result_type = bool;

    }; // struct location_less

} // namespace osrm_osmium

#endif // OSMIUM_OSM_NODE_REF_HPP
