package org.oracles;

import android.app.Presentation;
import android.content.Context;
import android.graphics.Typeface;
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
        root.addView(page, new LinearLayout.LayoutParams(-1, 0, 1));
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
        page.setText(map ? "Map" : "Inventory");
    }

    private static void styleTab(Button button, boolean selected) {
        button.setSelected(selected);
        button.setBackgroundColor(selected ? 0xffefd58c : 0xff364e42);
        button.setTextColor(selected ? 0xff25382d : 0xffe4dcba);
    }
}
