package org.oracles;

import android.app.Presentation;
import android.content.Context;
import android.graphics.Typeface;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Handler;
import android.widget.ScrollView;
import android.os.Bundle;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

final class CompanionScreen extends Presentation {
    interface SelectionListener { void selected(boolean map); }

    private final SelectionListener listener;
    private boolean map;
    private Button inventoryTab, mapTab;
    private TextView page;
    private LinearLayout items, slots;
    private ScrollView scroll;
    private int[] snapshot;
    private int selected;
    private static final int RECORD = 258;
    private static final String[] KEYS = {"A", "B", "X", "Y"};
    private static final String[] NAMES = {"", "Shield", "Punch", "Bombs", "Cane of Somaria",
        "Sword", "Boomerang", "Rod of Seasons", "Magnet Gloves", "", "Switch Hook", "",
        "Biggoron's Sword", "Bombchus", "Flute", "Seed Shooter", "", "Harp of Ages", "",
        "Slingshot", "", "Shovel", "Power Bracelet", "Roc's Feather", "", "Seed Satchel"};
    private static native int[] inventory(int revision);
    private static native void assign(int revision, int slot, int item);
    private final Handler handler = new Handler(android.os.Looper.getMainLooper());
    private final Runnable poll = new Runnable() {
        @Override public void run() {
            if (!map) {
                int[] changed = inventory(snapshot == null ? -1 : snapshot[0]);
                if (changed != null) {
                    snapshot = changed;
                    render();
                }
            }
            handler.postDelayed(this, 160);
        }
    };

    @Override protected void onStart() {
        super.onStart();
        handler.post(poll);
    }

    @Override protected void onStop() {
        handler.removeCallbacks(poll);
        super.onStop();
    }

    CompanionScreen(Context game, Display display, boolean map, SelectionListener listener) {
        super(game, display);
        this.map = map;
        this.listener = listener;
    }

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        // Touch belongs to this window. Keep keyboard and controller focus with SDL.
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

        Context context = getContext(); // Uses the companion display's density.
        LinearLayout root = new LinearLayout(context);
        root.setOrientation(LinearLayout.VERTICAL);
        // Some secondary displays keep their system bars visible over a presentation.
        root.setOnApplyWindowInsetsListener((view, insets) -> {
            view.setPadding(insets.getStableInsetLeft(), insets.getStableInsetTop(),
                    insets.getStableInsetRight(), insets.getStableInsetBottom());
            return insets;
        });
        root.setBackgroundColor(0xfff1e6be);
        LinearLayout tabs = new LinearLayout(context);
        tabs.setBackgroundColor(0xff233831);
        root.addView(tabs);
        inventoryTab = addTab(tabs, "Inventory", false);
        mapTab = addTab(tabs, "Map", true);
        page = new TextView(context);
        page.setGravity(Gravity.CENTER);
        page.setTextColor(0xff283c34);
        page.setTextSize(28);
        page.setTypeface(Typeface.SERIF, Typeface.BOLD);
        root.addView(page);
        scroll = new ScrollView(context);
        items = new LinearLayout(context);
        items.setOrientation(LinearLayout.VERTICAL);
        scroll.addView(items);
        root.addView(scroll, new LinearLayout.LayoutParams(-1, 0, 1));
        slots = new LinearLayout(context);
        root.addView(slots);
        setContentView(root);
        render();
    }

    private Button addTab(LinearLayout tabs, String label, boolean showMap) {
        Button button = new Button(getContext());
        button.setText(label);
        button.setMinHeight(Math.round(52 * getContext().getResources().getDisplayMetrics().density));
        button.setOnClickListener(v -> {
            map = showMap;
            listener.selected(map);
            render();
        });
        tabs.addView(button, new LinearLayout.LayoutParams(0, -2, 1));
        return button;
    }

    private void render() {
        styleTab(inventoryTab, !map);
        styleTab(mapTab, map);
        scroll.setVisibility(map ? View.GONE : View.VISIBLE);
        slots.setVisibility(map ? View.GONE : View.VISIBLE);
        if (map) {
            page.setText("Map");
            return;
        }
        items.removeAllViews();
        slots.removeAllViews();
        if (snapshot == null || snapshot[1] == 0) {
            selected = 0;
            page.setText("Open a game file to see your items");
            return;
        }
        if (snapshot.length == 6) {
            selected = 0;
            page.setText("No items yet");
            return;
        }
        boolean found = false;
        for (int pos = 6; pos < snapshot.length; pos += RECORD) found |= snapshot[pos] == selected;
        if (!found) selected = 0;
        boolean canEquip = snapshot[1] > 0;
        page.setText(!canEquip ? "Inventory · Equipping paused"
                : selected == 0 ? "Choose an item" : name(selected) + " · Choose a slot");
        LinearLayout row = null;
        for (int pos = 6, i = 0; pos < snapshot.length; pos += RECORD, i++) {
            int id = snapshot[pos], count = snapshot[pos + 1];
            if (i % 3 == 0) {
                row = new LinearLayout(getContext());
                items.addView(row);
            }
            Button button = new Button(getContext());
            button.setAllCaps(false);
            button.setText(name(id) + (count < 0 ? "" : " ×" + count));
            Bitmap image = Bitmap.createBitmap(snapshot, pos + 2, 16, 16, 16, Bitmap.Config.ARGB_8888);
            BitmapDrawable sprite = new BitmapDrawable(getContext().getResources(), image);
            sprite.setFilterBitmap(false);
            int size = Math.round(40 * getContext().getResources().getDisplayMetrics().density);
            sprite.setBounds(0, 0, size, size);
            button.setCompoundDrawables(null, sprite, null, null);
            styleTab(button, id == selected);
            button.setOnClickListener(v -> { selected = id; render(); });
            row.addView(button, new LinearLayout.LayoutParams(0, -2, 1));
        }
        for (int i = 0; i < Math.abs(snapshot[1]); i++) {
            final int slot = i;
            Button button = new Button(getContext());
            int id = snapshot[2+i];
            button.setAllCaps(false);
            button.setText(KEYS[i] + "\n" + (id == 0 ? "—" : name(id)));
            button.setEnabled(canEquip && selected != 0);
            button.setOnClickListener(v -> assign(snapshot[0], slot, selected));
            slots.addView(button, new LinearLayout.LayoutParams(0, -2, 1));
        }
    }

    private static String name(int id) {
        if (id == 30) return "Fool’s Ore";
        if (id < NAMES.length && !NAMES[id].isEmpty()) return NAMES[id];
        return "Item " + id;
    }

    private static void styleTab(Button button, boolean selected) {
        button.setSelected(selected);
        button.setBackgroundColor(selected ? 0xffefd58c : 0xff364e42);
        button.setTextColor(selected ? 0xff25382d : 0xffe4dcba);
    }
}
