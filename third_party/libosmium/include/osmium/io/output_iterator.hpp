#ifndef OSMIUM_IO_OUTPUT_ITERATOR_HPP
#define OSMIUM_IO_OUTPUT_ITERATOR_HPP

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
#include <iterator>
#include <memory>
#include <utility>

#include <osmium/memory/buffer.hpp>
#include <osmium/osm/diff_object.hpp>
#include <osmium/util/compatibility.hpp>

namespace osrm_osmium {

    namespace memory {
        class Item;
    } // namespace memory

    namespace io {

        template <typename TDest>
        class OutputIterator : public std::iterator<std::output_iterator_tag, osrm_osmium::memory::Item> {

            TDest* m_destination;

        public:

            explicit OutputIterator(TDest& destination) :
                m_destination(&destination) {
            }

            /**
             * @deprecated
             * Use of buffer size argument on OutputIterator
             * constructor is deprecated. Call Writer::set_buffer_size()
             * instead if you want to change the default.
             */
            OSMIUM_DEPRECATED OutputIterator(TDest& destination, const size_t buffer_size) :
                m_destination(&destination) {
                destination.set_buffer_size(buffer_size);
            }

            OutputIterator(const OutputIterator&) = default;
            OutputIterator(OutputIterator&&) = default;

            OutputIterator& operator=(const OutputIterator&) = default;
            OutputIterator& operator=(OutputIterator&&) = default;

            ~OutputIterator() = default;

            /**
             * @deprecated
             * Calling OutputIterator<Writer>::flush() is usually not
             * needed any more. Call flush() on the Writer instead if needed.
             */
            OSMIUM_DEPRECATED void flush() {
                m_destination->flush();
            }

            OutputIterator& operator=(const osrm_osmium::memory::Item& item) {
                (*m_destination)(item);
                return *this;
            }

            OutputIterator& operator=(const osrm_osmium::DiffObject& diff) {
                return this->operator=(diff.curr());
            }

            OutputIterator& operator*() {
                return *this;
            }

            OutputIterator& operator++() {
                return *this;
            }

            OutputIterator& operator++(int) {
                return *this;
            }

        }; // class OutputIterator

        template <typename TDest>
        OutputIterator<TDest> make_output_iterator(TDest& destination) {
            return OutputIterator<TDest>{destination};
        }

        /**
         * @deprecated
         * Use of buffer size argument on make_output_iterator is deprecated.
         * Call Writer::set_buffer_size() instead if you want to change the
         * default.
         */
        template <typename TDest>
        OSMIUM_DEPRECATED OutputIterator<TDest> make_output_iterator(TDest& destination, const size_t buffer_size) {
            destination.set_buffer_size(buffer_size);
            return OutputIterator<TDest>{destination};
        }

    } // namespace io

} // namespace osrm_osmium

#endif // OSMIUM_IO_OUTPUT_ITERATOR_HPP
