//
// Created by kiva on 2018/4/16.
//
#pragma once

#include <kivm/kivm.h>

JNI_ENTRY(void, FillInterface());

JNI_ENTRY(jint, GetVersion(JNIEnv *env));

JNI_ENTRY(jclass, DefineClass(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize len));

JNI_ENTRY(jclass, FindClass(JNIEnv *env, const char *name));

JNI_ENTRY(jmethodID, FromReflectedMethod(JNIEnv *env, jobject method));

JNI_ENTRY(jfieldID, FromReflectedField(JNIEnv *env, jobject field));

JNI_ENTRY(jobject, ToReflectedMethod(JNIEnv *env, jclass cls, jmethodID methodID, jboolean isStatic));

JNI_ENTRY(jclass, GetSuperclass(JNIEnv *env, jclass sub));

JNI_ENTRY(jboolean, IsAssignableFrom(JNIEnv *env, jclass sub, jclass sup));

JNI_ENTRY(jobject, ToReflectedField(JNIEnv *env, jclass cls, jfieldID fieldID, jboolean isStatic));

JNI_ENTRY(jint, Throw(JNIEnv *env, jthrowable obj));

JNI_ENTRY(jint, ThrowNew(JNIEnv *env, jclass clazz, const char *msg));

JNI_ENTRY(jthrowable, ExceptionOccurred(JNIEnv *env));

JNI_ENTRY(void, ExceptionDescribe(JNIEnv *env));

JNI_ENTRY(void, ExceptionClear(JNIEnv *env));

JNI_ENTRY(void, FatalError(JNIEnv *env, const char *msg));

JNI_ENTRY(jint, PushLocalFrame(JNIEnv *env, jint capacity));

JNI_ENTRY(jobject, PopLocalFrame(JNIEnv *env, jobject result));

JNI_ENTRY(jobject, NewGlobalRef(JNIEnv *env, jobject lobj));

JNI_ENTRY(void, DeleteGlobalRef(JNIEnv *env, jobject gref));

JNI_ENTRY(void, DeleteLocalRef(JNIEnv *env, jobject obj));

JNI_ENTRY(jboolean, IsSameObject(JNIEnv *env, jobject obj1, jobject obj2));

JNI_ENTRY(jobject, NewLocalRef(JNIEnv *env, jobject ref));

JNI_ENTRY(jint, EnsureLocalCapacity(JNIEnv *env, jint capacity));

JNI_ENTRY(jobject, AllocObject(JNIEnv *env, jclass clazz));

JNI_ENTRY(jobject, NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jobject, NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jobject, NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jclass, GetObjectClass(JNIEnv *env, jobject obj));

JNI_ENTRY(jboolean, IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz));

JNI_ENTRY(jmethodID, GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig));

JNI_ENTRY(jobject, CallObjectMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jobject, CallObjectMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jobject, CallObjectMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jboolean, CallBooleanMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jboolean, CallBooleanMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jboolean, CallBooleanMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jbyte, CallByteMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jbyte, CallByteMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jbyte, CallByteMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jchar, CallCharMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jchar, CallCharMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jchar, CallCharMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jshort, CallShortMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jshort, CallShortMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jshort, CallShortMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jint, CallIntMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jint, CallIntMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jint, CallIntMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jlong, CallLongMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jlong, CallLongMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jlong, CallLongMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jfloat, CallFloatMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jfloat, CallFloatMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jfloat, CallFloatMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jdouble, CallDoubleMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(jdouble, CallDoubleMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(jdouble, CallDoubleMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(void, CallVoidMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...));

JNI_ENTRY(void, CallVoidMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args));

JNI_ENTRY(void, CallVoidMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jobject, CallNonvirtualObjectMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jobject, CallNonvirtualObjectMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jobject, CallNonvirtualObjectMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jboolean, CallNonvirtualBooleanMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jboolean, CallNonvirtualBooleanMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jboolean, CallNonvirtualBooleanMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jbyte, CallNonvirtualByteMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jbyte, CallNonvirtualByteMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jbyte, CallNonvirtualByteMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jchar, CallNonvirtualCharMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jchar, CallNonvirtualCharMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jchar, CallNonvirtualCharMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jshort, CallNonvirtualShortMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jshort, CallNonvirtualShortMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jshort, CallNonvirtualShortMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jint, CallNonvirtualIntMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jint, CallNonvirtualIntMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jint, CallNonvirtualIntMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jlong, CallNonvirtualLongMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jlong, CallNonvirtualLongMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jlong, CallNonvirtualLongMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jfloat, CallNonvirtualFloatMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jfloat, CallNonvirtualFloatMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jfloat, CallNonvirtualFloatMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jdouble, CallNonvirtualDoubleMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jdouble, CallNonvirtualDoubleMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jdouble, CallNonvirtualDoubleMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(void, CallNonvirtualVoidMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(void, CallNonvirtualVoidMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(void, CallNonvirtualVoidMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jfieldID, GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig));

JNI_ENTRY(jobject, GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jboolean, GetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jbyte, GetByteField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jchar, GetCharField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jshort, GetShortField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jint, GetIntField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jlong, GetLongField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jfloat, GetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(jdouble, GetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID));

JNI_ENTRY(void, SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID, jobject val));

JNI_ENTRY(void, SetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean val));

JNI_ENTRY(void, SetByteField(JNIEnv *env, jobject obj, jfieldID fieldID, jbyte val));

JNI_ENTRY(void, SetCharField(JNIEnv *env, jobject obj, jfieldID fieldID, jchar val));

JNI_ENTRY(void, SetShortField(JNIEnv *env, jobject obj, jfieldID fieldID, jshort val));

JNI_ENTRY(void, SetIntField(JNIEnv *env, jobject obj, jfieldID fieldID, jint val));

JNI_ENTRY(void, SetLongField(JNIEnv *env, jobject obj, jfieldID fieldID, jlong val));

JNI_ENTRY(void, SetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID, jfloat val));

JNI_ENTRY(void, SetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID, jdouble val));

JNI_ENTRY(jmethodID, GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig));

JNI_ENTRY(jobject, CallStaticObjectMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jobject, CallStaticObjectMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jobject, CallStaticObjectMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jboolean, CallStaticBooleanMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jboolean, CallStaticBooleanMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jboolean, CallStaticBooleanMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jbyte, CallStaticByteMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jbyte, CallStaticByteMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jbyte, CallStaticByteMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jchar, CallStaticCharMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jchar, CallStaticCharMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jchar, CallStaticCharMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jshort, CallStaticShortMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jshort, CallStaticShortMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jshort, CallStaticShortMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jint, CallStaticIntMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jint, CallStaticIntMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jint, CallStaticIntMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jlong, CallStaticLongMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jlong, CallStaticLongMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jlong, CallStaticLongMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jfloat, CallStaticFloatMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jfloat, CallStaticFloatMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jfloat, CallStaticFloatMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jdouble, CallStaticDoubleMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...));

JNI_ENTRY(jdouble, CallStaticDoubleMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args));

JNI_ENTRY(jdouble, CallStaticDoubleMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args));

JNI_ENTRY(void, CallStaticVoidMethod(JNIEnv *env, jclass cls, jmethodID methodID, ...));

JNI_ENTRY(void, CallStaticVoidMethodV(JNIEnv *env, jclass cls, jmethodID methodID, va_list args));

JNI_ENTRY(void, CallStaticVoidMethodA(JNIEnv *env, jclass cls, jmethodID methodID, const jvalue *args));

JNI_ENTRY(jfieldID, GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig));

JNI_ENTRY(jobject, GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jboolean, GetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jbyte, GetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jchar, GetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jshort, GetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jint, GetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jlong, GetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jfloat, GetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(jdouble, GetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID));

JNI_ENTRY(void, SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value));

JNI_ENTRY(void, SetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID, jboolean value));

JNI_ENTRY(void, SetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID, jbyte value));

JNI_ENTRY(void, SetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID, jchar value));

JNI_ENTRY(void, SetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID, jshort value));

JNI_ENTRY(void, SetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID, jint value));

JNI_ENTRY(void, SetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID, jlong value));

JNI_ENTRY(void, SetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID, jfloat value));

JNI_ENTRY(void, SetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID, jdouble value));

JNI_ENTRY(jstring, NewString(JNIEnv *env, const jchar *unicode, jsize len));

JNI_ENTRY(jsize, GetStringLength(JNIEnv *env, jstring str));

JNI_ENTRY(const jchar *,GetStringChars(JNIEnv *env, jstring str, jboolean *isCopy));

JNI_ENTRY(void, ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars));

JNI_ENTRY(jstring, NewStringUTF(JNIEnv *env, const char *utf));

JNI_ENTRY(jsize, GetStringUTFLength(JNIEnv *env, jstring str));

JNI_ENTRY(const char *,GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy));

JNI_ENTRY(void, ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *chars));

JNI_ENTRY(jsize, GetArrayLength(JNIEnv *env, jarray array));

JNI_ENTRY(jobjectArray, NewObjectArray(JNIEnv *env, jsize len, jclass clazz, jobject init));

JNI_ENTRY(jobject, GetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index));

JNI_ENTRY(void, SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index, jobject val));

JNI_ENTRY(jbooleanArray, NewBooleanArray(JNIEnv *env, jsize len));

JNI_ENTRY(jbyteArray, NewByteArray(JNIEnv *env, jsize len));

JNI_ENTRY(jcharArray, NewCharArray(JNIEnv *env, jsize len));

JNI_ENTRY(jshortArray, NewShortArray(JNIEnv *env, jsize len));

JNI_ENTRY(jintArray, NewIntArray(JNIEnv *env, jsize len));

JNI_ENTRY(jlongArray, NewLongArray(JNIEnv *env, jsize len));

JNI_ENTRY(jfloatArray, NewFloatArray(JNIEnv *env, jsize len));

JNI_ENTRY(jdoubleArray, NewDoubleArray(JNIEnv *env, jsize len));

JNI_ENTRY(jboolean *,GetBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *isCopy));

JNI_ENTRY(jbyte *,GetByteArrayElements(JNIEnv *env, jbyteArray array, jboolean *isCopy));

JNI_ENTRY(jchar *,GetCharArrayElements(JNIEnv *env, jcharArray array, jboolean *isCopy));

JNI_ENTRY(jshort *,GetShortArrayElements(JNIEnv *env, jshortArray array, jboolean *isCopy));

JNI_ENTRY(jint *,GetIntArrayElements(JNIEnv *env, jintArray array, jboolean *isCopy));

JNI_ENTRY(jlong *,GetLongArrayElements(JNIEnv *env, jlongArray array, jboolean *isCopy));

JNI_ENTRY(jfloat *,GetFloatArrayElements(JNIEnv *env, jfloatArray array, jboolean *isCopy));

JNI_ENTRY(jdouble *,GetDoubleArrayElements(JNIEnv *env, jdoubleArray array, jboolean *isCopy));

JNI_ENTRY(void, ReleaseBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *elems, jint mode));

JNI_ENTRY(void, ReleaseByteArrayElements(JNIEnv *env, jbyteArray array, jbyte *elems, jint mode));

JNI_ENTRY(void, ReleaseCharArrayElements(JNIEnv *env, jcharArray array, jchar *elems, jint mode));

JNI_ENTRY(void, ReleaseShortArrayElements(JNIEnv *env, jshortArray array, jshort *elems, jint mode));

JNI_ENTRY(void, ReleaseIntArrayElements(JNIEnv *env, jintArray array, jint *elems, jint mode));

JNI_ENTRY(void, ReleaseLongArrayElements(JNIEnv *env, jlongArray array, jlong *elems, jint mode));

JNI_ENTRY(void, ReleaseFloatArrayElements(JNIEnv *env, jfloatArray array, jfloat *elems, jint mode));

JNI_ENTRY(void, ReleaseDoubleArrayElements(JNIEnv *env, jdoubleArray array, jdouble *elems, jint mode));

JNI_ENTRY(void, GetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, jboolean *buf));

JNI_ENTRY(void, GetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, jbyte *buf));

JNI_ENTRY(void, GetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, jchar *buf));

JNI_ENTRY(void, GetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, jshort *buf));

JNI_ENTRY(void, GetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, jint *buf));

JNI_ENTRY(void, GetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, jlong *buf));

JNI_ENTRY(void, GetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, jfloat *buf));

JNI_ENTRY(void, GetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, jdouble *buf));

JNI_ENTRY(void, SetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, const jboolean *buf));

JNI_ENTRY(void, SetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, const jbyte *buf));

JNI_ENTRY(void, SetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, const jchar *buf));

JNI_ENTRY(void, SetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, const jshort *buf));

JNI_ENTRY(void, SetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, const jint *buf));

JNI_ENTRY(void, SetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, const jlong *buf));

JNI_ENTRY(void, SetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, const jfloat *buf));

JNI_ENTRY(void, SetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, const jdouble *buf));

JNI_ENTRY(jint, RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods));

JNI_ENTRY(jint, UnregisterNatives(JNIEnv *env, jclass clazz));

JNI_ENTRY(jint, MonitorEnter(JNIEnv *env, jobject obj));

JNI_ENTRY(jint, MonitorExit(JNIEnv *env, jobject obj));

JNI_ENTRY(jint, GetJavaVM(JNIEnv *env, JavaVM **vm));

JNI_ENTRY(void, GetStringRegion(JNIEnv *env, jstring str, jsize start, jsize len, jchar *buf));

JNI_ENTRY(void, GetStringUTFRegion(JNIEnv *env, jstring str, jsize start, jsize len, char *buf));

JNI_ENTRY(void *,GetPrimitiveArrayCritical(JNIEnv *env, jarray array, jboolean *isCopy));

JNI_ENTRY(void, ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void *carray, jint mode));

JNI_ENTRY(const jchar *,GetStringCritical(JNIEnv *env, jstring string, jboolean *isCopy));

JNI_ENTRY(void, ReleaseStringCritical(JNIEnv *env, jstring string, const jchar *cstring));

JNI_ENTRY(jweak, NewWeakGlobalRef(JNIEnv *env, jobject obj));

JNI_ENTRY(void, DeleteWeakGlobalRef(JNIEnv *env, jweak ref));

JNI_ENTRY(jboolean, ExceptionCheck(JNIEnv *env));

JNI_ENTRY(jobject, NewDirectByteBuffer(JNIEnv *env, void *address, jlong capacity));

JNI_ENTRY(void *,GetDirectBufferAddress(JNIEnv *env, jobject buf));

JNI_ENTRY(jlong, GetDirectBufferCapacity(JNIEnv *env, jobject buf));

JNI_ENTRY(jobjectRefType, GetObjectRefType(JNIEnv *env, jobject obj));


