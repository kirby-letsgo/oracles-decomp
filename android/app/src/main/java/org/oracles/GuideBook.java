package org.oracles;

import android.content.Context;
import android.net.Uri;
import android.os.Handler;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import org.json.JSONArray;
import org.json.JSONException;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.LinkedHashMap;
import java.util.Map;

// Save the rendered walkthrough with Android's web archive writer, including loaded images.
final class GuideBook {
    interface Listener {
        void progress(String message);
        void finished(String message);
    }

    static String index(String game) {
        return "https://www.zeldadungeon.net/oracle-of-" + game + "-walkthrough/";
    }

    static String chapter(String raw, String root) {
        if (raw == null || raw.isEmpty()) return "";
        try {
            URI uri = URI.create(raw);
            if (!"https".equals(uri.getScheme()) || !("www.zeldadungeon.net".equals(uri.getHost()) ||
                    "zeldadungeon.net".equals(uri.getHost()))) return "";
            String url = "https://www.zeldadungeon.net" + uri.normalize().getPath();
            if (!url.endsWith("/")) url += "/";
            if (!url.startsWith(root)) return "";
            String slug = url.substring(root.length());
            return slug.isEmpty() || slug.matches("[a-z0-9-]+/") ? url : "";
        } catch (IllegalArgumentException e) { return ""; }
    }

    static String fileName(String url, String root) {
        return url.equals(root) ? "index.html" : url.substring(root.length(), url.length()-1) + ".mht";
    }

    static File existing(File folder, String name) {
        File archive = new File(folder, name);
        if (archive.isFile() || !name.endsWith(".mht")) return archive;
        File legacy = new File(folder, name.substring(0, name.length()-4) + ".html");
        return legacy.isFile() ? legacy : archive;
    }

    private final File partial, complete;
    private final String root;
    private final WebView browser;
    private final Handler handler = new Handler(android.os.Looper.getMainLooper());
    private final Listener listener;
    private final boolean whole;
    private final LinkedHashMap<String,String> chapters = new LinkedHashMap<>();
    private String pending;
    private File temp;
    private boolean stopped, inspecting, archiving;
    private final Runnable timeout = () -> finish("Download stopped · Page could not be saved. Tap Save to retry.");

    GuideBook(Context context, File library, String game, boolean whole, Listener listener) {
        this.root = index(game);
        this.whole = whole;
        this.listener = listener;
        partial = new File(library, game + "-partial");
        complete = new File(library, game);
        browser = new WebView(context);
        browser.getSettings().setAllowContentAccess(false);
        browser.getSettings().setAllowFileAccess(false);
        browser.getSettings().setDomStorageEnabled(true);
        browser.getSettings().setJavaScriptEnabled(true);
        browser.setWebViewClient(new WebViewClient() {
            @Override public void onPageFinished(WebView view, String url) {
                if (!stopped && !inspecting && !archiving && pending.equals(chapter(url, root))) inspect();
            }
            @Override public boolean shouldOverrideUrlLoading(WebView view, android.webkit.WebResourceRequest request) {
                return request.isForMainFrame() && chapter(request.getUrl().toString(), root).isEmpty();
            }
        });
    }

    void start(String url) {
        if (!partial.isDirectory() && !partial.mkdirs()) { finish("Could not create the walkthrough folder"); return; }
        if (!whole) chapters.put(url, "");
        load(whole ? root : url);
    }

    private void load(String url) {
        pending = url;
        inspecting = archiving = false;
        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(timeout, 60000);
        browser.loadUrl(url);
    }

    private void inspect() {
        inspecting = true;
        String expected = pending;
        String script = "(function(){let a=document.querySelector('.page-content,.entry-content,.post-content')||document.querySelector('article,main,#content');"
                + "if(!a||!a.querySelector('p')||a.innerText.length<200)return null;"
                + "if(location.pathname.replace(/\\/$/,'')===" + org.json.JSONObject.quote(Uri.parse(root).getPath().replaceAll("/$", "")) + ")"
                + "return Array.from(a.querySelectorAll('a[href]'),l=>[l.href,l.textContent.trim()]);"
                + "if(!a.dataset.offlineReader){a.dataset.offlineReader='1';"
                + "let h=document.createElement('h1');h.textContent=document.querySelector('h1')?.textContent||document.title;"
                + "a.prepend(h);document.body.replaceChildren(a);"
                + "a.querySelectorAll('iframe,script,form,video,ins').forEach(e=>e.remove());"
                + "document.querySelectorAll('link[rel=stylesheet],style').forEach(e=>e.remove());"
                + "let css=document.createElement('style');css.textContent='body{font:18px/1.5 sans-serif;color:#263a30;background:#fff3d2;margin:1em}img,table{max-width:100%;height:auto}a{color:#185b48}h1{font-size:1.45em}';document.head.append(css);}"
                + "let imgs=Array.from(a.querySelectorAll('img'));for(let i of imgs){i.loading='eager';"
                + "let src=i.dataset.lazySrc||i.dataset.src;if(src&&i.getAttribute('src')!==src)i.src=src;if(i.src.startsWith('http://'))i.src='https://'+i.src.slice(7);}"
                + "return imgs.some(i=>!i.complete)?null:[];})()";
        browser.evaluateJavascript(script, result -> {
            if (stopped || !expected.equals(pending) || archiving) return;
            if ("null".equals(result) || result == null) {
                handler.postDelayed(() -> { if (!stopped && expected.equals(pending)) inspect(); }, 600);
                return;
            }
            try {
                if (expected.equals(root)) {
                    JSONArray links = new JSONArray(result);
                    for (int i=0; i<links.length(); i++) {
                        JSONArray link = links.getJSONArray(i);
                        String target = chapter(link.getString(0), root);
                        if (!target.isEmpty() && !target.equals(root) && !link.getString(1).isEmpty())
                            chapters.putIfAbsent(target, link.getString(1));
                    }
                    if (chapters.isEmpty()) { finish("The chapter list could not be read"); return; }
                    next();
                } else archive(expected);
            } catch (JSONException | IOException e) { finish("Download stopped · " + e.getMessage()); }
        });
    }

    private void archive(String url) throws IOException {
        archiving = true;
        temp = File.createTempFile("chapter-", ".tmp", partial);
        File output = temp;
        browser.saveWebArchive(output.getAbsolutePath(), false, path -> {
            if (stopped) { output.delete(); return; }
            try {
                if (path == null || output.length() == 0) throw new IOException("Android could not save this chapter");
                Files.move(output.toPath(), new File(partial, fileName(url, root)).toPath(), StandardCopyOption.ATOMIC_MOVE);
                temp = null;
                if (whole) next(); else finish("Chapter saved for offline reading");
            } catch (IOException e) { finish("Download stopped · " + e.getMessage()); }
        });
    }

    private void next() throws IOException {
        int number = 0;
        Document toc = Document.createShell(root);
        toc.head().appendElement("meta").attr("name","viewport").attr("content","width=device-width,initial-scale=1");
        toc.head().appendElement("style").text("body{font:18px/1.5 sans-serif;background:#fff3d2;color:#263a30}li{padding:.4em}a{color:#185b48}");
        toc.body().appendElement("h2").text("Zelda Dungeon walkthrough");
        Element list = toc.body().appendElement("ol");
        for (Map.Entry<String,String> entry : chapters.entrySet()) {
            number++;
            File page = existing(partial, fileName(entry.getKey(), root));
            if (!page.isFile()) {
                listener.progress("Saving chapter " + number + " of " + chapters.size());
                load(entry.getKey());
                return;
            }
            list.appendElement("li").appendElement("a").attr("href",page.getName()).text(entry.getValue());
        }
        Files.write(new File(partial,"index.html").toPath(), toc.outerHtml().getBytes(StandardCharsets.UTF_8));
        Files.move(partial.toPath(), complete.toPath(), StandardCopyOption.ATOMIC_MOVE);
        finish("Walkthrough saved for offline reading");
    }

    private void finish(String message) {
        if (stopped) return;
        close();
        listener.finished(message);
    }

    void close() {
        stopped = true;
        handler.removeCallbacksAndMessages(null);
        browser.stopLoading();
        browser.destroy();
        if (temp != null) temp.delete();
    }
}
