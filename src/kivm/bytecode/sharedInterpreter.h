//
// Created by kiva on 2018/6/3.
//
#pragma once

#undef D
#define D(...)

#define GOTO_BY_OFFSET(branch) \
                    pc += branch

#define GOTO_BY_OFFSET_WITH_OCCUPIED(branch, occupied) \
                    GOTO_BY_OFFSET(branch); \
                    GOTO_BY_OFFSET(-((occupied) - 1))

#define GOTO_BY_OFFSET_HARDCODEDED(occupied) \
                    short branch = codeBlob[pc] << 8 | codeBlob[pc + 1]; \
                    GOTO_BY_OFFSET_WITH_OCCUPIED(branch, occupied)

#define GOTO_ABSOLUTE(newPc) \
                    pc = newPc

#define GOTO_ABSOLUTE_WITH_OCCUPIED(newPc, occupied) \
                    GOTO_ABSOLUTE(newPc); \
                    GOTO_BY_OFFSET(-((occupied) - 1))

#define __IF_GOTO_FACTORY(func, target, occupied, op) \
                    if (stack.func() op target) { \
                        GOTO_BY_OFFSET_HARDCODEDED(occupied); \
                    } else { \
                        pc += (occupied); \
                    }

#define __IF_CMP_GOTO_FACTORY(func, occupied, op) \
                    auto v2 = stack.func(); \
                    auto v1 = stack.func(); \
                    if (v1 op v2) { \
                        GOTO_BY_OFFSET_HARDCODEDED(occupied); \
                    } else { \
                        pc += (occupied); \
                    }

#define IF_GOTO(occupied, op) __IF_GOTO_FACTORY(popInt, 0, occupied, op)
#define IF_NULLCMP_GOTO(occupied, op) __IF_GOTO_FACTORY(popReference, nullptr, occupied, op)

#define IF_ICMP_GOTO(occupied, op) __IF_CMP_GOTO_FACTORY(popInt, occupied, op)
#define IF_ACMP_GOTO(occupied, op) __IF_CMP_GOTO_FACTORY(popReference, occupied, op)
