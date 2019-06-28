//
// Created by kiva on 2019-06-22.
//
#pragma once

#include <exception>
#include <stdexcept>
#include <algorithm>

namespace kivm {
    namespace instr {
        enum EdgeType {
            /**
            * Regular transition.
            *
            * For conditional jumps, this edge type requires either the string <code>true</code> or <code>false</code>
            * as meta-data.
            */
                REGULAR,

            /**
             * Transition because of caught exception.
             *
             * This edge type requires the handled exception type as meta-data.
             */
                CAUGHT_EXCEPTION,

            /**
             * case of a LOOKUPSWITCH instruction.
             *
             * This edge type requires the case label as meta-data.
             */
                LOOKUP_SWITCH,

            /**
             * case of a TABLESWITCH instruction.
             *
             * This edge type requires the case label as meta-data.
             */
                TABLE_SWITCH
        };

        using Object = void *;

        class IBlock;

        class Edge {
        public:
            IBlock *src;
            IBlock *dest;
            EdgeType type;
            Object metaData;

            Edge(IBlock *src, IBlock *dest, EdgeType type, Object metaData)
                : src(src), dest(dest), type(type), metaData(metaData) {
                if (src == nullptr) {
                    throw std::invalid_argument("src cannot be null");
                }

                if (dest == nullptr) {
                    throw std::invalid_argument("dest cannot be null");
                }
            }


            Edge *withSource(IBlock *source) {
                this->src = source;
                return this;
            }

            Edge *withDestination(IBlock *destination) {
                this->dest = destination;
                return this;
            }

            bool isSuccessor(IBlock *block) {
                return src == block;
            }

            bool isPredecessor(IBlock *block) {
                return dest = block;
            }

            bool hasType(EdgeType type) {
                return this->type == type;
            }

            bool equals(Edge *other) {
                // TODO
            }
        };
    }
}
