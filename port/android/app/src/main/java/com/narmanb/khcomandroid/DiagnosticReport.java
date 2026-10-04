package com.narmanb.khcomandroid;

import android.app.ActivityManager;
import android.app.ApplicationExitInfo;
import android.content.Context;
import android.os.Build;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.List;

final class DiagnosticReport {
    private static ApplicationExitInfo lastExit(Context context) {
        if (Build.VERSION.SDK_INT < 30) return null;
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ApplicationExitInfo> exits = am.getHistoricalProcessExitReasons(null, 0, 1);
        return exits.isEmpty() ? null : exits.get(0);
    }

    static boolean hasNewCrash(Context context) {
        try {
            if (Build.VERSION.SDK_INT < 30) {
                File log = new File(context.getFilesDir(), "native.log");
                if (!log.isFile()) return false;
                String message = read(new FileInputStream(log));
                if (!message.contains("UNHANDLED SIGSEGV") &&
                    !message.contains("FATAL native abort")) return false;
                android.content.SharedPreferences prefs = context.getSharedPreferences("diagnostics", 0);
                if (prefs.getLong("reportedNativeLog", 0) >= log.lastModified()) return false;
                prefs.edit().putLong("reportedNativeLog", log.lastModified()).apply();
                return true;
            }
            ApplicationExitInfo exit = lastExit(context);
            if (exit == null) return false;
            int reason = exit.getReason();
            if (reason != ApplicationExitInfo.REASON_CRASH_NATIVE &&
                reason != ApplicationExitInfo.REASON_CRASH &&
                reason != ApplicationExitInfo.REASON_ANR &&
                reason != ApplicationExitInfo.REASON_SIGNALED) return false;
            android.content.SharedPreferences prefs = context.getSharedPreferences("diagnostics", 0);
            if (prefs.getLong("reportedExit", 0) >= exit.getTimestamp()) return false;
            prefs.edit().putLong("reportedExit", exit.getTimestamp()).apply();
            return true;
        } catch (Exception ignored) {
            return false;
        }
    }

    private static String read(InputStream input) throws Exception {
        if (input == null) return "No trace supplied by Android.\n";
        try (InputStream in = input; ByteArrayOutputStream out = new ByteArrayOutputStream()) {
            byte[] buffer = new byte[8192];
            int n;
            while (out.size() < 512 * 1024 &&
                   (n = in.read(buffer, 0, Math.min(buffer.length, 512 * 1024 - out.size()))) > 0) {
                out.write(buffer, 0, n);
            }
            return out.toString(StandardCharsets.UTF_8.name());
        }
    }

    static String capture(Context context) {
        StringBuilder text = new StringBuilder("KHCoM Android test2 diagnostics\n");
        text.append("Device: ").append(Build.MANUFACTURER).append(' ').append(Build.MODEL)
            .append("\nAndroid: ").append(Build.VERSION.RELEASE)
            .append(" API ").append(Build.VERSION.SDK_INT)
            .append("\nABIs: ").append(java.util.Arrays.toString(Build.SUPPORTED_ABIS)).append('\n');
        try {
            File log = new File(context.getFilesDir(), "native.log");
            if (log.isFile()) text.append("\nNative log:\n").append(read(new FileInputStream(log)));
            File previous = new File(context.getFilesDir(), "native.log.previous");
            if (previous.isFile()) text.append("\nPrior native log:\n").append(read(new FileInputStream(previous)));
        } catch (Exception e) { text.append("\nLog unavailable: ").append(e).append('\n'); }
        try {
            ApplicationExitInfo exit = lastExit(context);
            if (exit != null) {
                text.append("\nPrevious process exit: reason=").append(exit.getReason())
                    .append(" status=").append(exit.getStatus())
                    .append(" time=").append(exit.getTimestamp())
                    .append("\n").append(exit.getDescription()).append('\n');
                text.append(read(exit.getTraceInputStream()));
            }
        } catch (Exception e) { text.append("\nExit trace unavailable: ").append(e).append('\n'); }
        return text.toString();
    }
}
