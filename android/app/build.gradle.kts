plugins {
    id("com.android.application")
}

// Release signing comes from the environment (CI secrets); without it only debug builds are signed.
val keystore: String? = System.getenv("ORACLES_KEYSTORE")

android {
    namespace = "org.oracles"
    compileSdk = 35
    ndkVersion = "27.2.12479018"

    defaultConfig {
        applicationId = "org.oracles"
        minSdk = 26
        targetSdk = 35
        versionCode = (System.getenv("ORACLES_VERSION_CODE") ?: "1").toInt()
        // the release workflow passes VERSION (1.2.0); a local build has none and never checks for updates
        versionName = (System.getenv("ORACLES_VERSION") ?: "").removePrefix("v").ifEmpty { "dev" }
        ndk {
            abiFilters += "arm64-v8a"
        }
        externalNativeBuild {
            cmake {
                // the engine is too slow unoptimised, so debug APKs build it as Release too
                arguments += "-DCMAKE_BUILD_TYPE=Release"
                // 16 KB pages (Android 15+ phones may use them): Android won't install an app whose
                // native code is aligned for 4 KB only; NDK 28 makes this the default
                arguments += "-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON"
                arguments += "-DORACLES_VERSION=${System.getenv("ORACLES_VERSION") ?: ""}"
                targets += "main"
            }
        }
    }

    signingConfigs {
        if (keystore != null) {
            create("release") {
                storeFile = file(keystore)
                storePassword = System.getenv("ORACLES_KEYSTORE_PASSWORD")
                keyAlias = System.getenv("ORACLES_KEY_ALIAS")
                keyPassword = System.getenv("ORACLES_KEYSTORE_PASSWORD")
            }
        }
    }

    buildTypes {
        // a debug build installs beside the released app (its own package, saves and name), so
        // testing one never means uninstalling the other
        debug {
            applicationIdSuffix = ".test"
        }
        release {
            isMinifyEnabled = false
            if (keystore != null) signingConfig = signingConfigs.getByName("release")
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
            version = "3.31.6"
        }
    }

    lint {
        abortOnError = false
    }
}

dependencies {
    implementation("org.jsoup:jsoup:1.18.3")
}
