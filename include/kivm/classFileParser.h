//
// Created by kiva on 2018/2/25.
//

#pragma once

#include <kivm/classFile.h>
#include <kivm/classFileStream.h>

namespace kivm {
    class ClassFileParser {
    public:
        static ClassFile *alloc();

        static void dealloc(ClassFile *class_file);

        static void read_attributes(attribute_info ***p, u2 count,
                                    ClassFileStream &stream, cp_info **constant_pool);

        static void dealloc_attributes(attribute_info ***p, u2 count);

    private:
        static attribute_info *parse_attribute(ClassFileStream &stream, cp_info **constant_pool);

    private:
        ClassFile *_classFile;
        ClassFileStream _classFileStream;
        FILE *_file;

        u1 *_content;

        ClassFile *parse();

        void parse_constant_pool(ClassFile *classFile);

        void parse_interfaces(ClassFile *classFile);

        void parse_fields(ClassFile *classFile);

        void parse_methods(ClassFile *classFile);

        void parse_attributes(ClassFile *classFile);

    public:
        explicit ClassFileParser(const char *filePath);

        ~ClassFileParser();

        ClassFile *classFile();
    };
}
