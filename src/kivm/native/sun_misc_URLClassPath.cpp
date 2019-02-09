//
// Created by kiva on 2019-02-09.
//

#include <kivm/kivm.h>

using namespace kivm;

JAVA_NATIVE jarray Java_sun_misc_URLClassPath_getLookupCacheURLs(JNIEnv *env, jclass unused, jobject cl) {
    D("native: sun/misc/URLClassPath.getLookupCacheURLs()");
    return nullptr;
}
