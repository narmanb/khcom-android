package com.narmanb.khcomandroid;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

final class AudioPlayer {
    private static final int RATE = 48000;
    private static final int FRAMES = 768;

    private final Object stateLock = new Object();
    private AudioTrack track;
    private Thread thread;
    private volatile boolean running;
    private volatile boolean paused;

    void start() {
        if (running) {
            return;
        }

        int minBytes = AudioTrack.getMinBufferSize(
            RATE,
            AudioFormat.CHANNEL_OUT_STEREO,
            AudioFormat.ENCODING_PCM_16BIT);
        int bufferBytes = Math.max(minBytes, FRAMES * 2 * 2 * 4);

        track = new AudioTrack.Builder()
            .setAudioAttributes(new AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_GAME)
                .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                .build())
            .setAudioFormat(new AudioFormat.Builder()
                .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                .setSampleRate(RATE)
                .setChannelMask(AudioFormat.CHANNEL_OUT_STEREO)
                .build())
            .setBufferSizeInBytes(bufferBytes)
            .setTransferMode(AudioTrack.MODE_STREAM)
            .build();

        if (track.getState() != AudioTrack.STATE_INITIALIZED) {
            track.release();
            track = null;
            return;
        }

        running = true;
        paused = false;
        track.play();
        thread = new Thread(this::runAudio, "khcom-audio");
        thread.setPriority(Thread.MAX_PRIORITY);
        thread.start();
    }

    private void runAudio() {
        short[] samples = new short[FRAMES * 2];

        while (running) {
            synchronized (stateLock) {
                while (running && paused) {
                    try {
                        stateLock.wait();
                    } catch (InterruptedException ignored) {
                    }
                }
            }
            if (!running) {
                break;
            }

            int frames = NativeBridge.readAudio(samples, FRAMES);
            int shorts = frames * 2;
            int offset = 0;
            while (running && !paused && offset < shorts) {
                int written = track.write(
                    samples,
                    offset,
                    shorts - offset,
                    AudioTrack.WRITE_BLOCKING);
                if (written <= 0) {
                    break;
                }
                offset += written;
            }
        }
    }

    void pause() {
        paused = true;
        if (track != null) {
            track.pause();
            track.flush();
        }
    }

    void resume() {
        if (!running || track == null) {
            return;
        }
        paused = false;
        track.play();
        synchronized (stateLock) {
            stateLock.notifyAll();
        }
    }

    void stop() {
        running = false;
        paused = false;
        synchronized (stateLock) {
            stateLock.notifyAll();
        }
        if (thread != null) {
            thread.interrupt();
            thread = null;
        }
        if (track != null) {
            track.pause();
            track.flush();
            track.release();
            track = null;
        }
    }
}
