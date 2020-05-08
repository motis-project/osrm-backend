#ifndef OSMIUM_RELATIONS_DETAIL_RELATION_META_HPP
#define OSMIUM_RELATIONS_DETAIL_RELATION_META_HPP

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

#include <cassert>
#include <cstddef>
#include <iosfwd>

namespace osrm_osmium {

    namespace relations {

        /**
         * Helper class for the Collector class.
         *
         * Stores information needed to collect all members of a relation. This
         * includes the offset of the relation in a buffer plus the information
         * needed to add members to this relation.
         */
        class RelationMeta {

            /// The relation we are assembling.
            size_t m_relation_offset;

            /**
             * The number of members still needed before the relation is
             * complete. This will be set to the number of members we are
             * interested in and then count down for every member we find.
             * When it is 0, the relation is complete.
             */
            int m_need_members = 0;

        public:

            /**
             * Initialize an empty RelationMeta. This is needed to zero out
             * relations that have been completed.
             */
            RelationMeta() noexcept :
                m_relation_offset(0) {
            }

            explicit RelationMeta(size_t relation_offset) noexcept :
                m_relation_offset(relation_offset) {
            }

            /**
             * Get offset of relation in buffer.
             */
            size_t relation_offset() const noexcept {
                return m_relation_offset;
            }

            /**
             * Increment the m_need_members counter.
             */
            void increment_need_members() noexcept {
                ++m_need_members;
            }

            /**
             * This decrements the "members needed" counter.
             */
            void got_one_member() {
                assert(m_need_members > 0);
                --m_need_members;
            }

            /**
             * Returns true if all members for this relation are available.
             */
            bool has_all_members() const noexcept {
                return m_need_members == 0;
            }

        }; // class RelationMeta

        template <typename TChar, typename TTraits>
        inline std::basic_ostream<TChar, TTraits>& operator<<(std::basic_ostream<TChar, TTraits>& out, const RelationMeta& rm) {
            out << "RelationMeta(relation_offset=" << rm.relation_offset() << " has_all_members=" << rm.has_all_members() << ")";
            return out;
        }

        /**
         * Function object to check if a relation is complete.
         */
        struct has_all_members {

            typedef RelationMeta& argument_type;
            typedef bool result_type;

            /**
             * @returns true if this relation is complete, false otherwise.
             */
            bool operator()(RelationMeta& relation_info) const {
                return relation_info.has_all_members();
            }

        }; // struct has_all_members

    } // namespace relations

} // namespace osrm_osmium

#endif // OSMIUM_RELATIONS_DETAIL_RELATION_META_HPP
