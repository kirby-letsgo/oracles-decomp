// Android: org.oracles.OraclesHttp (HttpURLConnection) through JNI. The sync thread is an SDL
// thread, so SDL has attached it to the VM; the helper class comes from the activity's class
// loader because FindClass on a native thread only sees the system classes.
#include "sync/http.h"
#include <SDL3/SDL.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void http_add_header(HttpResponse *res, const char *line, size_t len);

bool http_available(void) { return true; }

static jclass helper_class(JNIEnv *env) {
  jobject activity = (jobject)SDL_GetAndroidActivity();
  if (!activity) return NULL;
  jclass activity_class = (*env)->GetObjectClass(env, activity);
  jmethodID get_loader = (*env)->GetMethodID(env, activity_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
  jobject loader = (*env)->CallObjectMethod(env, activity, get_loader);
  jclass loader_class = (*env)->FindClass(env, "java/lang/ClassLoader");
  jmethodID load_class = (*env)->GetMethodID(env, loader_class, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
  jstring name = (*env)->NewStringUTF(env, "org.oracles.OraclesHttp");
  jclass cls = (jclass)(*env)->CallObjectMethod(env, loader, load_class, name);
  (*env)->DeleteLocalRef(env, name);
  (*env)->DeleteLocalRef(env, loader_class);
  (*env)->DeleteLocalRef(env, loader);
  (*env)->DeleteLocalRef(env, activity_class);
  (*env)->DeleteLocalRef(env, activity);
  if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); return NULL; }
  return cls;
}

bool http_request(const char *method, const char *url, const char *headers, const uint8_t *body, size_t size, HttpResponse *res) {
  memset(res, 0, sizeof *res);
  JNIEnv *env = (JNIEnv *)SDL_GetAndroidJNIEnv();
  if (!env) { snprintf(res->error, sizeof res->error, "no JNI environment"); return false; }
  if ((*env)->PushLocalFrame(env, 16) != 0) return false;
  bool ok = false;
  jclass cls = helper_class(env);
  jmethodID request = cls ? (*env)->GetStaticMethodID(env, cls, "request", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[B)[Ljava/lang/Object;") : NULL;
  if (!request) { snprintf(res->error, sizeof res->error, "OraclesHttp not found"); goto done; }
  jbyteArray jbody = NULL;
  if (body) {
    jbody = (*env)->NewByteArray(env, (jsize)size);
    (*env)->SetByteArrayRegion(env, jbody, 0, (jsize)size, (const jbyte *)body);
  }
  jobjectArray out = (jobjectArray)(*env)->CallStaticObjectMethod(env, cls, request, (*env)->NewStringUTF(env, method),
                                                                  (*env)->NewStringUTF(env, url), (*env)->NewStringUTF(env, headers ? headers : ""), jbody);
  if ((*env)->ExceptionCheck(env) || !out) { (*env)->ExceptionClear(env); snprintf(res->error, sizeof res->error, "request threw"); goto done; }
  jobject status = (*env)->GetObjectArrayElement(env, out, 0);
  jclass integer = (*env)->FindClass(env, "java/lang/Integer");
  res->status = (*env)->CallIntMethod(env, status, (*env)->GetMethodID(env, integer, "intValue", "()I"));
  jbyteArray data = (jbyteArray)(*env)->GetObjectArrayElement(env, out, 1);
  if (data) {
    jsize n = (*env)->GetArrayLength(env, data);
    res->body = malloc((size_t)n + 1);
    if (res->body) {
      (*env)->GetByteArrayRegion(env, data, 0, n, (jbyte *)res->body);
      res->size = (size_t)n;
      res->body[n] = 0;
    }
  }
  jstring jheaders = (jstring)(*env)->GetObjectArrayElement(env, out, 2);
  const char *text = (*env)->GetStringUTFChars(env, jheaders, NULL);
  for (const char *p = text; p && *p;) {
    const char *eol = strchr(p, '\n');
    size_t len = eol ? (size_t)(eol - p) : strlen(p);
    http_add_header(res, p, len);
    p += len + (eol ? 1 : 0);
  }
  if (text) (*env)->ReleaseStringUTFChars(env, jheaders, text);
  jstring err = (jstring)(*env)->GetObjectArrayElement(env, out, 3);
  const char *e = (*env)->GetStringUTFChars(env, err, NULL);
  snprintf(res->error, sizeof res->error, "%s", e ? e : "");
  if (e) (*env)->ReleaseStringUTFChars(env, err, e);
  ok = res->status != 0;
done:
  (*env)->PopLocalFrame(env, NULL);
  return ok;
}
