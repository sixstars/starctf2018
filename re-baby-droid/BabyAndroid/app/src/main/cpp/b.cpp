#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
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
#define DEAD LOGE("app dead!")
//#endif
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__) // 定义LOGV类型
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
int a[] __attribute__ ((visibility ("hidden"))) = {0xdead0001, 0xdeadff01, 0xdead0002, 0xdeadff02};

__always_inline static const char *get_key(JNIEnv *env) {
    jclass activity = env->FindClass("com/q2zy/babyandroid/MainActivity");
#ifndef NDEBUG
    if (!activity) {
        LOGW("Can't find activity.");
        return JNI_FALSE;
    }
#endif
    jmethodID stringFromJNI = env->GetStaticMethodID(activity, "stringFromJNI",
                                                     "()Ljava/lang/String;");
#ifndef NDEBUG
    if (!stringFromJNI) {
        LOGW("Can't find stringFromJNI.");
        return JNI_FALSE;
    }
#endif
    jstring key = (jstring) env->CallStaticObjectMethod(activity, stringFromJNI);
    env->DeleteLocalRef(activity);
    jboolean isCopy;
    const char *c_key = env->GetStringUTFChars(key, &isCopy);
    return c_key;
}

//JNIEXPORT jboolean JNICALL
static jboolean JNICALL
check(
        JNIEnv *env,
        jobject,
        jstring enc) {
    const char *c_key = get_key(env);
    const char *c_enc = env->GetStringUTFChars(enc, NULL);
    LOGD("key: %s", c_key);
    LOGD("enc: %s", c_enc);
    long long xor_key = (long long) *(int *) c_key;
    xor_key *= xor_key;
    for (int i = 0; i < 8; ++i) {
        *(long long *) (c_enc + i) ^= xor_key;
    }
    *(long long *) (c_enc + 15) ^= xor_key;
//    char aim[] = "\x88\x19\xd6\x8a\xa6\xec\x9b\x89\x17\xd0oy;\x1b]\x89\xa7\x91W,,4\x0e\x00";
    char aim[] = "\xd5\x08\x4b\x11\xbf\x0c\x0b\x1f\xdc\x57\x64\x74\xb4\x6d\x5b\xd4\xeb\x1d\x51\xae\xd5\x44\x08";
    if (strcmp(c_enc, aim) == 0) {
        return JNI_TRUE;
    } else {
        LOGE("Attacker detected!");
        DEAD;
        return JNI_FALSE;
    }
}

// 注册native方法到java中
__always_inline static int register_native_methods(JNIEnv *env, const char *className,
                                                   JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass("java/lang/Thread");
    if (clazz == NULL) {
        LOGW("clazz Thread not found!");
    } else {
        LOGW("clazz Thread found!");
    }
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGW("clazz %s not found!", className);
        return JNI_FALSE;
    } else {
        LOGW("clazz %s found!", className);
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGW("register failed!");
        return JNI_FALSE;
    }
    LOGW("register success!");
    return JNI_TRUE;
}

__always_inline static void str_xor(char *s, int key) {
    while (*s) {
        *s++ ^= key;
    }
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    LOGD("fuck");
    if (env == NULL) {
        LOGW("env is null");
    }

    LOGD("fuck");
    // assert crc for liba and libb are both '*ctf'
    const char *crc = sub_E1C(LIB_A);
    if ((strcmp(sub_E1C(LIB_A), sub_E1C(LIB_B)) != 0)) {
//    if ((strcmp(sub_E1C(LIB_A), sub_E1C(LIB_B)) != 0) ||
//        (*(int *) crc != 0x6674632a)) {
        LOGD("crc not match");
        DEAD;
    }
    LOGD("fuck");

//    sleep(2);
    if (((my_time() >> 1 << 1) ^ POLYNOMIAL) != (4374732215ULL - 1)) {
        LOGW("time incorrect, %#llx != %#llx", (my_time() >> 1 << 1) ^ POLYNOMIAL, 4374732215ULL - 1);
        DEAD;
    }

    char reg_class[] = "\xf0\xfc\xfe\xbc\xe2\xa1\xe9\xea\xbc\xf1\xf2\xf1\xea\xf2\xfd\xf7\xe1\xfc\xfa\xf7\xbc\xde\xf2\xfa\xfd\xd2\xf0\xe7\xfa\xe5\xfa\xe7\xea\x00";
    char reg_func[] = "sxus{\x00";
    char reg_func_sig[] = ">Z|w`w9zwxq9Ebd\x7fxq-?L";
//    char reg_class[] = "com/q2zy/babyandroid/MainActivity\x00";
//    char reg_func[] = "check\x00";
//    char reg_func_sig[] = "(Ljava/lang/String;)Z\x00";
    str_xor(reg_class, 0x93);
    str_xor(reg_func, 0x10);
    str_xor(reg_func_sig, 0x16);
#ifndef NDEBUG
    LOGD("%s", reg_class);
    LOGD("%s", reg_func);
    LOGD("%s", reg_func_sig);
#endif

    // assert trace id == getppid()
    char buf[0x100];
    FILE *fp = fopen("/proc/self/status", "r");
    for (int i = 0; i < 6; ++i) {
        fgets(buf, 0xff, fp);
    }
    LOGD("trace id: %s", buf + 11);
    if (atoi(buf + 11) != getppid()) {
        LOGD("trace id not match process id");
        DEAD;
    }

    // Java和JNI函数的绑定表
    JNINativeMethod method_table[] = {
            {reg_func, reg_func_sig, (void *) check},//绑定
    };
    register_native_methods(env, reg_class, method_table, NELEM(method_table));

    for (int i = a[2]; i < a[3]; i += 4) {
        int *addr = (int *) (((int) JNI_OnLoad & (~1)) + i);
        LOGD("%x before: %x", i, *addr);
        short t = *(short *) addr;
        *(short *) addr = *(short *) ((int) addr + 2);
        *(short *) ((int) addr + 2) = t;
        LOGD("%x middle: %x", i, *addr);
        *addr ^= *(int *) crc;
        LOGD("%x after: %x", i, *addr);
    }

//    LOGE("loaded");
    // 返回jni的版本
    return JNI_VERSION_1_6;
}

__attribute__((noinline)) __attribute__ ((visibility ("hidden"))) time_t my_time() {
    fprintf(stderr, "loaded");
    return time(NULL);
}

__attribute__((noinline)) __attribute__ ((visibility ("hidden"))) time_t my_time_garbage() {

    // assert app name == "com.q2zy.babyandroid"
    char buf[0x20];
    FILE *fp = fopen("/proc/self/cmdline", "r");
    fread(buf, 1, 0x1f, fp);
//    LOGE("loaded %d", time(NULL));
    return NULL;
}

static void before_load(void) __attribute__((constructor (111)));

void before_load(void) {
    LOGD("running before JNI load");

    // assert app name == "com.q2zy.babyandroid"
    char buf[0x20];
    FILE *fp = fopen("/proc/self/cmdline", "r");
    fread(buf, 1, 0x1f, fp);
    LOGD("cmdline: %s", buf);
    if (strcmp(buf, "com.q2zy.babyandroid") != 0) {
        LOGD("app name not match");
        DEAD;
    }

    // use crc of libb to decrypt JNI_OnLoad
    const char *crc = sub_E1C(LIB_B);
    LOGD("%#x", *(int *) crc);
    mprotect((void *) ((int) JNI_OnLoad & (~0xfff)), 0x2000, PROT_READ | PROT_WRITE | PROT_EXEC);
    for (int i = a[0]; i < a[1]; i += 4) {
        int *addr = (int *) (((int) JNI_OnLoad & (~1)) + i);
        LOGV("%x before: %x", i, *addr);
        *addr ^= *(int *) crc;
        LOGV("%x after: %x", i, *addr);
    }

//    LOGE("fuckfuckfuck");

    POLYNOMIAL = (my_time() >> 1 << 1) ^ (4374732215ULL - 1);
//    POLYNOMIAL = (my_time() * 2);
}
