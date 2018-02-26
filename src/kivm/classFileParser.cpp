//
// Created by kiva on 2018/2/25.
//

#include <cstdio>
#include <kivm/classFileParser.h>
#include <cassert>

namespace kivm {
    static u1 *read_all_rewind(FILE *file, size_t *psize) {
        fseek(file, 0L, SEEK_END);
        long size = ftell(file);
        auto *content = new u1[size + 1];
        if (content != nullptr) {
            fseek(file, 0L, SEEK_SET);
            fread(content, static_cast<size_t>(size), 1, file);
            *psize = static_cast<size_t>(size);
        }
        return content;
    }

    ClassFile *ClassFileParser::alloc() {
        auto *classFile = new ClassFile();
        classFile->constant_pool = nullptr;
        classFile->interfaces = nullptr;
        classFile->fields = nullptr;
        classFile->methods = nullptr;
        classFile->attributes = nullptr;
        return classFile;
    }

    void ClassFileParser::dealloc(ClassFile *class_file) {
        AttributeParser::dealloc_attributes(&class_file->attributes, class_file->attributes_count);
        for (int i = 1; i < class_file->constant_pool_count; ++i) {
            delete class_file->constant_pool[i];
        }
        delete[] class_file->methods;
        delete[] class_file->fields;
        delete[] class_file->interfaces;
        delete[] class_file->attributes;
        delete[] class_file->constant_pool;
        delete class_file;
    }

    ClassFileParser::ClassFileParser(const char *filePath) {
        _classFile = nullptr;
        _content = nullptr;
        _file = fopen(filePath, "rb");
        _classFileStream.set_source(filePath);
    }

    ClassFileParser::~ClassFileParser() {
        if (_file != nullptr) {
            fclose(_file);
        }
        delete[] _content;
    }

    ClassFile *ClassFileParser::classFile() {
        if (_classFile == nullptr) {
            if (_file != nullptr) {
                _classFile = parse();
            }
        }

        return _classFile;
    }

    ClassFile *ClassFileParser::parse() {
        size_t size = 0;
        _content = read_all_rewind(_file, &size);
        if (_content == nullptr) {
            return nullptr;
        }

        ClassFile *classFile = ClassFileParser::alloc();
        if (classFile == nullptr) {
            return nullptr;
        }

        _classFileStream.init(_content, size);

        classFile->magic = _classFileStream.get_u4();
        if (classFile->magic != 0xCAFEBABE) {
            ClassFileParser::dealloc(classFile);
            return nullptr;
        }

        classFile->major_version = _classFileStream.get_u2();
        classFile->minor_version = _classFileStream.get_u2();
        parse_constant_pool(classFile);

        classFile->access_flags = _classFileStream.get_u2();
        classFile->this_class = _classFileStream.get_u2();
        classFile->super_class = _classFileStream.get_u2();

        parse_interfaces(classFile);
        parse_fields(classFile);
        parse_methods(classFile);
        parse_attributes(classFile);
        return classFile;
    }

    template<typename T>
    static void read_pool_entry(cp_info **pool, int index, ClassFileStream &stream) {
        pool[index] = new T;
        stream >> *(T *) pool[index];
    }

    void ClassFileParser::parse_constant_pool(ClassFile *classFile) {
        u2 count = classFile->constant_pool_count = _classFileStream.get_u2();

        classFile->constant_pool = new cp_info *[count];
        cp_info **pool = classFile->constant_pool;

        // The constant_pool table is indexed
        // from 1 to count - 1
        for (int i = 1; i < count; ++i) {
            switch (_classFileStream.peek_u1()) {
                case CONSTANT_Utf8:
                    read_pool_entry<CONSTANT_Utf8_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Integer:
                    read_pool_entry<CONSTANT_Integer_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Float:
                    read_pool_entry<CONSTANT_Float_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Long:
                    read_pool_entry<CONSTANT_Long_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Double:
                    read_pool_entry<CONSTANT_Double_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Class:
                    read_pool_entry<CONSTANT_Class_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_String:
                    read_pool_entry<CONSTANT_String_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Fieldref:
                    read_pool_entry<CONSTANT_Fieldref_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Methodref:
                    read_pool_entry<CONSTANT_Methodref_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_InterfaceMethodref:
                    read_pool_entry<CONSTANT_InterfaceMethodref_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_NameAndType:
                    read_pool_entry<CONSTANT_NameAndType_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_MethodHandle:
                    read_pool_entry<CONSTANT_MethodHandle_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_MethodType:
                    read_pool_entry<CONSTANT_MethodType_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_InvokeDynamic:
                    read_pool_entry<CONSTANT_InvokeDynamic_info>(pool, i, _classFileStream);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }

    void ClassFileParser::parse_interfaces(ClassFile *classFile) {
        u2 count = classFile->interfaces_count = _classFileStream.get_u2();
        classFile->interfaces = new u2[count];
        for (int i = 0; i < count; i++) {
            classFile->interfaces[i] = _classFileStream.get_u2();
        }
    }

    void ClassFileParser::parse_fields(ClassFile *classFile) {
        u2 count = classFile->fields_count = _classFileStream.get_u2();
        classFile->fields = new field_info[count];
        for (int i = 0; i < count; ++i) {
            classFile->fields[i].init(_classFileStream, classFile->constant_pool);
        }
    }

    void ClassFileParser::parse_methods(ClassFile *classFile) {
        u2 count = classFile->methods_count = _classFileStream.get_u2();
        classFile->methods = new method_info[count];
        for (int i = 0; i < count; ++i) {
            classFile->methods[i].init(_classFileStream, classFile->constant_pool);
        }
    }

    void ClassFileParser::parse_attributes(ClassFile *classFile) {
        classFile->attributes_count = _classFileStream.get_u2();
        AttributeParser::read_attributes(&classFile->attributes, classFile->attributes_count,
                        _classFileStream, classFile->constant_pool);
    }
}