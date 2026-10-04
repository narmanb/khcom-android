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
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;

public final class MainActivity extends Activity {
    private static final int PICK_ROM = 1001;
    private static final int SAVE_REPORT = 1002;
    private static final String ROM_NAME = "rom.gba";

    private AudioPlayer audioPlayer;
    private boolean nativeReady;
    private boolean launchAfterReport;
    private boolean exportInProgress;
    private String pendingReport;

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        hideSystemUi();

        if (state != null && state.getBoolean("exportInProgress")) {
            // The document picker may outlive this Activity or process. Keep
            // the previous log intact until its snapshot has been exported.
            exportInProgress = true;
            launchAfterReport = true;
            return;
        }

        if (DiagnosticReport.hasNewCrash(this)) {
            pendingReport = DiagnosticReport.capture(this);
            new AlertDialog.Builder(this)
                .setTitle("Previous run stopped unexpectedly")
                .setMessage("Save the diagnostic report and send it back with what you saw on screen.")
                .setCancelable(false)
                .setPositiveButton("Save report", (dialog, which) -> exportReport(true))
                .setNegativeButton("Continue", (dialog, which) -> launchGame())
                .show();
        } else {
            launchGame();
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle state) {
        state.putBoolean("exportInProgress", exportInProgress);
        super.onSaveInstanceState(state);
    }

    private void launchGame() {
        File rom = new File(getFilesDir(), ROM_NAME);
        if (rom.isFile()) {
            startGame(rom);
        } else {
            pickRom();
        }
    }

    @Override
    protected void onPause() {
        if (nativeReady && NativeBridge.isStarted()) {
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
        if (nativeReady && NativeBridge.isStarted()) {
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

        if (requestCode == SAVE_REPORT) {
            if (resultCode == RESULT_OK && data != null && data.getData() != null) {
                try (InputStream in = new FileInputStream(new File(getFilesDir(), "pending-diagnostics.txt"));
                     OutputStream out = getContentResolver().openOutputStream(data.getData())) {
                    if (out == null) throw new IOException("Could not open report destination");
                    byte[] buffer = new byte[8192];
                    int n;
                    while ((n = in.read(buffer)) > 0) out.write(buffer, 0, n);
                } catch (IOException e) {
                    android.widget.Toast.makeText(this, "Could not save report: " + e.getMessage(),
                        android.widget.Toast.LENGTH_LONG).show();
                }
            }
            pendingReport = null;
            exportInProgress = false;
            if (launchAfterReport) { launchAfterReport = false; launchGame(); }
            return;
        }

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
            nativeReady = true;
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
        } catch (LinkageError e) {
            pendingReport = DiagnosticReport.capture(this) + "\nLibrary load error: " + e;
            new AlertDialog.Builder(this).setTitle("Native library could not load")
                .setMessage(e.toString())
                .setPositiveButton("Save report", (dialog, which) -> exportReport(false))
                .setNegativeButton("Close", (dialog, which) -> finish()).show();
        }
    }

    private void exportReport(boolean launchAfter) {
        launchAfterReport = launchAfter;
        if (pendingReport == null) pendingReport = DiagnosticReport.capture(this);
        try (OutputStream out = new FileOutputStream(new File(getFilesDir(), "pending-diagnostics.txt"))) {
            out.write(pendingReport.getBytes(StandardCharsets.UTF_8));
        } catch (IOException e) {
            android.widget.Toast.makeText(this, "Could not prepare report: " + e.getMessage(),
                android.widget.Toast.LENGTH_LONG).show();
            if (launchAfterReport) { launchAfterReport = false; launchGame(); }
            return;
        }
        exportInProgress = true;
        Intent intent = new Intent(Intent.ACTION_CREATE_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("text/plain");
        intent.putExtra(Intent.EXTRA_TITLE, "KHCoM-test2-diagnostics.txt");
        startActivityForResult(intent, SAVE_REPORT);
    }

    @Override
    public void onBackPressed() {
        new AlertDialog.Builder(this).setTitle("KHCoM Android")
            .setItems(new String[] {"Save diagnostic report", "Resume"}, (dialog, which) -> {
                if (which == 0) { pendingReport = null; exportReport(false); }
            }).show();
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
