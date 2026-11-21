package org.oracles;

import android.app.Presentation;
import android.content.Context;
import android.graphics.Typeface;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Handler;
import android.widget.ScrollView;
import android.widget.ImageView;
import android.os.Bundle;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

final class CompanionScreen extends Presentation {
    interface SelectionListener { void selected(int tab); }

    private final SelectionListener listener;
    private static final int INVENTORY = 0, MAP = 1, GUIDE = 2;
    private int tab;
    private WalkthroughView guide;
    private Button inventoryTab, mapTab, guideTab;
    private TextView page;
    private LinearLayout items, slots;
    private ScrollView scroll;
    private int[] snapshot;
    private int selected;
    private ImageView mapImage;
    private int[] mapSnapshot;
    private static native int[] worldMap(int revision);
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
            if (tab != INVENTORY) {
                int[] changed = worldMap(mapSnapshot == null ? -1 : mapSnapshot[0]);
                if (changed != null) {
                    mapSnapshot = changed;
                    if (tab == MAP) renderMap(); else renderGuide();
                }
            } else {
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

    CompanionScreen(Context game, Display display, int tab, SelectionListener listener) {
        super(game, display);
        this.tab = tab;
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
        inventoryTab = addTab(tabs, "Inventory", INVENTORY);
        mapTab = addTab(tabs, "Map", MAP);
        guideTab = addTab(tabs, "Guide", GUIDE);
        page = new TextView(context);
        page.setGravity(Gravity.CENTER);
        page.setTextColor(0xff283c34);
        page.setTextSize(28);
        page.setTypeface(Typeface.SERIF, Typeface.BOLD);
        root.addView(page);
        mapImage = new ImageView(context);
        mapImage.setScaleType(ImageView.ScaleType.FIT_CENTER);
        root.addView(mapImage, new LinearLayout.LayoutParams(-1, 0, 1));
        scroll = new ScrollView(context);
        items = new LinearLayout(context);
        items.setOrientation(LinearLayout.VERTICAL);
        scroll.addView(items);
        root.addView(scroll, new LinearLayout.LayoutParams(-1, 0, 1));
        slots = new LinearLayout(context);
        root.addView(slots);
        container = root;
        setContentView(root);
        render();
    }

    private Button addTab(LinearLayout tabs, String label, int next) {
        Button button = new Button(getContext());
        button.setText(label);
        button.setMinHeight(Math.round(52 * getContext().getResources().getDisplayMetrics().density));
        button.setOnClickListener(v -> {
            tab = next;
            listener.selected(tab);
            render();
        });
        tabs.addView(button, new LinearLayout.LayoutParams(0, -2, 1));
        return button;
    }

    private void render() {
        boolean map = tab == MAP;
        styleTab(guideTab, tab == GUIDE);
        styleTab(inventoryTab, tab == INVENTORY);
        styleTab(mapTab, map);
        scroll.setVisibility(tab == INVENTORY ? View.VISIBLE : View.GONE);
        slots.setVisibility(tab == INVENTORY ? View.VISIBLE : View.GONE);
        mapImage.setVisibility(map ? View.VISIBLE : View.GONE);
        if (guide != null) guide.setVisibility(tab == GUIDE ? View.VISIBLE : View.GONE);
        page.setVisibility(View.VISIBLE);
        if (tab == GUIDE) {
            renderGuide();
            return;
        }
        if (map) {
            renderMap();
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

    private LinearLayout container;

    private void renderGuide() {
        if (mapSnapshot == null || mapSnapshot[1] == -2) {
            page.setVisibility(View.VISIBLE);
            page.setText("Open Ages or Seasons to read its walkthrough");
            if (guide != null) guide.setVisibility(View.GONE);
            return;
        }
        if (guide == null) {
            guide = new WalkthroughView(getContext());
            container.addView(guide, new LinearLayout.LayoutParams(-1, 0, 1));
        }
        page.setVisibility(View.GONE);
        guide.setVisibility(View.VISIBLE);
        guide.setGame(mapSnapshot[1] == -1 ? "ages" : "seasons");
    }

    @Override public void dismiss() {
        super.dismiss();
        if (guide != null) {
            guide.close();
            guide = null;
        }
    }

    private void renderMap() {
        int plane = mapSnapshot == null ? -2 : mapSnapshot[1];
        if (plane != 0 && plane != 1) {
            mapImage.setImageDrawable(null);
            page.setText(plane == -2 ? "Open a game file to see the map" :
                    plane == 2 ? "Dungeon map unavailable" : "Map available in Seasons");
            return;
        }
        page.setText(plane == 0 ? "Holodrum" : "Subrosia");
        int width = plane == 0 ? 128 : 88, height = plane == 0 ? 128 : 64;
        Bitmap image = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        image.setPixels(mapSnapshot, 3, width, 0, 0, width, height);
        int room = mapSnapshot[2], cx = (room % 16) * 8 + 3, cy = (room / 16) * 8 + 3;
        if (cx < width && cy < height) {
            for (int y = -3; y <= 3; y++) for (int x = -3; x <= 3; x++) {
                int distance = Math.abs(x) + Math.abs(y);
                if (distance <= 3) image.setPixel(cx+x, cy+y, distance == 3 ? 0xff26332c : 0xffffdf55);
            }
        }
        BitmapDrawable art = new BitmapDrawable(getContext().getResources(), image);
        art.setFilterBitmap(false);
        mapImage.setImageDrawable(art);
        mapImage.setContentDescription(page.getText() + ", Link at column " + (room%16+1) + ", row " + (room/16+1));
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
