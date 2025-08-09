plugins {
    kotlin("jvm") version "2.1.21"
}

group = "org.peyilo"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation(kotlin("test"))

    implementation(libs.jsoup)
    implementation(libs.json.path)
    implementation(libs.jsoupxpath)
    implementation(libs.gson)
    implementation(libs.rhino)

    implementation(libs.kotlinx.coroutines.core)

    //加解密类库,有些书源使用
    //noinspection GradleDependency,GradlePackageUpdate
    implementation(libs.hutool.crypto)
}

tasks.test {
    useJUnitPlatform()
}
kotlin {
    jvmToolchain(17)
}