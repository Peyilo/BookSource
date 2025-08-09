package org.peyilo.booksource.constant

import cn.hutool.crypto.digest.DigestUtil
import org.peyilo.booksource.BuildConfig
import org.peyilo.booksource.help.update.AppVariant
import org.apache.commons.lang3.time.FastDateFormat

@Suppress("ConstPropertyName")
object AppConst {

    const val APP_TAG = "Legado"

    const val channelIdDownload = "channel_download"
    const val channelIdReadAloud = "channel_read_aloud"
    const val channelIdWeb = "channel_web"

    const val UA_NAME = "User-Agent"

    const val MAX_THREAD = 9

    const val DEFAULT_WEBDAV_ID = -1L

    private const val OFFICIAL_SIGNATURE =
        "8DACBF25EC667C9B1374DB1450C1A866C2AAA1173016E80BF6AD2F06FABDDC08"
    private const val BETA_SIGNATURE =
        "93A28468B0F69E8D14C8A99AB45841CEF902BBBA3761BBFEE02E67CBA801563E"

    val timeFormat: FastDateFormat by lazy {
        FastDateFormat.getInstance("HH:mm")
    }

    val dateFormat: FastDateFormat by lazy {
        FastDateFormat.getInstance("yyyy/MM/dd HH:mm")
    }

    val fileNameFormat: FastDateFormat by lazy {
        FastDateFormat.getInstance("yy-MM-dd-HH-mm-ss")
    }

    const val imagePathKey = "imagePath"

    val menuViewNames = arrayOf(
        "com.android.internal.view.menu.ListMenuItemView",
        "androidx.appcompat.view.menu.ListMenuItemView"
    )

    // ===== sysElevation：Android 原来从资源读，这里给一个通用的默认值（像素） =====
    // 如需自定义：传入 -Dapp.sysElevation=8 覆盖
    val sysElevation: Int = (System.getProperty("app.sysElevation")?.toIntOrNull() ?: 4)

    // ===== androidId：非 Android 环境下生成一个稳定的“设备ID”替代品 =====
    // 优先取 -Dapp.androidId=...；否则生成并缓存到用户目录的一个小文件中
    val androidId: String by lazy {
        val prop = System.getProperty("app.androidId")
        if (!prop.isNullOrBlank()) return@lazy prop

        val home = System.getProperty("user.home") ?: "."
        val idFile = java.io.File(home, ".app_android_id")
        if (idFile.exists()) {
            runCatching { idFile.readText().trim() }.getOrElse { "null" }.ifBlank { "null" }
        } else {
            val gen = java.util.UUID.randomUUID().toString()
            runCatching { idFile.writeText(gen) }
            gen
        }
    }

    // ===== AppInfo：从包元数据或系统属性推断版本信息（脱离 PackageManager） =====
    val appInfo: AppInfo by lazy {
        val info = AppInfo()
        val pkg = AppConst::class.java.`package`

        // versionName：优先 -Dapp.versionName，其次读取 JAR 的 Implementation-Version，最后 "dev"
        info.versionName = System.getProperty("app.versionName")
            ?: pkg?.implementationVersion
                    ?: "dev"

        // versionCode：优先 -Dapp.versionCode，否则根据 versionName 简单提取（如 1.2.3 -> 10203），失败则 0
        info.versionCode = System.getProperty("app.versionCode")?.toLongOrNull()
            ?: parseVersionCode(info.versionName)

        // 变体判断：沿用你原逻辑关键点
        info.appVariant = when {
            info.versionName.contains("releaseA", ignoreCase = true) -> AppVariant.BETA_RELEASEA
            isBeta -> AppVariant.BETA_RELEASE
            isOfficial -> AppVariant.OFFICIAL
            else -> AppVariant.UNKNOWN
        }
        info
    }

    // ===== 辅助：版本号转长整型，适配常见语义化版本 =====
    private fun parseVersionCode(versionName: String): Long {
        // 1.2.3 -> 1*10000 + 2*100 + 3 = 10203
        val parts = versionName.split('.', '-', '_')
            .take(3)
            .mapNotNull { it.toIntOrNull() }
        if (parts.isEmpty()) return 0
        val major = parts.getOrElse(0) { 0 }
        val minor = parts.getOrElse(1) { 0 }
        val patch = parts.getOrElse(2) { 0 }
        return (major * 10000L + minor * 100L + patch)
    }

    // ===== sha256Signature：无法用 PackageManager，改为从系统属性传入或资源计算 =====
    // 1) 可通过 -Dapp.signature=HEX 直接指定
    // 2) 或者对运行 JAR 本体计算 SHA-256（可选，视需要保留）
    private val sha256Signature: String by lazy {
        val prop = System.getProperty("app.signature")?.trim()?.uppercase()
        if (!prop.isNullOrBlank()) return@lazy prop

        // 可选：对当前代码来源 (JAR) 计算 SHA-256，失败则空串
        runCatching {
            val codeSource = AppConst::class.java.protectionDomain.codeSource
            val url = codeSource?.location ?: return@runCatching ""
            val path = java.nio.file.Paths.get(url.toURI())
            val bytes = java.nio.file.Files.readAllBytes(path)
            sha256Hex(bytes).uppercase()
        }.getOrDefault("")
    }

    // ===== 辅助：SHA-256 计算（替代 DigestUtil.sha256Hex） =====
    private fun sha256Hex(bytes: ByteArray): String {
        val md = java.security.MessageDigest.getInstance("SHA-256")
        val digest = md.digest(bytes)
        val sb = StringBuilder(digest.size * 2)
        for (b in digest) {
            val v = b.toInt() and 0xff
            if (v < 16) sb.append('0')
            sb.append(v.toString(16))
        }
        return sb.toString()
    }

    private val isOfficial = sha256Signature == OFFICIAL_SIGNATURE

    private val isBeta = sha256Signature == BETA_SIGNATURE || BuildConfig.DEBUG

    val charsets =
        arrayListOf("UTF-8", "GB2312", "GB18030", "GBK", "Unicode", "UTF-16", "UTF-16LE", "ASCII")

    data class AppInfo(
        var versionCode: Long = 0L,
        var versionName: String = "",
        var appVariant: AppVariant = AppVariant.UNKNOWN
    )

    /**
     * FileProvider authority：Android 下是 APPLICATION_ID + ".fileProvider"
     * 脱离 Android 后给一个固定默认值；如需定制，可用 -Dapp.authority 覆盖。
     */
    const val authority: String = "app.fileProvider"

}
