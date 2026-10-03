package com.narmanb.khcomandroid;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

final class NativeBridge {
    static {
        System.loadLibrary("khcom");
    }

    private NativeBridge() {}

    static native String start(String romPath, byte[] romMap);
    static native ByteBuffer frameBuffer();
    static native int frameCounter();
    static native void setKeys(int keys);
    static native boolean isStarted();

    static ByteBuffer nativeOrderFrameBuffer() {
        ByteBuffer buffer = frameBuffer();
        if (buffer != null) {
            buffer.order(ByteOrder.nativeOrder());
        }
        return buffer;
    }
}
