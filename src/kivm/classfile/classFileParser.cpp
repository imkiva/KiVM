//
// Created by kiva on 2018/2/25.
//

#include <cstdio>
#include <kivm/classfile/classFileParser.h>
#include <cassert>

namespace kivm {
    static u1 *read_all_rewind(FILE *file, size_t *psize) {
        fseek(file, 0L, SEEK_END);
        long size = ftell(file);
        auto *content = new u1[size + 1];
        fseek(file, 0L, SEEK_SET);
        fread(content, static_cast<size_t>(size), 1, file);
        *psize = static_cast<size_t>(size);
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
        AttributeParser::deallocAttributes(&class_file->attributes, class_file->attributes_count);
        for (int i = 1; i < class_file->constant_pool_count; ++i) {
            u1 tag = class_file->constant_pool[i]->tag;
            delete class_file->constant_pool[i];
            if (tag == CONSTANT_Long || tag == CONSTANT_Double) {
                ++i;
            }
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
        _classFileStream.setSource(filePath);
    }

    ClassFileParser::~ClassFileParser() {
        if (_file != nullptr) {
            fclose(_file);
        }
        delete[] _content;
    }

    ClassFile *ClassFileParser::getParsedClassFile() {
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

        classFile->magic = _classFileStream.get4();
        if (classFile->magic != 0xCAFEBABE) {
            ClassFileParser::dealloc(classFile);
            return nullptr;
        }

        classFile->major_version = _classFileStream.get2();
        classFile->minor_version = _classFileStream.get2();
        parseConstantPool(classFile);

        classFile->access_flags = _classFileStream.get2();
        classFile->this_class = _classFileStream.get2();
        classFile->super_class = _classFileStream.get2();

        parseInterfaces(classFile);
        parseFields(classFile);
        parseMethods(classFile);
        parseAttributes(classFile);
        return classFile;
    }

    template<typename T>
    static void readPoolEntry(cp_info **pool, int index, ClassFileStream &stream) {
        pool[index] = new T;
        stream >> *(T *) pool[index];
    }

    void ClassFileParser::parseConstantPool(ClassFile *classFile) {
        u2 count = classFile->constant_pool_count = _classFileStream.get2();

        classFile->constant_pool = new cp_info *[count];
        cp_info **pool = classFile->constant_pool;

        // The constant_pool table is indexed
        // from 1 to count - 1
        for (int i = 1; i < count; ++i) {
            u1 tag = _classFileStream.peek1();
            switch (tag) {
                case CONSTANT_Utf8:
                    readPoolEntry<CONSTANT_Utf8_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Integer:
                    readPoolEntry<CONSTANT_Integer_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Float:
                    readPoolEntry<CONSTANT_Float_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Long:
                    readPoolEntry<CONSTANT_Long_info>(pool, i, _classFileStream);
                    ++i;
                    break;
                case CONSTANT_Double:
                    readPoolEntry<CONSTANT_Double_info>(pool, i, _classFileStream);
                    ++i;
                    break;
                case CONSTANT_Class:
                    readPoolEntry<CONSTANT_Class_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_String:
                    readPoolEntry<CONSTANT_String_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Fieldref:
                    readPoolEntry<CONSTANT_Fieldref_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_Methodref:
                    readPoolEntry<CONSTANT_Methodref_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_InterfaceMethodref:
                    readPoolEntry<CONSTANT_InterfaceMethodref_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_NameAndType:
                    readPoolEntry<CONSTANT_NameAndType_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_MethodHandle:
                    readPoolEntry<CONSTANT_MethodHandle_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_MethodType:
                    readPoolEntry<CONSTANT_MethodType_info>(pool, i, _classFileStream);
                    break;
                case CONSTANT_InvokeDynamic:
                    readPoolEntry<CONSTANT_InvokeDynamic_info>(pool, i, _classFileStream);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }

    void ClassFileParser::parseInterfaces(ClassFile *classFile) {
        u2 count = classFile->interfaces_count = _classFileStream.get2();
        classFile->interfaces = new u2[count];
        for (int i = 0; i < count; i++) {
            classFile->interfaces[i] = _classFileStream.get2();
        }
    }

    void ClassFileParser::parseFields(ClassFile *classFile) {
        u2 count = classFile->fields_count = _classFileStream.get2();
        classFile->fields = new field_info[count];
        for (int i = 0; i < count; ++i) {
            classFile->fields[i].init(_classFileStream, classFile->constant_pool);
        }
    }

    void ClassFileParser::parseMethods(ClassFile *classFile) {
        u2 count = classFile->methods_count = _classFileStream.get2();
        classFile->methods = new method_info[count];
        for (int i = 0; i < count; ++i) {
            classFile->methods[i].init(_classFileStream, classFile->constant_pool);
        }
    }

    void ClassFileParser::parseAttributes(ClassFile *classFile) {
        classFile->attributes_count = _classFileStream.get2();
        AttributeParser::readAttributes(&classFile->attributes, classFile->attributes_count,
                                        _classFileStream, classFile->constant_pool);
    }
}