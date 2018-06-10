//
// Created by kiva on 2018/6/10.
//

#include <kivm/kivm.h>
#include <kivm/native/classNames.h>
#include <shared/osInfo.h>

using namespace kivm;

JAVA_NATIVE jint Java_java_lang_Runtime_availableProcessors(JNIEnv *env, jclass unused) {
    return OSInformation::getCpuNumbers();
}
