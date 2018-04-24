#include <jni.h>
#include <string>
#include <android/log.h>
#include <sys/ptrace.h>
#include "crc.h"

#define TAG "jni" // 这个是自定义的LOG的标识
#ifdef NDEBUG
//#define DEAD (*(int *)0xdeaddead = 0xdead)
#define DEAD _exit(-1)
#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#else
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__) // 定义LOGD类型
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__) // 定义LOGW类型
#endif
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__) // 定义LOGF类型

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

#define LIB_A "/data/data/com.q2zy.babyandroid/lib/liba.so"
#define LIB_B "/data/data/com.q2zy.babyandroid/lib/libb.so"

uint64_t POLYNOMIAL __attribute__ ((visibility ("hidden"))) = 4374732215ULL;

__attribute__((noinline)) __attribute__ ((visibility ("hidden"))) time_t my_time() {
//    return time(NULL);
    return 0;
}

__attribute__((noinline)) __attribute__ ((visibility ("hidden"))) time_t my_time_garbage() {

//    // assert app name == "com.q2zy.babyandroid"
//    char buf[0x20];
//    FILE *fp = fopen("/proc/self/cmdline", "r");
//    fread(buf, 1, 0x1f, fp);
//    LOGE("loaded %d", time(NULL));
//    return NULL;
}
// 注册native方法到java中
__always_inline static int register_native_methods(JNIEnv *env, const char *className,
                                                   JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

__always_inline static void load_b(JNIEnv *env) {
    jclass runtimeCls = env->FindClass("java/lang/Runtime");
    if (!runtimeCls) {
        LOGW("Can't find runtime");
    }
    jmethodID getRuntimeMethod = env->GetStaticMethodID(runtimeCls, "getRuntime",
                                                        "()Ljava/lang/Runtime;");
    if (!getRuntimeMethod) {
        LOGW("Can't find getRuntime");
    }
    jmethodID loadMethod = env->GetMethodID(runtimeCls, "load",
                                            "(Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    if (!loadMethod) {
        LOGW("Can't find load");
    }
    jobject runtime = env->CallStaticObjectMethod(runtimeCls, getRuntimeMethod);

//    jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
//    jmethodID getSystemClassLoaderMethod = env->GetStaticMethodID(classLoaderCls, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
//    if (!getSystemClassLoaderMethod) {
//        LOGW("Can't find getSystemClassLoader");
//    }
//    jobject classLoader = env->CallStaticObjectMethod(classLoaderCls, getSystemClassLoaderMethod);
//    if (!classLoader) {
//        LOGW("class loader is null");
//    }

    jclass threadCls = env->FindClass("java/lang/Thread");
    jmethodID currentThreadMethod = env->GetStaticMethodID(threadCls, "currentThread",
                                                           "()Ljava/lang/Thread;");
    jobject thread = env->CallStaticObjectMethod(threadCls, currentThreadMethod);
//    jfieldID classField =  env->GetStaticFieldID(activityCls, "class", "Ljava/lang/Class");
//    if (!classField) {
//        LOGW("Can't find .class");
//    }
    jmethodID getClassLoaderMethod = env->GetMethodID(threadCls, "getContextClassLoader",
                                                      "()Ljava/lang/ClassLoader;");
//    jmethodID getClassLoaderMethod = env->GetMethodID(activityCls, "getClassLoader", "()Ljava/lang/ClassLoader;");
//    jmethodID getClassLoaderMethod = env->GetMethodID(activityCls, "getClass", "()Ljava/lang/Class;");
    if (!getClassLoaderMethod) {
        LOGW("Can't find getClassLoader");
    }
    LOGW("pass");
    jobject classLoader = env->CallObjectMethod(thread, getClassLoaderMethod);
    LOGW("pass");
    if (!classLoader) {
        LOGW("class loader is null");
    }
////
////    LOGW("pass");
    jclass cl = env->FindClass("java/lang/ClassLoader");
    if (!cl) {
        LOGW("no to cl");
    }
    jmethodID toStr = env->GetMethodID(cl, "toString", "()Ljava/lang/String;");
    if (!toStr) {
        LOGW("no to str");
    }
//    LOGW("pass");
    jstring s = (jstring) env->CallObjectMethod(classLoader, toStr);
//    LOGW("pass");

    const char *cs = env->GetStringUTFChars(s, false);
    LOGW("xxxx: %s", cs);

//    jmethodID toString = env->GetMethodID(classLoader)


    LOGW("before load b");
    jstring lib_b = env->NewStringUTF(LIB_B);
    env->CallVoidMethod(runtime, loadMethod, lib_b, classLoader);

////    jclass activity = env->FindClass("java/lang/System");
//    jclass activity = env->FindClass("com/q2zy/babyandroid/MainActivity");
//#ifndef NDEBUG
//    if (!activity) {
//        LOGW("Can't find activity.");
//        return;
//    }
//#endif
//    jmethodID loadLibrary = env->GetStaticMethodID(activity, "load",
//                                                     "(Ljava/lang/String;)V");
//#ifndef NDEBUG
//    if (!loadLibrary) {
//        LOGW("Can't find loadLibrary.");
//        return;
//    }
//#endif
//    LOGW("before load b");
//    jstring lib_b = env->NewStringUTF(LIB_B);
//    env->CallStaticVoidMethod(activity, loadLibrary, lib_b);
//    LOGW("ok come on!");
//    env->DeleteLocalRef(activity);
}

__always_inline static jboolean check(JNIEnv *env, jstring s) {
    jclass activity = env->FindClass("com/q2zy/babyandroid/MainActivity");
#ifndef NDEBUG
    if (!activity) {
        LOGW("Can't find activity.");
        return JNI_FALSE;
    }
#endif
    jmethodID func = env->GetStaticMethodID(activity, "check",
                                            "(Ljava/lang/String;)Z");
#ifndef NDEBUG
    if (!func) {
        LOGW("Can't find stringFromJNI.");
        return JNI_FALSE;
    }
#endif
    jboolean res = env->CallStaticBooleanMethod(activity, func, s);
    env->DeleteLocalRef(activity);
    return res;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_q2zy_babyandroid_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    char *crc = (char *) sub_E1C(LIB_A);
    return env->NewStringUTF(crc);
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    JNIEnv *env = NULL;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    // Java和JNI函数的绑定表
    JNINativeMethod method_table[] = {
            {"stringFromJNI", "()Ljava/lang/String;", (void *) Java_com_q2zy_babyandroid_MainActivity_stringFromJNI},//绑定
    };
    register_native_methods(env, "com/q2zy/babyandroid/MainActivity", method_table,
                            NELEM(method_table));

    load_b(env);
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_q2zy_babyandroid_MainActivity_check0(
        JNIEnv *env,
        jobject /* this */,
        jstring enc) {
    return check(env, enc);
}
