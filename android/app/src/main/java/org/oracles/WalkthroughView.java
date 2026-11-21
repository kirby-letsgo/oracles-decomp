package org.oracles;

import android.content.Context;
import android.net.Uri;
import android.webkit.WebResourceRequest;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

final class WalkthroughView extends LinearLayout {
    private static final ExecutorService downloads = Executors.newSingleThreadExecutor();
    private final File library;
    private final WebView reader;
    private final TextView status;
    private final Button chapter, book, contents;
    private String game, current;
    private boolean busy;

    WalkthroughView(Context context) {
        super(context);
        setOrientation(VERTICAL);
        library = new File(context.getFilesDir(), "walkthroughs");
        LinearLayout row = new LinearLayout(context);
        addView(row);
        contents = button(row, "Contents", this::openContents);
        chapter = button(row, "Save chapter", () -> save(false));
        book = button(row, "Save walkthrough", () -> save(true));
        status = new TextView(context);
        addView(status);
        reader = new WebView(context);
        reader.getSettings().setAllowContentAccess(false);
        reader.getSettings().setAllowFileAccess(true);
        reader.getSettings().setBuiltInZoomControls(true);
        reader.getSettings().setDisplayZoomControls(false);
        reader.setWebViewClient(new WebViewClient() {
            @Override public boolean shouldOverrideUrlLoading(WebView v, WebResourceRequest req) {
                if (!req.isForMainFrame()) return false;
                Uri uri = req.getUrl();
                if (game == null) return true;
                if ("file".equals(uri.getScheme())) {
                    String path = uri.getPath();
                    if (path == null) return true;
                    File page = new File(path);
                    if (!folder().equals(page.getParentFile()) && !partial().equals(page.getParentFile())) return true;
                    String filename = page.getName();
                    if (!filename.matches("[a-z0-9-]+\\.html")) return true;
                    if (page.exists()) return false;
                    openUrl(root() + (filename.equals("index.html") ? "" : filename.replace(".html","")+"/"));
                    return true;
                }
                String target = GuideBook.chapter(uri.toString(), root());
                if (target.isEmpty()) return true;
                File cached = saved(target);
                if (!cached.exists()) return false;
                Uri local = Uri.fromFile(cached).buildUpon().fragment(uri.getFragment()).build();
                reader.loadUrl(local.toString());
                return true;
            }
            @Override public void onPageFinished(WebView view, String url) {
                current = url;
                updateButtons();
            }
        });
        addView(reader, new LayoutParams(-1, 0, 1));
    }

    private Button button(LinearLayout row, String label, Runnable action) {
        Button b = new Button(getContext());
        b.setAllCaps(false);
        b.setText(label);
        b.setOnClickListener(v -> action.run());
        row.addView(b, new LayoutParams(0,-2,1));
        return b;
    }

    void setGame(String next) {
        if (next.equals(game)) return;
        game = next;
        current = null;
        status.setText("Zelda Dungeon");
        openContents();
    }

    private String root() { return GuideBook.index(game); }
    private File folder() { return new File(library, game); }
    private File partial() { return new File(library, game+"-partial"); }
    private File saved(String url) {
        String name = GuideBook.fileName(url,root());
        File complete = new File(folder(),name);
        return complete.exists() ? complete : new File(partial(),name);
    }

    private void openUrl(String url) {
        File file = saved(url);
        reader.loadUrl(file.isFile() ? Uri.fromFile(file).toString() : url);
        updateButtons();
    }

    private void openContents() {
        File[] pages = partial().listFiles((dir,name) -> name.endsWith(".html") && !name.equals("index.html"));
        if (new File(folder(),"index.html").exists() || pages == null || pages.length == 0) {
            openUrl(root());
            return;
        }
        android.widget.PopupMenu menu = new android.widget.PopupMenu(getContext(), contents);
        menu.getMenu().add("Online contents").setOnMenuItemClickListener(item -> { openUrl(root()); return true; });
        for (File page : pages) {
            menu.getMenu().add(page.getName().replace(".html","").replace('-',' '))
                    .setOnMenuItemClickListener(item -> { reader.loadUrl(Uri.fromFile(page).toString()); return true; });
        }
        menu.show();
    }

    private void updateButtons() {
        if (game == null) return;
        String url = current == null ? "" : GuideBook.chapter(current,root());
        chapter.setEnabled(!busy && !url.isEmpty() && !url.equals(root()) && !saved(url).exists());
        boolean complete = new File(folder(),"index.html").isFile();
        book.setText(complete ? "Saved offline" : "Save walkthrough");
        book.setEnabled(!busy && !complete);
    }

    private void save(boolean whole) {
        if (busy || game == null) return;
        String target = game, url = current;
        busy = true;
        updateButtons();
        status.setText(whole ? "Finding chapters…" : "Saving chapter…");
        downloads.execute(() -> {
            String message;
            try {
                if (whole) GuideBook.download(library,target,GuideBook::fetch,
                        text -> post(() -> { if (target.equals(game)) status.setText(text); }));
                else GuideBook.downloadChapter(library,target,url,GuideBook::fetch);
                message = whole ? "Walkthrough saved for offline reading" : "Chapter saved for offline reading";
            } catch (Exception e) { message = "Download stopped · " + e.getMessage(); }
            String result = message;
            post(() -> {
                busy = false;
                if (target.equals(game)) {
                    status.setText(result);
                    if (new File(folder(),"index.html").isFile()) openUrl(root());
                }
                updateButtons();
            });
        });
    }

    void close() {
        reader.stopLoading();
        removeView(reader);
        reader.destroy();
    }
}
