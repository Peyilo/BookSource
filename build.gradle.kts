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

    implementation(libs.kotlinx.coroutines.core)

    //加解密类库,有些书源使用
    //noinspection GradleDependency,GradlePackageUpdate
    implementation(libs.hutool.crypto)

    implementation(libs.mozilla.rhino)
    implementation("com.squareup.okio:okio:3.9.0")
    implementation("org.apache.commons:commons-text:1.10.0")
    implementation("org.json:json:20240303")
    implementation("com.squareup.okhttp3:okhttp:4.12.0")
    implementation("xmlpull:xmlpull:1.1.3.1")

}

tasks.test {
    useJUnitPlatform()
}
kotlin {
    jvmToolchain(11)
}