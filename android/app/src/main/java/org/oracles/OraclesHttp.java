package org.oracles;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.List;
import java.util.Map;

// The sync client's HTTP on Android (called from C through JNI, see src/sync/http_android.c).
public final class OraclesHttp {
    private OraclesHttp() {}

    // Returns {Integer status, byte[] body, String headers ("name: value\n"), String error}.
    public static Object[] request(String method, String url, String headers, byte[] body) {
        HttpURLConnection c = null;
        try {
            c = (HttpURLConnection) new URL(url).openConnection();
            c.setRequestMethod(method);
            c.setConnectTimeout(10000);
            c.setReadTimeout(30000);
            for (String line : headers.split("\n")) {
                int colon = line.indexOf(':');
                if (colon > 0) c.setRequestProperty(line.substring(0, colon).trim(), line.substring(colon + 1).trim());
            }
            if (body != null) {
                c.setDoOutput(true);
                c.setFixedLengthStreamingMode(body.length);
                try (OutputStream out = c.getOutputStream()) {
                    out.write(body);
                }
            }
            int status = c.getResponseCode();
            StringBuilder h = new StringBuilder();
            for (Map.Entry<String, List<String>> e : c.getHeaderFields().entrySet()) {
                if (e.getKey() == null) continue;
                for (String v : e.getValue()) h.append(e.getKey()).append(": ").append(v).append('\n');
            }
            InputStream in = status >= 400 ? c.getErrorStream() : c.getInputStream();
            ByteArrayOutputStream data = new ByteArrayOutputStream();
            if (in != null) {
                byte[] buf = new byte[16384];
                for (int n; (n = in.read(buf)) > 0; ) data.write(buf, 0, n);
                in.close();
            }
            return new Object[] {status, data.toByteArray(), h.toString(), ""};
        } catch (Exception e) {
            return new Object[] {0, null, "", String.valueOf(e.getMessage())};
        } finally {
            if (c != null) c.disconnect();
        }
    }
}
