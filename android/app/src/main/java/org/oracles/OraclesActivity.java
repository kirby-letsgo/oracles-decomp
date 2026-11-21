package org.oracles;

import android.hardware.display.DisplayManager;
import android.view.Display;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.view.HapticFeedbackConstants;
import android.view.View;
import org.libsdl.app.SDLActivity;

public class OraclesActivity extends SDLActivity implements DisplayManager.DisplayListener {
    private DisplayManager displays;
    private CompanionScreen companion;
    private int selectedTab;
    private boolean companionEnabled;

    public void setCompanionEnabled(boolean enabled) {
        runOnUiThread(() -> {
            if (companionEnabled == enabled) return;
            companionEnabled = enabled;
            updateCompanion();
        });
    }

    private static native void controllerBack(boolean down);

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // Some handhelds expose the Linux KEY_BACK key on the gamepad itself.
        // SDL's controller path folds Android Back into View/Select.
        if (!mBrokenLibraries && event.getKeyCode() == KeyEvent.KEYCODE_BACK
                && event.getScanCode() == 158 && event.getDevice() != null
                && event.getDevice().supportsSource(android.view.InputDevice.SOURCE_GAMEPAD)) {
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                if (event.getRepeatCount() == 0) controllerBack(true);
                return true;
            }
            if (event.getAction() == KeyEvent.ACTION_UP) {
                controllerBack(false);
                return true;
            }
        }
        return super.dispatchKeyEvent(event);
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (mBrokenLibraries) return;
        displays = getSystemService(DisplayManager.class);
        displays.registerDisplayListener(this, null);
        updateCompanion();
    }

    @Override
    protected void onStop() {
        if (displays != null) displays.unregisterDisplayListener(this);
        displays = null;
        dismissCompanion();
        super.onStop();
    }

    @Override public void onDisplayAdded(int id) { updateCompanion(); }
    @Override public void onDisplayChanged(int id) { updateCompanion(); }
    @Override public void onDisplayRemoved(int id) { updateCompanion(); }

    private void dismissCompanion() {
        if (companion != null) {
            companion.dismiss();
            companion = null;
        }
    }

    private void updateCompanion() {
        if (!companionEnabled || displays == null) {
            dismissCompanion();
            return;
        }
        int gameDisplay = getWindowManager().getDefaultDisplay().getDisplayId();
        for (Display display : displays.getDisplays(DisplayManager.DISPLAY_CATEGORY_PRESENTATION)) {
            if (display.getDisplayId() == gameDisplay) continue;
            if (companion != null && companion.isShowing()
                    && companion.getDisplay().getDisplayId() == display.getDisplayId()) return;
            dismissCompanion();
            CompanionScreen screen = new CompanionScreen(this, display, selectedTab,
                    selected -> selectedTab = selected);
            try {
                screen.show();
                companion = screen;
                return;
            } catch (WindowManager.InvalidDisplayException ignored) {
                // The display may disconnect between enumeration and show().
            }
        }
        dismissCompanion();
    }

    @Override
    protected String[] getLibraries() {
        return new String[] { "main" };
    }

    // A key tap's feedback on the game's view (called from C, see src/platform/haptic.c). It follows
    // the system's touch feedback setting, and runs on the UI thread as views expect.
    public static void haptic() {
        final View view = mSurface;
        if (view == null) return;
        view.post(new Runnable() {
            @Override
            public void run() {
                view.performHapticFeedback(HapticFeedbackConstants.KEYBOARD_TAP);
            }
        });
    }
}
