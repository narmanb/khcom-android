package com.narmanb.khcomandroid;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public final class MainActivity extends Activity {
    private static final int PICK_ROM = 1001;
    private static final String ROM_NAME = "rom.gba";

    private AudioPlayer audioPlayer;

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        hideSystemUi();

        File rom = new File(getFilesDir(), ROM_NAME);
        if (rom.isFile()) {
            startGame(rom);
        } else {
            pickRom();
        }
    }

    @Override
    protected void onPause() {
        if (NativeBridge.isStarted()) {
            NativeBridge.setPaused(true);
            NativeBridge.flushSave();
        }
        if (audioPlayer != null) {
            audioPlayer.pause();
        }
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (NativeBridge.isStarted()) {
            NativeBridge.setPaused(false);
        }
        if (audioPlayer != null) {
            audioPlayer.resume();
        }
        hideSystemUi();
    }

    @Override
    protected void onDestroy() {
        if (audioPlayer != null) {
            audioPlayer.stop();
            audioPlayer = null;
        }
        super.onDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            hideSystemUi();
        }
    }

    private void hideSystemUi() {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_FULLSCREEN |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
            View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    private void pickRom() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("application/octet-stream");
        intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
            "application/octet-stream",
            "application/x-gba-rom",
            "*/*"
        });
        startActivityForResult(intent, PICK_ROM);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode != PICK_ROM) {
            return;
        }
        if (resultCode != RESULT_OK || data == null || data.getData() == null) {
            finish();
            return;
        }

        File rom = new File(getFilesDir(), ROM_NAME);
        try {
            copyUri(data.getData(), rom);
            startGame(rom);
        } catch (IOException e) {
            showError("Could not copy the selected ROM: " + e.getMessage(), true);
        }
    }

    private void copyUri(Uri uri, File destination) throws IOException {
        try (InputStream in = getContentResolver().openInputStream(uri);
             FileOutputStream out = new FileOutputStream(destination)) {
            if (in == null) {
                throw new IOException("Android could not open the selected file");
            }
            byte[] buffer = new byte[1024 * 1024];
            int count;
            while ((count = in.read(buffer)) >= 0) {
                out.write(buffer, 0, count);
            }
        }
    }

    private byte[] readRomMap() throws IOException {
        try (InputStream in = getAssets().open("rommap.bin");
             ByteArrayOutputStream out = new ByteArrayOutputStream()) {
            byte[] buffer = new byte[64 * 1024];
            int count;
            while ((count = in.read(buffer)) >= 0) {
                out.write(buffer, 0, count);
            }
            return out.toByteArray();
        }
    }

    private void startGame(File rom) {
        try {
            String error = NativeBridge.start(rom.getAbsolutePath(), readRomMap());
            if (error != null) {
                rom.delete();
                showError(error, true);
                return;
            }
            setContentView(new GameView(this));
            audioPlayer = new AudioPlayer();
            audioPlayer.start();
        } catch (IOException e) {
            showError("rommap.bin is missing from this build.", false);
        } catch (UnsatisfiedLinkError e) {
            showError("The native KHCoM library is missing or could not load.", false);
        }
    }

    private void showError(String message, boolean allowReselect) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this)
            .setTitle("KHCoM Android")
            .setMessage(message)
            .setCancelable(false);

        if (allowReselect) {
            builder.setPositiveButton("Choose ROM", (dialog, which) -> pickRom());
        } else {
            builder.setPositiveButton("Close", (dialog, which) -> finish());
        }
        builder.show();
    }
}
