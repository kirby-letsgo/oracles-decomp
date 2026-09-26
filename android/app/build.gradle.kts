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
        versionName = System.getenv("ORACLES_VERSION_NAME") ?: "0.5"
        ndk {
            abiFilters += "arm64-v8a"
        }
        externalNativeBuild {
            cmake {
                // the engine is too slow unoptimised, so debug APKs build it as Release too
                arguments += "-DCMAKE_BUILD_TYPE=Release"
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
