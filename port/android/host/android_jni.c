#include <jni.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>

#include "android_fault.h"
#include "android_host.h"
#include "android_rom.h"

void AgbMain(void);

static int sStarted;

static void* GameThread(void* unused) {
    (void)unused;
    AgbMain();
    return NULL;
}

JNIEXPORT jstring JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_start(
    JNIEnv* env, jclass clazz, jstring romPath, jbyteArray mapBytes) {
    const char* path;
    jbyte* map;
    jsize mapSize;
    char error[512];
    pthread_t thread;
    int rc;

    (void)clazz;

    if (sStarted) {
        return NULL;
    }
    if (romPath == NULL || mapBytes == NULL) {
        return (*env)->NewStringUTF(env, "ROM path or rommap.bin is missing");
    }

    path = (*env)->GetStringUTFChars(env, romPath, NULL);
    if (path == NULL) {
        return (*env)->NewStringUTF(env, "Could not read ROM path");
    }

    mapSize = (*env)->GetArrayLength(env, mapBytes);
    map = (*env)->GetByteArrayElements(env, mapBytes, NULL);
    if (map == NULL) {
        (*env)->ReleaseStringUTFChars(env, romPath, path);
        return (*env)->NewStringUTF(env, "Could not read rommap.bin");
    }

    error[0] = '\0';
    if (!AndroidHostInitSram(path, error, sizeof(error))) {
        (*env)->ReleaseByteArrayElements(env, mapBytes, map, JNI_ABORT);
        (*env)->ReleaseStringUTFChars(env, romPath, path);
        return (*env)->NewStringUTF(
            env, error[0] != '\0' ? error : "Could not initialize save data");
    }
    if (!AndroidRomLoad(path, map, (size_t)mapSize, error, sizeof(error))) {
        (*env)->ReleaseByteArrayElements(env, mapBytes, map, JNI_ABORT);
        (*env)->ReleaseStringUTFChars(env, romPath, path);
        return (*env)->NewStringUTF(
            env, error[0] != '\0' ? error : "ROM initialization failed");
    }

    (*env)->ReleaseByteArrayElements(env, mapBytes, map, JNI_ABORT);
    (*env)->ReleaseStringUTFChars(env, romPath, path);

    if (!AndroidFaultInit()) {
        return (*env)->NewStringUTF(
            env, "Could not install ARM32 GBA address fault handler");
    }

    rc = pthread_create(&thread, NULL, GameThread, NULL);
    if (rc != 0) {
        return (*env)->NewStringUTF(env, "Could not create native game thread");
    }
    pthread_detach(thread);
    sStarted = 1;
    return NULL;
}

JNIEXPORT jobject JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_frameBuffer(
    JNIEnv* env, jclass clazz) {
    (void)clazz;
    return (*env)->NewDirectByteBuffer(
        env,
        (void*)AndroidHostGetFrame(),
        (jlong)(240 * 160 * 4));
}

JNIEXPORT jint JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_frameCounter(
    JNIEnv* env, jclass clazz) {
    (void)env;
    (void)clazz;
    return (jint)AndroidHostGetFrameCounter();
}

JNIEXPORT void JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_setKeys(
    JNIEnv* env, jclass clazz, jint keys) {
    (void)env;
    (void)clazz;
    AndroidHostSetKeys((uint16_t)keys);
}

JNIEXPORT jboolean JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_isStarted(
    JNIEnv* env, jclass clazz) {
    (void)env;
    (void)clazz;
    return sStarted ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_flushSave(
    JNIEnv* env, jclass clazz) {
    (void)env;
    (void)clazz;
    AndroidHostFlushSram();
}

JNIEXPORT jint JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_readAudio(
    JNIEnv* env, jclass clazz, jshortArray output, jint frames) {
    jshort* samples;
    jsize capacity;
    int done;

    (void)clazz;
    if (output == NULL || frames <= 0) {
        return 0;
    }

    capacity = (*env)->GetArrayLength(env, output);
    if (capacity < frames * 2) {
        frames = capacity / 2;
    }
    samples = (*env)->GetShortArrayElements(env, output, NULL);
    if (samples == NULL) {
        return 0;
    }

    done = AndroidHostReadAudio((int16_t*)samples, frames);
    (*env)->ReleaseShortArrayElements(env, output, samples, 0);
    return done;
}

JNIEXPORT void JNICALL
Java_com_narmanb_khcomandroid_NativeBridge_setPaused(
    JNIEnv* env, jclass clazz, jboolean paused) {
    (void)env;
    (void)clazz;
    AndroidHostSetPaused(paused == JNI_TRUE);
}
