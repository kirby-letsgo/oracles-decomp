plugins {
    id("com.android.application")
}

android {
    namespace = "org.oracles"
    compileSdk = 35
    ndkVersion = "27.2.12479018"

    defaultConfig {
        applicationId = "org.oracles"
        minSdk = 26
        targetSdk = 35
        versionCode = 1
        versionName = "0.5"
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
