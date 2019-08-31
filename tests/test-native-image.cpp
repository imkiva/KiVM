//
// Created by kiva on 2019/8/31.
//

#include <kivm/kivm.h>

#define LINEBREAK 12
#define MAIN_CLASS_BYTES "__MAIN_CLASS_BYTES"
#define MAIN_CLASS_SIZE "__MAIN_CLASS_SIZE"

void printHeaders() {
    printf("#include <kivm/kivm.h>\n");
    printf("#include <kivm/runtime/javaThread.h>\n");
    printf("#include <vector>\n");
}

void printFiles(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    int lineBreakCounter = 0;
    printf("unsigned char " MAIN_CLASS_BYTES "[] = {\n");
    printf("  ");
    while (!feof(file)) {
        int c = fgetc(file);
        if (c != EOF) {
            printf("0x%02x, ", c);
        }
        ++lineBreakCounter;
        if (lineBreakCounter % LINEBREAK == 0) {
            printf("\n");
            printf("  ");
        }
    }
    printf("\n};\n");

    printf("unsigned long " MAIN_CLASS_SIZE " = %ld;\n", fileSize);
}

void printEntrance() {
    printf("\n\n");
    printf("int main(int argc, const char **argv) {\n");
    printf("    using namespace kivm;\n");
    printf("    JavaVM *javaVM = nullptr;\n");
    printf("    JNIEnv *env = nullptr;\n");
    printf("    if (JNI_CreateJavaVM(&javaVM, (void **) &env, nullptr) != JNI_OK) {\n");
    printf("        PANIC(\"JNI_CreateJavaVM() failed\");\n");
    printf("    }\n");
    printf("    std::vector<String> arguments;\n");
    printf("    ++argv;\n");
    printf("    for (const char **p = argv; *p; p++) {\n");
    printf("        arguments.push_back(strings::fromStdString(std::string(*p)));\n");
    printf("    }\n");
    printf("\n");
    printf("    JavaMainThread javaMainThread(" MAIN_CLASS_BYTES ", " MAIN_CLASS_SIZE ", arguments);\n");
    printf("    javaMainThread.start();\n");
    printf("\n");
    printf("    javaVM->DestroyJavaVM();\n");
    printf("    return 0;\n");
    printf("}\n");
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <java-class-file>\n", argv[0]);
        return 1;
    }

    printHeaders();
    printFiles(argv[1]);
    printEntrance();
}
