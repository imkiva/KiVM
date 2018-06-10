//
// Created by kiva on 2018/6/10.
//

#include <shared/osInfo.h>
#include <dlfcn.h>

#if defined(KIVM_PLATFORM_APPLE)
namespace kivm {
    static void *getJRSFramework() {
        static void *jrsFwk = nullptr;
        if (jrsFwk == nullptr) {
            jrsFwk = dlopen(
                "/System/Library/Frameworks/JavaVM.framework/Frameworks/JavaRuntimeSupport.framework/JavaRuntimeSupport",
                RTLD_LAZY | RTLD_LOCAL);
        }
        return jrsFwk;
    }

    String MacOSInformation::getOSName() {
        return L"Mac OS X";
    }

    String MacOSInformation::getOSVersion() {
        void *jrsFwk = getJRSFramework();
        if (jrsFwk != nullptr) {
            auto copyOSVersion = (char *(*)()) (dlsym(jrsFwk, "JRSCopyOSVersion"));

            if (copyOSVersion != nullptr) {
                char *osVersion = copyOSVersion();
                String ver = strings::fromStdString(osVersion);
                free(osVersion);
                return ver;
            }
        }
        return kivm::String();
    }
}
#endif
