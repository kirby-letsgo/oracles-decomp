package org.oracles;

import android.content.Context;
import android.content.SharedPreferences;
import android.net.Uri;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceError;
import android.webkit.WebResourceResponse;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;


final class WalkthroughView extends LinearLayout {
    private GuideBook download;
    private final File library;
    private final WebView reader;
    private final TextView status;
    private final Button chapter, book, contents;
    private final SharedPreferences bookmarks;
    private String game, current;
    private String resumeChapter;
    private int resumeOffset;
    private boolean ready;
    private boolean busy, closed;
    private String failedPage;

    WalkthroughView(Context context) {
        super(context);
        setOrientation(VERTICAL);
        bookmarks = context.getSharedPreferences("guide-bookmarks", Context.MODE_PRIVATE);
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
        reader.getSettings().setAllowFileAccess(false);
        reader.getSettings().setDomStorageEnabled(true);
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
                    if (!filename.matches("[a-z0-9-]+\\.(html|mht)")) return true;
                    if (page.exists()) { loadPage(uri.toString()); return true; }
                    openUrl(root() + (filename.equals("index.html") ? "" : filename.substring(0, filename.lastIndexOf('.'))+"/"));
                    return true;
                }
                String target = GuideBook.chapter(uri.toString(), root());
                if (target.isEmpty()) return true;
                File cached = saved(target);
                if (!cached.exists()) {
                    if (reader.getSettings().getAllowFileAccess()) { loadPage(uri.toString()); return true; }
                    return false;
                }
                Uri local = Uri.fromFile(cached).buildUpon().fragment(uri.getFragment()).build();
                loadPage(local.toString());
                return true;
            }
            @Override public void onPageStarted(WebView view, String url, android.graphics.Bitmap icon) {
                if (closed) return;
                if (failedPage != null && !busy) status.setText("Zelda Dungeon");
                failedPage = null;
                current = null;
                ready = false;
                updateButtons();
            }
            @Override public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                if (request.isForMainFrame()) pageFailed(request);
            }
            @Override public void onReceivedHttpError(WebView view, WebResourceRequest request, WebResourceResponse response) {
                if (request.isForMainFrame() && response.getStatusCode() >= 400) pageFailed(request);
            }
            @Override public void onPageFinished(WebView view, String url) {
                if (closed || !url.equals(view.getUrl()) || url.equals(failedPage) || game == null) return;
                current = url;
                String chapterUrl = canonicalPage(url);
                if (!chapterUrl.isEmpty()) {
                    int offset = chapterUrl.equals(resumeChapter) ? resumeOffset : 0;
                    resumeChapter = null;
                    view.postVisualStateCallback(0, new WebView.VisualStateCallback() {
                        @Override public void onComplete(long requestId) {
                            if (!url.equals(current)) return;
                            if (offset > 0) view.scrollTo(0, offset);
                            ready = true;
                            rememberPlace();
                        }
                    });
                }
                updateButtons();
            }
        });
        reader.setOnScrollChangeListener((v, x, y, oldX, oldY) -> {
            reader.removeCallbacks(storePosition);
            reader.postDelayed(storePosition, 350);
        });
        addView(reader, new LayoutParams(-1, 0, 1));
    }

    private void pageFailed(WebResourceRequest request) {
        if (closed || game == null) return;
        String url = request.getUrl().toString();
        if (GuideBook.chapter(url, root()).isEmpty()) return;
        failedPage = url;
        current = null;
        ready = false;
        File[] pages = savedChapters();
        status.setText(pages != null && pages.length > 0
                ? "Page unavailable · Open Contents for saved chapters"
                : "Page unavailable · Tap Contents to retry");
        updateButtons();
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
        rememberPlace();
        ready = false;
        game = next;
        current = null;
        status.setText("Zelda Dungeon");
        resumeChapter = GuideBook.chapter(bookmarks.getString(game + "-page", ""), root());
        resumeOffset = bookmarks.getInt(game + "-offset", 0);
        openUrl(resumeChapter.isEmpty() ? root() : resumeChapter);
    }

    private final Runnable storePosition = this::rememberPlace;

    private String canonicalPage(String url) {
        Uri uri = Uri.parse(url);
        if (!"file".equals(uri.getScheme())) return GuideBook.chapter(url, root());
        if (uri.getPath() == null) return "";
        File file = new File(uri.getPath());
        if (!folder().equals(file.getParentFile()) && !partial().equals(file.getParentFile())) return "";
        String name = file.getName();
        if (!name.matches("[a-z0-9-]+\\.(html|mht)")) return "";
        return name.equals("index.html") ? root() : root() + name.substring(0, name.lastIndexOf('.')) + "/";
    }

    private void rememberPlace() {
        if (!ready || current == null || game == null) return;
        String url = canonicalPage(current);
        if (!url.isEmpty()) bookmarks.edit().putString(game + "-page", url)
                .putInt(game + "-offset", reader.getScrollY()).apply();
    }

    private String root() { return GuideBook.index(game); }
    private File folder() { return new File(library, game); }
    private File partial() { return new File(library, game+"-partial"); }
    private File saved(String url) {
        String name = GuideBook.fileName(url,root());
        File complete = GuideBook.existing(folder(), name);
        return complete.exists() ? complete : GuideBook.existing(partial(), name);
    }

    private void loadPage(String url) {
        boolean local = "file".equals(Uri.parse(url).getScheme());
        reader.getSettings().setBlockNetworkLoads(local);
        reader.getSettings().setJavaScriptEnabled(!local);
        reader.getSettings().setAllowFileAccess(local);
        reader.loadUrl(url);
    }

    private void openUrl(String url) {
        File file = saved(url);
        loadPage(file.isFile() ? Uri.fromFile(file).toString() : url);
        updateButtons();
    }

    private File[] savedChapters() {
        return partial().listFiles((dir,name) -> (name.endsWith(".html") || name.endsWith(".mht")) && !name.equals("index.html"));
    }

    private void openContents() {
        File[] pages = savedChapters();
        if (new File(folder(),"index.html").exists() || pages == null || pages.length == 0) {
            openUrl(root());
            return;
        }
        java.util.Arrays.sort(pages);
        android.widget.PopupMenu menu = new android.widget.PopupMenu(getContext(), contents);
        menu.getMenu().add("Online contents").setOnMenuItemClickListener(item -> { openUrl(root()); return true; });
        for (File page : pages) {
            menu.getMenu().add(page.getName().replace(".html","").replace(".mht","").replace('-',' '))
                    .setOnMenuItemClickListener(item -> { loadPage(Uri.fromFile(page).toString()); return true; });
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
        download = new GuideBook(getContext(), library, target, whole, new GuideBook.Listener() {
            @Override public void progress(String message) {
                if (target.equals(game)) status.setText(message);
            }
            @Override public void finished(String message) {
                download = null;
                busy = false;
                if (target.equals(game)) {
                    status.setText(message);
                    String place = current == null ? "" : canonicalPage(current);
                    if (!place.isEmpty() && saved(place).isFile()) {
                        resumeChapter = place;
                        resumeOffset = reader.getScrollY();
                        openUrl(place);
                    }
                }
                updateButtons();
            }
        });
        download.start(whole ? root() : GuideBook.chapter(url, root()));
    }

    void close() {
        rememberPlace();
        closed = true;
        if (download != null) download.close();
        ready = false;
        current = null;
        reader.removeCallbacks(storePosition);
        reader.stopLoading();
        removeView(reader);
        reader.destroy();
    }
}
