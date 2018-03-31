//
// Created by kiva on 2018/2/26.
//

#include <kivm/kivm.h>
#include <kivm/classfile/classFileParser.h>

using namespace kivm;

void print_constant_pool(ClassFile *classFile) {
    cp_info **pool = classFile->constant_pool;
    printf("\n");
    for (int i = 1; i < classFile->constant_pool_count; i++) {
        switch (pool[i]->tag) {
            case CONSTANT_Class: {
                auto *target = (CONSTANT_Class_info *) pool[i];
                printf("    #%4d = Class %15s #%d\n", i, "", target->name_index);
                break;
            }
            case CONSTANT_String: {
                auto *target = (CONSTANT_String_info *) pool[i];
                printf("    #%4d = String %14s #%d\n", i, "", target->string_index);
                break;
            }
            case CONSTANT_Fieldref: {
                auto *target = (CONSTANT_Fieldref_info *) pool[i];
                printf("    #%4d = Fieldref %12s #%d.#%d\n", i, "", target->class_index,
                       target->name_and_type_index);
                break;
            }
            case CONSTANT_Methodref: {
                auto *target = (CONSTANT_InterfaceMethodref_info *) pool[i];
                printf("    #%4d = Methodref %11s #%d.#%d\n", i, "", target->class_index,
                       target->name_and_type_index);
                break;
            }
            case CONSTANT_InterfaceMethodref: {
                auto *target = (CONSTANT_InterfaceMethodref_info *) pool[i];
                printf("    #%4d = InterfaceMethodref %2s #%d.#%d\n", i, "", target->class_index,
                       target->name_and_type_index);
                break;
            }
            case CONSTANT_Integer: {
                auto *target = (CONSTANT_Integer_info *) pool[i];
                printf("    #%4d = Integer %13s %di\n", i, "", target->get_constant());
                break;
            }
            case CONSTANT_Float: {
                auto *target = (CONSTANT_Float_info *) pool[i];
                float result = target->get_constant();
                if (FLOAT_IS_POSITIVE_INFINITY(result)) {
                    printf("    #%4d = Float %15s Inf\n", i, "");

                } else if (FLOAT_IS_NEGATIVE_INFINITY(result)) {
                    printf("    #%4d = Float %15s -Inf\n", i, "");

                } else if (FLOAT_IS_NAN(result)) {
                    printf("    #%4d = Float %15s NaN\n", i, "");

                } else {
                    printf("    #%4d = Float %15s %f\n", i, "", result);
                }
                break;
            }
            case CONSTANT_Long: {
                auto *target = (CONSTANT_Long_info *) pool[i];
                printf("    #%4d = Long %16s %ldl\n", i, "", target->get_constant());
                ++i;
                break;
            }
            case CONSTANT_Double: {
                auto *target = (CONSTANT_Double_info *) pool[i];
                double result = target->get_constant();
                if (DOUBLE_IS_POSITIVE_INFINITY(result))
                    printf("    #%4d = Double %14s Inf\n", i, "");
                else if (DOUBLE_IS_NEGATIVE_INFINITY(result))
                    printf("    #%4d = Double %14s -Inf\n", i, "");
                else if (DOUBLE_IS_NAN(result)) {
                    printf("    #%4d = Double %14s NaN\n", i, "");
                } else {
                    printf("    #%4d = Double %14s %lf\n", i, "", result);
                }
                ++i;
                break;
            }
            case CONSTANT_NameAndType: {
                auto *target = (CONSTANT_NameAndType_info *) pool[i];
                printf("    #%4d = NameAndType %9s #%d.#%d\n", i, "", target->name_index,
                       target->descriptor_index);
                break;
            }
            case CONSTANT_Utf8: {
                auto *target = (CONSTANT_Utf8_info *) pool[i];
                printf("    #%4d = Utf8 %16s ", i, "");
                printf("%s\n", strings::toStdString(target->get_constant()).c_str());
                break;
            }
            case CONSTANT_MethodHandle: {
                auto *target = (CONSTANT_MethodHandle_info *) pool[i];
                const char *ref_kind;
                switch (target->reference_kind) {
                    case REF_getField:
                        ref_kind = "[getField]";
                        break;
                    case REF_getStatic:
                        ref_kind = "[getStatic]";
                        break;
                    case REF_putField:
                        ref_kind = "[putField]";
                        break;
                    case REF_putStatic:
                        ref_kind = "[putStatic]";
                        break;
                    case REF_invokeVirtual:
                        ref_kind = "[invokeVirtual]";
                        break;
                    case REF_invokeStatic:
                        ref_kind = "[invokeStatic]";
                        break;
                    case REF_invokeSpecial:
                        ref_kind = "[invokeSpecial]";
                        break;
                    case REF_newInvokeSpecial:
                        ref_kind = "[newInvokeSpecial]";
                        break;
                    case REF_invokeInterface:
                        ref_kind = "[invokeInterface]";
                        break;
                    default:
                        printf("Unknown reference kind: %u\n", target->reference_kind);
                        return;
                }
                printf("    #%4d = MethodHandle %8s %s:#%d\n", i, "", ref_kind, target->reference_index);
                break;
            }
            case CONSTANT_MethodType: {
                auto *target = (CONSTANT_MethodType_info *) pool[i];
                printf("    #%4d = MethodType %10s #%d\n", i, "", target->descriptor_index);
                break;
            }
            case CONSTANT_InvokeDynamic: {
                auto *target = (CONSTANT_InvokeDynamic_info *) pool[i];
                printf("    #%4d = InvokeDynamic %7s #%d:#%d\n", i, "", target->bootstrap_method_attr_index,
                       target->name_and_type_index);
                break;
            }
            default: {
                printf("Unknown constant entry: %u", pool[i]->tag);
                return;
            }
        }
    }
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        printf("Usage: javap <class file>\n");
        return 1;
    }

    ClassFileParser parser(argv[1]);
    ClassFile *classFile = parser.getParsedClassFile();
    if (classFile != nullptr) {
        print_constant_pool(classFile);
        ClassFileParser::dealloc(classFile);
    }
    return 0;
}
