//
// Created by kiva on 2018/3/21.
//
#pragma once

#include <kivm/runtime/slot.h>

namespace kivm {
    class SlotArray {
    protected:
        Slot *_elements;
        int _size;

        explicit SlotArray(int size);

    public:
        inline void set_int(int position, jint i) {
            _elements[position].i32 = i;
        }

        inline jint get_int(int position) {
            return _elements[position].i32;
        }

        inline void set_long(int position, jlong j) {
            set_int(position, static_cast<jint>(j));
            set_int(position + 1, static_cast<jint>(j >> 32));
        }

        inline jlong get_long(int position) {
            jint low = get_int(position);
            jint high = get_int(position + 1);
            return static_cast<jlong>(high) << 32 | low;
        }

        inline void set_float(int position, jfloat f) {
            union _Cvt {
                jfloat f;
                jint i32;
            };
            _Cvt X{};
            X.f = f;
            set_int(position, X.i32);
        }

        inline jfloat get_float(int position) {
            union _Cvt {
                jfloat f;
                jint i32;
            };
            _Cvt X{};
            X.i32 = get_int(position);
            return X.f;
        }

        inline void set_double(int position, jdouble d) {
            union _Cvt {
                jdouble d;
                jint i1;
                jint i2;
            };
            _Cvt X{};
            X.d = d;
            set_int(position, X.i1);
            set_int(position + 1, X.i2);
        }

        inline jdouble get_double(int position) {
            union _Cvt {
                jdouble d;
                jint i1;
                jint i2;
            };
            _Cvt X{};
            X.i1 = get_int(position);
            X.i2 = get_int(position + 1);
            return X.d;
        }

        inline void set_ref(int position, jobject l) {
            _elements[position].ref = l;
        }

        inline jobject get_ref(int position) {
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

        inline void push_int(jint v) { SlotArray::set_int(_sp++, v); }

        inline void push_float(jfloat v) { SlotArray::set_float(_sp++, v); }

        inline void push_ref(jobject v) { SlotArray::set_ref(_sp++, v); }

        inline void push_double(jdouble v) {
            SlotArray::set_double(_sp, v);
            _sp += 2;
        }

        inline jint pop_int() { return SlotArray::get_int(--_sp); }

        inline jfloat pop_float() { return SlotArray::get_float(--_sp); }

        inline jobject pop_ref() { return SlotArray::get_ref(--_sp); }

        inline jdouble pop_double() {
            _sp -= 2;
            return SlotArray::get_double(_sp);
        }
    };

    class Locals : public SlotArray {
    public:
        explicit Locals(int size);

        ~Locals() override = default;
    };
}

