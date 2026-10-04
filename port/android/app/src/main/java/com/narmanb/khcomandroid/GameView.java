package com.narmanb.khcomandroid;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

final class GameView extends View {
    private static final int GBA_A = 0x0001;
    private static final int GBA_B = 0x0002;
    private static final int GBA_SELECT = 0x0004;
    private static final int GBA_START = 0x0008;
    private static final int GBA_RIGHT = 0x0010;
    private static final int GBA_LEFT = 0x0020;
    private static final int GBA_UP = 0x0040;
    private static final int GBA_DOWN = 0x0080;
    private static final int GBA_R = 0x0100;
    private static final int GBA_L = 0x0200;
    private static final int FRAME_BYTES = 240 * 160 * 4;

    private final Bitmap bitmap = Bitmap.createBitmap(240, 160, Bitmap.Config.ARGB_8888);
    private final Paint paint = new Paint();
    private final Rect dest = new Rect();
    private final ByteBuffer frame = ByteBuffer.allocateDirect(FRAME_BYTES).order(ByteOrder.nativeOrder());

    private int digitalKeys;
    private int axisKeys;
    private int lastFrame = -1;

    GameView(Context context) {
        super(context);
        paint.setFilterBitmap(false);
        paint.setAntiAlias(false);
        setBackgroundColor(Color.BLACK);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        int frameNumber = NativeBridge.copyFrame(frame, lastFrame);
        if (frameNumber != lastFrame) {
            frame.position(0);
            bitmap.copyPixelsFromBuffer(frame);
            lastFrame = frameNumber;
        }

        int width = getWidth();
        int height = getHeight();
        int drawWidth = width;
        int drawHeight = width * 2 / 3;
        if (drawHeight > height) {
            drawHeight = height;
            drawWidth = height * 3 / 2;
        }

        int left = (width - drawWidth) / 2;
        int top = (height - drawHeight) / 2;
        dest.set(left, top, left + drawWidth, top + drawHeight);
        canvas.drawBitmap(bitmap, null, dest, paint);
        postInvalidateOnAnimation();
    }

    private static int gbaKeyForAndroid(int keyCode) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_DPAD_RIGHT: return GBA_RIGHT;
            case KeyEvent.KEYCODE_DPAD_LEFT: return GBA_LEFT;
            case KeyEvent.KEYCODE_DPAD_UP: return GBA_UP;
            case KeyEvent.KEYCODE_DPAD_DOWN: return GBA_DOWN;
            case KeyEvent.KEYCODE_BUTTON_A: return GBA_A;
            case KeyEvent.KEYCODE_BUTTON_B: return GBA_B;
            case KeyEvent.KEYCODE_BUTTON_L1: return GBA_L;
            case KeyEvent.KEYCODE_BUTTON_R1: return GBA_R;
            case KeyEvent.KEYCODE_BUTTON_START: return GBA_START;
            case KeyEvent.KEYCODE_BUTTON_SELECT:
            case KeyEvent.KEYCODE_BUTTON_MODE: return GBA_SELECT;
            default: return 0;
        }
    }

    private static boolean isControllerSource(KeyEvent event) {
        int source = event.getSource();
        return (source & (InputDevice.SOURCE_GAMEPAD |
                          InputDevice.SOURCE_JOYSTICK |
                          InputDevice.SOURCE_DPAD)) != 0;
    }

    private static void logDiagnosticKey(int keyCode, KeyEvent event, int gbaKey) {
        if (gbaKey == GBA_START || gbaKey == GBA_SELECT ||
            (gbaKey == 0 && isControllerSource(event))) {
            NativeBridge.logKeyEvent(event.getAction(), keyCode, event.getScanCode());
        }
    }

    private void pushKeys() {
        NativeBridge.setKeys(digitalKeys | axisKeys);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        int key = gbaKeyForAndroid(keyCode);
        logDiagnosticKey(keyCode, event, key);
        if (key == 0) {
            return super.onKeyDown(keyCode, event);
        }
        digitalKeys |= key;
        pushKeys();
        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        int key = gbaKeyForAndroid(keyCode);
        logDiagnosticKey(keyCode, event, key);
        if (key == 0) {
            return super.onKeyUp(keyCode, event);
        }
        digitalKeys &= ~key;
        pushKeys();
        return true;
    }

    private static float axis(MotionEvent event, InputDevice device, int axis) {
        InputDevice.MotionRange range = device.getMotionRange(axis, event.getSource());
        return range == null ? 0.0f : event.getAxisValue(axis);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) == 0 ||
            event.getAction() != MotionEvent.ACTION_MOVE) {
            return super.onGenericMotionEvent(event);
        }

        InputDevice device = event.getDevice();
        if (device == null) {
            return super.onGenericMotionEvent(event);
        }

        float x = axis(event, device, MotionEvent.AXIS_HAT_X);
        float y = axis(event, device, MotionEvent.AXIS_HAT_Y);
        if (Math.abs(x) < 0.5f && Math.abs(y) < 0.5f) {
            x = axis(event, device, MotionEvent.AXIS_X);
            y = axis(event, device, MotionEvent.AXIS_Y);
        }

        int keys = 0;
        if (x < -0.45f) keys |= GBA_LEFT;
        if (x > 0.45f) keys |= GBA_RIGHT;
        if (y < -0.45f) keys |= GBA_UP;
        if (y > 0.45f) keys |= GBA_DOWN;

        axisKeys = keys;
        pushKeys();
        return true;
    }
}
