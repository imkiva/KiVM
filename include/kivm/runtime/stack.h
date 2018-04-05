//
// Created by kiva on 2018/3/21.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/oop/oopfwd.h>
#include <kivm/runtime/slot.h>

namespace kivm {
    class SlotArray {
    protected:
        Slot *_elements;
        int _size;

        explicit SlotArray(int size);

    public:
        inline void setInt(int position, jint i) {
            _elements[position].i32 = i;
        }

        inline jint getInt(int position) {
            return _elements[position].i32;
        }

        inline void setLong(int position, jlong j) {
            setInt(position, static_cast<jint>(j));
            setInt(position + 1, static_cast<jint>(j >> 32));
        }

        inline jlong getLong(int position) {
            jint low = getInt(position);
            jint high = getInt(position + 1);
            return static_cast<jlong>(high) << 32 | low;
        }

        inline void setFloat(int position, jfloat f) {
            union _Cvt {
                jfloat f;
                jint i32;
            };
            _Cvt X{};
            X.f = f;
            setInt(position, X.i32);
        }

        inline jfloat getFloat(int position) {
            union _Cvt {
                jfloat f;
                jint i32;
            };
            _Cvt X{};
            X.i32 = getInt(position);
            return X.f;
        }

        inline void setDouble(int position, jdouble d) {
            union _Cvt {
                jdouble d;
                jlong j;
            };
            _Cvt X{};
            X.d = d;
            setLong(position, X.j);
        }

        inline jdouble getDouble(int position) {
            union _Cvt {
                jdouble d;
                jlong j;
            };
            _Cvt X{};
            X.j = getLong(position);
            return X.d;
        }

        inline void setReference(int position, jobject l) {
            _elements[position].ref = l;
        }

        inline jobject getReference(int position) {
            return _elements[position].ref;
        }

    public:
        virtual ~SlotArray();
    };

    class Stack : public SlotArray {
    private:
        int _sp;

    public:
        explicit Stack(int size);

        ~Stack() override = default;

        inline void pushInt(jint v) { SlotArray::setInt(_sp++, v); }

        inline void pushFloat(jfloat v) { SlotArray::setFloat(_sp++, v); }

        inline void pushReference(jobject v) { SlotArray::setReference(_sp++, v); }

        inline void pushDouble(jdouble v) {
            SlotArray::setDouble(_sp, v);
            _sp += 2;
        }

        inline void pushLong(jlong v) {
            SlotArray::setLong(_sp, v);
            _sp += 2;
        }

        inline jint popInt() { return SlotArray::getInt(--_sp); }

        inline jfloat popFloat() { return SlotArray::getFloat(--_sp); }

        inline jobject popReference() { return SlotArray::getReference(--_sp); }

        inline jdouble popDouble() {
            _sp -= 2;
            return SlotArray::getDouble(_sp);
        }

        inline jlong popLong() {
            _sp -= 2;
            return SlotArray::getLong(_sp);
        }
    };

    class Locals : public SlotArray {
    public:
        explicit Locals(int size);

        ~Locals() override = default;
    };
}

