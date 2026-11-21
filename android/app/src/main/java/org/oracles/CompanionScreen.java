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
    private CompanionMapView mapImage;
    private LinearLayout floorButtons;
    private int selectedFloor, previousDungeon = -1, previousLinkFloor = -1;
    private int currentGuideGame = -1;
    private static native int guideGame();
    private static final int MAP_REVISION = 0, MAP_PLANE = 1, MAP_ROOM = 2, MAP_GAME = 3,
            MAP_WIDTH = 4, MAP_HEIGHT = 5, MAP_DUNGEON = 6, MAP_LINK_FLOOR = 7,
            MAP_BASE_FLOOR = 8, MAP_VISIBLE = 9, MAP_FLOORS = 10, MAP_PIXELS = 11;
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
            if (tab == GUIDE) {
                int game = guideGame();
                if (game != currentGuideGame) {
                    currentGuideGame = game;
                    renderGuide();
                }
            } else if (tab == MAP) {
                int[] changed = worldMap(mapSnapshot == null ? -1 : mapSnapshot[MAP_REVISION]);
                if (changed != null) {
                    mapSnapshot = changed;
                    renderMap();
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
        mapImage = new CompanionMapView(context);
        root.addView(mapImage, new LinearLayout.LayoutParams(-1, 0, 1));
        floorButtons = new LinearLayout(context);
        root.addView(floorButtons);
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
        page.setTextSize(map ? 20 : 28);
        styleTab(guideTab, tab == GUIDE);
        styleTab(inventoryTab, tab == INVENTORY);
        styleTab(mapTab, map);
        scroll.setVisibility(tab == INVENTORY ? View.VISIBLE : View.GONE);
        slots.setVisibility(tab == INVENTORY ? View.VISIBLE : View.GONE);
        mapImage.setVisibility(map ? View.VISIBLE : View.GONE);
        floorButtons.setVisibility(View.GONE);
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
        currentGuideGame = guideGame();
        if (currentGuideGame == 0) {
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
        guide.setGame(currentGuideGame == 1 ? "ages" : "seasons");
    }

    @Override public void dismiss() {
        super.dismiss();
        if (guide != null) {
            guide.close();
            guide = null;
        }
    }

    private void renderMap() {
        int plane = mapSnapshot == null ? -2 : mapSnapshot[MAP_PLANE];
        boolean dungeon = plane == 2;
        floorButtons.removeAllViews();
        floorButtons.setVisibility(View.GONE);
        if (!dungeon) previousDungeon = -1;
        if (plane < 0 || (dungeon && mapSnapshot[MAP_VISIBLE] == 0)) {
            mapImage.setImageDrawable(null);
            mapImage.setContentDescription(null);
            mapImage.resetViewport();
            page.setText(plane < 0 ? "Open a game file to see the map" : "No mapped floors yet");
            return;
        }

        boolean seasons = mapSnapshot[MAP_GAME] != 0;
        int width = mapSnapshot[MAP_WIDTH], height = mapSnapshot[MAP_HEIGHT];
        int room = mapSnapshot[MAP_ROOM], columns = dungeon ? 8 : 16;
        int offset = MAP_PIXELS;
        boolean showLink = true;
        int area = (seasons ? 256 : 0) + (dungeon ? 2 + mapSnapshot[MAP_DUNGEON] : plane);
        if (dungeon) {
            int linkFloor = mapSnapshot[MAP_LINK_FLOOR], visible = mapSnapshot[MAP_VISIBLE];
            if (area != previousDungeon || linkFloor != previousLinkFloor || (visible & (1 << selectedFloor)) == 0) {
                selectedFloor = linkFloor < 8 && (visible & (1 << linkFloor)) != 0
                        ? linkFloor : Integer.numberOfTrailingZeros(visible);
            }
            previousDungeon = area;
            previousLinkFloor = linkFloor;
            int base = mapSnapshot[MAP_BASE_FLOOR];
            for (int i = 0; i < mapSnapshot[MAP_FLOORS]; i++) {
                if ((visible & (1 << i)) == 0) continue;
                final int floor = i;
                Button button = new Button(getContext());
                String label = floorName(base + i);
                button.setText(i == linkFloor ? "◆ " + label : label);
                button.setPadding(0, 0, 0, 0);
                button.setContentDescription(label + (i == linkFloor ? ", Link’s floor" : ""));
                styleTab(button, i == selectedFloor);
                button.setOnClickListener(v -> { selectedFloor = floor; renderMap(); });
                floorButtons.addView(button, new LinearLayout.LayoutParams(0, -2, 1));
            }
            if (floorButtons.getChildCount() > 1) floorButtons.setVisibility(View.VISIBLE);
            offset += width * height * selectedFloor;
            showLink = selectedFloor == linkFloor;
            page.setText(dungeonName(seasons, mapSnapshot[MAP_DUNGEON]) + " · " + floorName(base + selectedFloor));
        } else {
            page.setText(seasons ? (plane == 0 ? "Holodrum" : "Subrosia")
                    : (plane == 0 ? "Labrynna · Present" : "Labrynna · Past"));
        }
        mapImage.setArea(area * 8 + (dungeon ? selectedFloor : 0));
        Bitmap image = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        image.setPixels(mapSnapshot, offset, width, 0, 0, width, height);
        int cx = (room % columns) * 8 + 3, cy = (room / columns) * 8 + 3;
        if (showLink && cx < width && cy < height) {
            for (int y = -3; y <= 3; y++) for (int x = -3; x <= 3; x++) {
                int distance = Math.abs(x) + Math.abs(y);
                if (distance <= 3) image.setPixel(cx+x, cy+y, distance == 3 ? 0xff26332c : 0xffffdf55);
            }
        }
        BitmapDrawable art = new BitmapDrawable(getContext().getResources(), image);
        art.setFilterBitmap(false);
        mapImage.setImageDrawable(art);
        mapImage.setContentDescription(page.getText() + (showLink && cx < width && cy < height
                ? ", Link at column " + (room % columns + 1) + ", row " + (room / columns + 1) : ""));
    }

    private static String floorName(int index) {
        return index < 3 ? "B" + (3 - index) : (index - 2) + "F";
    }

    private static final String[] AGES_DUNGEONS = {"", "Spirit’s Grave", "Wing Dungeon",
            "Moonlit Grotto", "Skull Dungeon", "Crown Dungeon", "Mermaid’s Cave",
            "Jabu-Jabu’s Belly", "Ancient Tomb"};
    private static final String[] SEASONS_DUNGEONS = {"", "Gnarled Root Dungeon", "Snake’s Remains",
            "Poison Moth’s Lair", "Dancing Dragon Dungeon", "Unicorn’s Cave", "Ancient Ruins",
            "Explorer’s Crypt", "Sword & Shield Maze"};

    private static String dungeonName(boolean seasons, int index) {
        if (!seasons && index == 12) return "Mermaid’s Cave · Past";
        if (!seasons && index == 6) return "Mermaid’s Cave · Present";
        if (index < 1 || index > 8) return "Dungeon";
        return (seasons ? SEASONS_DUNGEONS : AGES_DUNGEONS)[index];
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
