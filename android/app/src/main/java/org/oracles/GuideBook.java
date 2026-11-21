package org.oracles;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.safety.Safelist;
import org.jsoup.safety.Cleaner;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.security.MessageDigest;
import java.util.LinkedHashSet;
import java.util.Locale;
import java.util.Set;

// Private, offline reader copies. Only a finished book becomes visible to the reader.
final class GuideBook {
    interface Progress { void update(String message); }
    interface Fetch { byte[] get(String url, boolean image) throws IOException; }

    static String index(String game) {
        return "https://www.zeldadungeon.net/oracle-of-" + game + "-walkthrough/";
    }

    static byte[] fetch(String url, boolean image) throws IOException {
        org.jsoup.Connection.Response response = Jsoup.connect(url).timeout(30000)
                .maxBodySize(24 * 1024 * 1024).ignoreContentType(image).ignoreHttpErrors(true).execute();
        if (response.statusCode() == 403)
            throw new IOException("Zelda Dungeon declined this download (403)");
        if (response.statusCode() != 200)
            throw new IOException("Server returned HTTP " + response.statusCode());
        if (response.bodyAsBytes().length >= 24 * 1024 * 1024) throw new IOException("Page or image is too large");
        if (image && (response.contentType() == null || !response.contentType().startsWith("image/")))
            throw new IOException("An illustration could not be downloaded");
        return response.bodyAsBytes();
    }

    static String chapter(String raw, String root) {
        if (raw.isEmpty()) return "";
        try {
            URI u = URI.create(raw);
            String host = u.getHost();
            if (!"https".equals(u.getScheme()) || !("www.zeldadungeon.net".equals(host) ||
                    "zeldadungeon.net".equals(host))) return "";
            String url = "https://www.zeldadungeon.net" + u.normalize().getPath();
            if (!url.endsWith("/")) url += "/";
            if (!url.startsWith(root)) return "";
            String slug = url.substring(root.length());
            return slug.isEmpty() || slug.matches("[a-z0-9-]+/") ? url : "";
        } catch (IllegalArgumentException e) { return ""; }
    }

    static String fileName(String url, String root) {
        return url.equals(root) ? "index.html" :
                url.substring(root.length(), url.length()-1) + ".html";
    }

    private static String digest(String url) {
        try {
            byte[] hash = MessageDigest.getInstance("SHA-256").digest(url.getBytes(StandardCharsets.UTF_8));
            StringBuilder name = new StringBuilder();
            for (byte b : hash) name.append(String.format(Locale.ROOT, "%02x", b & 255));
            return name.toString();
        } catch (java.security.NoSuchAlgorithmException e) { throw new AssertionError(e); }
    }

    private static Element article(byte[] html, String url) throws IOException {
        Document doc = Jsoup.parse(new String(html, StandardCharsets.UTF_8), url);
        Element body = doc.selectFirst(".entry-content, .post-content");
        if (body == null) body = doc.selectFirst("article, main, #content");
        if (body == null) throw new IOException("The site did not return a guide page");
        body.select("script,style,nav,footer,form,iframe,.comments,#comments,.sharedaddy,.advertisement").remove();
        return body;
    }

    static void download(File parent, String game, Fetch fetch, Progress progress) throws IOException {
        File book = new File(parent, game), staging = new File(parent, game + "-partial");
        if (new File(book,"index.html").isFile()) return;
        Files.createDirectories(staging.toPath());
        String root = index(game);
        Element contents = article(fetch.get(root, false), root);
        Set<String> chapters = new LinkedHashSet<>();
        for (Element link : contents.select("a[href]")) {
            String url = chapter(link.absUrl("href"), root);
            if (!url.isEmpty() && !url.equals(root)) chapters.add(url);
        }
        if (chapters.isEmpty()) throw new IOException("The site did not return its chapter list");
        Document toc = Document.createShell(root);
        toc.title("Oracle of " + game + " · Zelda Dungeon");
        toc.body().appendElement("h1").text("Oracle of " + (game.equals("ages") ? "Ages" : "Seasons"));
        toc.body().appendElement("p").text("Walkthrough by Zelda Dungeon");
        Element list = toc.body().appendElement("ol");
        int number = 0;
        for (String url : chapters) {
            progress.update("Saving chapter " + (++number) + " of " + chapters.size());
            File page = new File(staging, fileName(url, root));
            if (!page.isFile()) saveChapter(staging, page, root, url, chapters, fetch);
            String title = Jsoup.parse(page, "UTF-8").title();
            list.appendElement("li").appendElement("a").attr("href", page.getName()).text(title);
        }
        writePage(new File(staging, "index.html"), toc);
        Files.move(staging.toPath(), book.toPath(), StandardCopyOption.ATOMIC_MOVE);
    }

    static void downloadChapter(File parent, String game, String url, Fetch fetch) throws IOException {
        String root = index(game);
        url = chapter(url, root);
        if (url.isEmpty() || url.equals(root)) throw new IOException("Open a chapter first");
        File folder = new File(parent, game + "-partial");
        Files.createDirectories(folder.toPath());
        File page = new File(folder, fileName(url, root));
        if (!page.isFile()) saveChapter(folder, page, root, url, null, fetch);
    }

    private static void saveChapter(File folder, File file, String root, String url,
                                    Set<String> chapters, Fetch fetch) throws IOException {
        Element content = article(fetch.get(url, false), url);
        for (Element link : content.select("a[href]")) {
            String href = link.absUrl("href"), target = chapter(href, root);
            String fragment = href.contains("#") ? href.substring(href.indexOf('#')) : "";
            if (!target.isEmpty() && (chapters == null || root.equals(target) || chapters.contains(target)))
                link.attr("href", fileName(target, root) + fragment);
            else link.removeAttr("href");
        }
        for (Element img : content.select("img")) {
            String src = img.absUrl("data-src");
            if (src.isEmpty()) src = img.absUrl("data-lazy-src");
            if (src.isEmpty()) src = img.absUrl("src");
            if (!src.startsWith("https://")) { img.remove(); continue; }
            String name = "img-" + digest(src);
            File asset = new File(folder, name);
            if (!asset.isFile()) writeAtomic(asset, fetch.get(src, true));
            img.attr("src", name);
        }
        Document shell = Document.createShell("");
        shell.body().appendChild(content);
        Safelist allowed = new Safelist().addTags("p","br","h1","h2","h3","h4","h5","h6","ul","ol",
                "li","div","span","strong","b","i","em","a","img","table","thead","tbody","tr","td","th",
                "blockquote","figure","figcaption","hr").addAttributes(":all","id")
                .addAttributes("a","href","name").addAttributes("img","src","alt");
        Document page = new Cleaner(allowed).clean(shell);
        String slug = file.getName().replace(".html","").replace('-',' ');
        page.title(Character.toUpperCase(slug.charAt(0)) + slug.substring(1));
        page.body().prependElement("a").attr("href","index.html").text("All chapters · Zelda Dungeon");
        writePage(file, page);
    }

    private static void writePage(File file, Document doc) throws IOException {
        doc.head().appendElement("meta").attr("name","viewport").attr("content","width=device-width,initial-scale=1");
        doc.head().appendElement("style").text("body{font:18px/1.5 sans-serif;margin:1em;color:#263a30;background:#fff3d2}img,table{max-width:100%;height:auto}a{color:#185b48}li{padding:.35em 0}");
        writeAtomic(file, doc.outerHtml().getBytes(StandardCharsets.UTF_8));
    }

    private static void writeAtomic(File file, byte[] bytes) throws IOException {
        File temp = new File(file.getParentFile(), file.getName()+".tmp");
        Files.write(temp.toPath(),bytes);
        Files.move(temp.toPath(),file.toPath(),StandardCopyOption.ATOMIC_MOVE, StandardCopyOption.REPLACE_EXISTING);
    }
}
