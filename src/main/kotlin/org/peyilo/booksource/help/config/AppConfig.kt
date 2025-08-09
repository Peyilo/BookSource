package org.peyilo.booksource.help.config

import org.peyilo.booksource.constant.AppConst
import java.util.concurrent.ConcurrentHashMap
import kotlin.properties.ReadWriteProperty
import kotlin.reflect.KProperty

@Suppress("MemberVisibilityCanBePrivate", "ConstPropertyName")
object AppConfig {

    // -------------------------- 内存“偏好”实现（替代 SharedPreferences） --------------------------

    private object MemPrefs {
        private val map = ConcurrentHashMap<String, Any?>()

        @Suppress("UNCHECKED_CAST")
        fun <T> get(key: String, default: T): T = (map[key] as? T) ?: default
        fun put(key: String, value: Any?) {
            if (value == null) map.remove(key) else map[key] = value
        }
        fun remove(key: String) = map.remove(key)
    }

    // 通用委托：var foo by pref("key", default)
    private fun <T> pref(key: String, default: T): ReadWriteProperty<Any?, T> =
        object : ReadWriteProperty<Any?, T> {
            override fun getValue(thisRef: Any?, property: KProperty<*>): T =
                MemPrefs.get(key, default)

            override fun setValue(thisRef: Any?, property: KProperty<*>, value: T) {
                MemPrefs.put(key, value)
            }
        }

    // -------------------------- 所有配置项（默认值版） --------------------------

    // 原：val isCronet = appCtx.getPrefBoolean(PreferKey.cronet)
    val isCronet: Boolean get() = false

    var useAntiAlias by pref("antiAlias", false)

    var userAgent: String
        get() = MemPrefs.get("userAgent",
            // 直接给一个桌面 Chrome UA；不再依赖 BuildConfig.Cronet_Main_Version
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) " +
                    "AppleWebKit/537.36 (KHTML, like Gecko) " +
                    "Chrome/120.0.0.0 Safari/537.36"
        )
        set(value) = MemPrefs.put("userAgent", value)

    var themeMode by pref("themeMode", "0")          // 0=跟随系统,1=亮,2=暗,3=墨水屏
    var isEInkMode: Boolean
        get() = themeMode == "3"
        set(_) {}                                     // 由 themeMode 推导，不单独设置

    var clickActionTL by pref("clickActionTL", 2)
    var clickActionTC by pref("clickActionTC", 2)
    var clickActionTR by pref("clickActionTR", 1)
    var clickActionML by pref("clickActionML", 2)
    var clickActionMC by pref("clickActionMC", 0)
    var clickActionMR by pref("clickActionMR", 1)
    var clickActionBL by pref("clickActionBL", 2)
    var clickActionBC by pref("clickActionBC", 1)
    var clickActionBR by pref("clickActionBR", 1)

    var useDefaultCover by pref("useDefaultCover", false)

    // 原：CanvasRecorderFactory.isSupport && ...
    var optimizeRender by pref("optimizeRender", false)

    var recordLog by pref("recordLog", false)

    // -------------------------- 主题 & 亮度等 --------------------------

    var isNightTheme: Boolean
        get() = when (themeMode) {
            "1" -> false
            "2" -> true
            "3" -> false
            else -> false // 非 Android 无“系统暗色”概念，默认 false
        }
        set(value) {
            // 与原逻辑保持：切换时写入 themeMode
            themeMode = if (value) "2" else "1"
        }

    var showUnread by pref("showUnread", true)
    var showLastUpdateTime by pref("showLastUpdateTime", false)
    var showWaitUpCount by pref("showWaitUpCount", false)

    var readBrightness: Int
        get() = if (isNightTheme) MemPrefs.get("nightBrightness", 100) else MemPrefs.get("brightness", 100)
        set(value) {
            if (isNightTheme) MemPrefs.put("nightBrightness", value) else MemPrefs.put("brightness", value)
        }

    val textSelectAble get() = MemPrefs.get("textSelectAble", true)
    val isTransparentStatusBar get() = MemPrefs.get("transparentStatusBar", true)
    val immNavigationBar get() = MemPrefs.get("immNavigationBar", true)
    val screenOrientation: String? get() = MemPrefs.get("screenOrientation", null)

    var bookGroupStyle by pref("bookGroupStyle", 0)
    var bookshelfLayout by pref("bookshelfLayout", 0)
    var saveTabPosition by pref("saveTabPosition", 0)

    var bookExportFileName by pref<String?>("bookExportFileName", null)
    var episodeExportFileName by pref("episodeExportFileName", "")
    var bookImportFileName by pref<String?>("bookImportFileName", null)

    var backupPath by pref<String?>("backupPath", null)
    var defaultBookTreeUri by pref<String?>("defaultBookTreeUri", null)

    val showDiscovery get() = MemPrefs.get("showDiscovery", true)
    val showRSS get() = MemPrefs.get("showRss", true)
    val autoRefreshBook get() = MemPrefs.get("autoRefresh", false)

    // 原：BuildConfig.DEBUG && pref
    var enableReview: Boolean
        get() = MemPrefs.get("enableReview", false)
        set(value) = MemPrefs.put("enableReview", value)

    var threadCount by pref("threadCount", 16)
    var remoteServerId by pref("remoteServerId", 0L)

    var importBookPath by pref<String?>("importBookPath", null)

    var ttsFlowSys by pref("ttsFollowSys", true)
    val noAnimScrollPage get() = MemPrefs.get("noAnimScrollPage", false)

    const val defaultSpeechRate = 5
    var ttsSpeechRate by pref("ttsSpeechRate", defaultSpeechRate)
    var ttsTimer by pref("ttsTimer", 0)
    val speechRatePlay: Int get() = if (ttsFlowSys) defaultSpeechRate else ttsSpeechRate

    var chineseConverterType by pref("chineseConverterType", 0)
    var systemTypefaces by pref("systemTypefaces", 0)

    var elevation: Int
        get() = if (isEInkMode) 0 else MemPrefs.get("barElevation", AppConst.sysElevation)
        set(value) = MemPrefs.put("barElevation", value)

    var readUrlInBrowser by pref("readUrlOpenInBrowser", false)

    var exportCharset: String
        get() = MemPrefs.get("exportCharset", "UTF-8").ifBlank { "UTF-8" }
        set(value) = MemPrefs.put("exportCharset", value)

    var exportUseReplace by pref("exportUseReplace", true)
    var exportToWebDav by pref("exportToWebDav", false)
    var exportNoChapterName by pref("exportNoChapterName", false)
    var enableCustomExport by pref("enableCustomExport", false)
    var exportType by pref("exportType", 0)
    var exportPictureFile by pref("exportPictureFile", false)
    var parallelExportBook by pref("parallelExportBook", false)
    var changeSourceCheckAuthor by pref("changeSourceCheckAuthor", false)

    var ttsEngine by pref<String?>("ttsEngine", null)
    var webPort by pref("webPort", 1122)
    var tocUiUseReplace by pref("tocUiUseReplace", false)
    var tocCountWords by pref("tocCountWords", true)
    var enableReadRecord by pref("enableReadRecord", true)

    val autoChangeSource get() = MemPrefs.get("autoChangeSource", true)

    var changeSourceLoadInfo by pref("changeSourceLoadInfo", false)
    var changeSourceLoadToc by pref("changeSourceLoadToc", false)
    var changeSourceLoadWordCount by pref("changeSourceLoadWordCount", false)

    var openBookInfoByClickTitle by pref("openBookInfoByClickTitle", true)
    var showBookshelfFastScroller by pref("showBookshelfFastScroller", false)

    var contentSelectSpeakMod by pref("contentSelectSpeakMod", 0)
    var batchChangeSourceDelay by pref("batchChangeSourceDelay", 0)

    val importKeepName get() = MemPrefs.get("importKeepName", false)
    val importKeepGroup get() = MemPrefs.get("importKeepGroup", false)
    var importKeepEnable by pref("importKeepEnable", false)

    var previewImageByClick by pref("previewImageByClick", false)

    var preDownloadNum by pref("preDownloadNum", 10)
    val syncBookProgress get() = MemPrefs.get("syncBookProgress", true)
    val syncBookProgressPlus get() = MemPrefs.get("syncBookProgressPlus", false)
    val mediaButtonOnExit get() = MemPrefs.get("mediaButtonOnExit", true)
    val readAloudByMediaButton get() = MemPrefs.get("readAloudByMediaButton", false)
    val replaceEnableDefault get() = MemPrefs.get("replaceEnableDefault", true)
    val webDavDir get() = MemPrefs.get("webDavDir", "legado")
    val webDavDeviceName get() = MemPrefs.get("webDavDeviceName", "JVM")
    val recordHeapDump get() = MemPrefs.get("recordHeapDump", false)
    val loadCoverOnlyWifi get() = MemPrefs.get("loadCoverOnlyWifi", false)
    val showAddToShelfAlert get() = MemPrefs.get("showAddToShelfAlert", true)
    val ignoreAudioFocus get() = MemPrefs.get("ignoreAudioFocus", false)

    var pauseReadAloudWhilePhoneCalls
        get() = MemPrefs.get("pauseReadAloudWhilePhoneCalls", false)
        set(value) = MemPrefs.put("pauseReadAloudWhilePhoneCalls", value)

    val onlyLatestBackup get() = MemPrefs.get("onlyLatestBackup", true)
    val defaultHomePage get() = MemPrefs.get("defaultHomePage", "bookshelf")
    val updateToVariant get() = MemPrefs.get("updateToVariant", "default_version")
    val streamReadAloudAudio get() = MemPrefs.get("streamReadAloudAudio", false)

    val doublePageHorizontal: String? get() = MemPrefs.get("doublePageHorizontal", null)
    val progressBarBehavior: String? get() = MemPrefs.get("progressBarBehavior", "page")

    val keyPageOnLongPress get() = MemPrefs.get("keyPageOnLongPress", false)
    val volumeKeyPage get() = MemPrefs.get("volumeKeyPage", true)
    val volumeKeyPageOnPlay get() = MemPrefs.get("volumeKeyPageOnPlay", true)
    val mouseWheelPage get() = MemPrefs.get("mouseWheelPage", true)
    val paddingDisplayCutouts get() = MemPrefs.get("paddingDisplayCutouts", false)

    var searchScope: String
        get() = MemPrefs.get("searchScope", "")
        set(value) = MemPrefs.put("searchScope", value)

    var searchGroup: String
        get() = MemPrefs.get("searchGroup", "")
        set(value) = MemPrefs.put("searchGroup", value)

    var pageTouchSlop by pref("pageTouchSlop", 0)
    var bookshelfSort by pref("bookshelfSort", 0)

    fun getBookSortByGroupId(groupId: Long): Int {
        // 脱离数据库，直接返回全局排序方式
        return bookshelfSort
    }

    private fun getPrefUserAgent(): String = userAgent

    var bitmapCacheSize by pref("bitmapCacheSize", 50)
    var imageRetainNum by pref("imageRetainNum", 0)

    var showReadTitleBarAddition by pref("showReadTitleAddition", true)
    var readBarStyleFollowPage by pref("readBarStyleFollowPage", false)

    var sourceEditMaxLine: Int
        get() = MemPrefs.get("sourceEditMaxLine", Int.MAX_VALUE).let { if (it < 10) Int.MAX_VALUE else it }
        set(value) = MemPrefs.put("sourceEditMaxLine", value)

    var audioPlayUseWakeLock by pref("audioPlayWakeLock", false)
    var brightnessVwPos by pref("brightnessVwPos", false)

    fun detectClickArea() {
        if (clickActionTL * clickActionTC * clickActionTR *
            clickActionML * clickActionMC * clickActionMR *
            clickActionBL * clickActionBC * clickActionBR != 0
        ) {
            clickActionMC = 0
            // 非 Android 环境：用打印替代 toast
            println("当前没有配置菜单区域, 自动恢复中间区域为菜单.")
        }
    }

    val showMangaUi get() = MemPrefs.get("showMangaUi", true)

    var disableMangaScale by pref("disableMangaScale", true)
    var mangaPreDownloadNum by pref("mangaPreDownloadNum", 10)
    var disableClickScroll by pref("disableClickScroll", false)
    var mangaAutoPageSpeed by pref("mangaAutoPageSpeed", 3)
    var mangaFooterConfig by pref("mangaFooterConfig", "")
    var enableMangaHorizontalScroll by pref("enableMangaHorizontalScroll", false)
    var mangaColorFilter by pref("mangaColorFilter", "")
    var hideMangaTitle by pref("hideMangaTitle", false)
    var enableMangaEInk by pref("enableMangaEInk", false)
    var mangaEInkThreshold by pref("mangaEInkThreshold", 150)
    var disableHorizontalAnimator by pref("disableHorizontalAnimator", false)
    var enableMangaGray by pref("enableMangaGray", false)

    // -------------------------- 占位：与原接口对齐（非 Android 无 SharedPreferences） --------------------------

    // 原来实现了 SharedPreferences.OnSharedPreferenceChangeListener。
    // 这里保留一个同名方法做 no-op，以尽量减少改动点。
    @Suppress("UNUSED_PARAMETER")
    fun onSharedPreferenceChanged(sharedPreferences: Any?, key: String?) {
        // 非 Android 环境下不做任何事
    }
}
