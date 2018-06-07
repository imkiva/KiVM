/*
 * Covariant Script KiVM Extension
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2018 Kiva
 * Email: libkernelpanic@gmail.com
 * Github: https://github.com/imkiva
 */

#pragma once
#include <covscript/cni.hpp>
#include <covscript/extension.hpp>

#define __CS_EXTENSION_NS_NAME(NAME) cs_ext_##NAME
#define __CNI_NAME_MIXER(PREFIX, NAME) static cni_register PREFIX##NAME
#define __CNI_REGISTER(NAME, ARGS) __CNI_NAME_MIXER(_cni_register_, NAME)(#NAME, NAME, ARGS);
#define __CNI_NORMAL(NAME) __CNI_REGISTER(NAME, false)
#define __CNI_CONST(NAME) __CNI_REGISTER(NAME, true)

#define __CNI_EXPAND(MARCO, R, NAME, ...) \
    extern R NAME(__VA_ARGS__); \
    MARCO(NAME); \
    R NAME(__VA_ARGS__)

#define CS_EXTENSION(NAME) \
    namespace __CS_EXTENSION_NS_NAME(NAME) { \
    static cs::extension __cs_extension_ext; \
    static cs::extension_t __cs_extension_shared = cs::make_shared_extension(__cs_extension_ext); \
    class cni_register final { \
    public: \
        template<typename _fT> \
        cni_register(const char *name, _fT &&func, bool is_const) { \
            using namespace cs; \
            __cs_extension_ext.add_var(name, cs::var::make_protect<cs::callable>(cs::cni(func), is_const)); \
        } \
    }; \
    } \
    cs::extension *cs_extension() { \
        return &__CS_EXTENSION_NS_NAME(NAME)::__cs_extension_ext; \
    } \
    namespace __CS_EXTENSION_NS_NAME(NAME) {

#define CS_EXTENSION_END() }

#define CS_DECLARE_AS_OBJECT(NAME, TYPE) \
    } \
    namespace cs_impl { \
        template<> \
        cs::extension_t &get_ext<__CS_EXTENSION_NS_NAME(NAME)::TYPE>() { \
            return __CS_EXTENSION_NS_NAME(NAME)::__cs_extension_shared; \
        } \
    } \
    namespace __CS_EXTENSION_NS_NAME(NAME) {

#define CS_OBJECT(NAME) \
    struct NAME

#define CNI_NORMAL(R, NAME, ...) __CNI_EXPAND(__CNI_NORMAL, R, NAME, ##__VA_ARGS__)
#define CNI_CONST(R, NAME, ...) __CNI_EXPAND(__CNI_CONST, R, NAME, ##__VA_ARGS__)

