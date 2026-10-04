package com.narmanb.khcomandroid;

import java.nio.ByteBuffer;

final class NativeBridge {
    static {
        System.loadLibrary("khcom");
    }

    private NativeBridge() {}

    static native String start(String romPath, byte[] romMap);
    static native int copyFrame(ByteBuffer target, int lastFrame);
    static native void setKeys(int keys);
    static native void logKeyEvent(int action, int keyCode, int scanCode);
    static native boolean isStarted();
    static native void flushSave();
    static native int readAudio(short[] output, int frames);
    static native void setPaused(boolean paused);

}
