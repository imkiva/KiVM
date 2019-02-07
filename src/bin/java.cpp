#include <kivm/classpath/classLoader.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/javaThread.h>
#include <kivm/classpath/classPathManager.h>
#include <kivm/runtime/runtimeConfig.h>
#include <bin/clipp.h>
#include <iostream>

int main(int argc, char **argv) {
    using namespace kivm;
    using namespace clipp;

    std::string optClassPath;
    std::string optClassName;
    std::vector<std::string> optArgs;
    bool optShowHelp = false;

    auto cli = (
            option("-h", "-help").call([&]() { optShowHelp = true; }) % "show help",
            option("-v", "-version").call([argv]() {
                fprintf(stderr, "%s: %s\n", argv[0], strings::toStdString(KIVM_VERSION_STRING).c_str());
            }) % "show version",
            (option("-cp") & value("path").set(optClassPath)) % "class search path",
            value("class-name").set(optClassName),
            opt_values("args", optArgs)
    );

    if (!parse(argc, argv, cli) || optShowHelp) {
        auto fmt = doc_formatting{}.doc_column(28);
        auto filter = param_filter{}.prefix("-");
        std::cerr << "Usage:\n" << usage_lines(cli, argv[0]) << "\n\n"
                  << "Options:\n" << documentation(cli, fmt, filter) << "\n\n";
        return optShowHelp ? 0 : 1;
    }


    String mainClassName = strings::replaceAll(strings::fromStdString(optClassName),
        Global::DOT, Global::SLASH);
    D("java: main class name: %s\n", strings::toStdString(mainClassName).c_str());

    JavaVM *javaVM = nullptr;
    JNIEnv *env = nullptr;
    if (JNI_CreateJavaVM(&javaVM, (void **) &env, nullptr) != JNI_OK) {
        PANIC("JNI_CreateJavaVM() failed");
    }

    std::vector<String> arguments;
    arguments.reserve(optArgs.size());
    for (const auto &a : optArgs) {
        arguments.push_back(strings::fromStdString(a));
    }

    if (!optClassPath.empty()) {
        ClassPathManager::get()->addClassPaths(strings::fromStdString(optClassPath));
    }

    JavaMainThread javaMainThread(mainClassName, arguments);
    javaMainThread.start();

    javaVM->DestroyJavaVM();
    return 0;
}
