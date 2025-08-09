package org.peyilo.booksource.help

import androidx.annotation.Keep
import org.peyilo.booksource.exception.NoStackTraceException
import org.peyilo.booksource.model.analyzeRule.AnalyzeRule
import org.peyilo.booksource.model.analyzeRule.AnalyzeRule.Companion.setCoroutineContext
import org.peyilo.booksource.model.analyzeRule.AnalyzeUrl
import org.peyilo.booksource.utils.ACache
import org.peyilo.booksource.utils.FileUtils
import org.peyilo.booksource.utils.GSON
import org.peyilo.booksource.utils.compress.ZipUtils
import org.peyilo.booksource.utils.createFileReplace
import org.peyilo.booksource.utils.externalCache
import org.peyilo.booksource.utils.fromJsonArray
import org.peyilo.booksource.utils.fromJsonObject
import splitties.init.appCtx
import java.io.File
import kotlin.coroutines.coroutineContext

@Suppress("MemberVisibilityCanBePrivate")
object DirectLinkUpload {

    const val ruleFileName = "directLinkUploadRule.json"

    @Throws(NoStackTraceException::class)
    suspend fun upLoad(
        fileName: String,
        file: Any,
        contentType: String,
        rule: Rule = getRule()
    ): String {
        val url = rule.uploadUrl
        if (url.isBlank()) {
            throw NoStackTraceException("上传url未配置")
        }
        val downloadUrlRule = rule.downloadUrlRule
        if (downloadUrlRule.isBlank()) {
            throw NoStackTraceException("下载地址规则未配置")
        }
        var mFileName = fileName
        var mFile = file
        var mContentType = contentType
        if (rule.compress && contentType != "application/zip") {
            mFileName = "$fileName.zip"
            mContentType = "application/zip"
            mFile = when (file) {
                is File -> {
                    val zipFile = File(FileUtils.getPath(appCtx.externalCache, "upload", mFileName))
                    zipFile.createFileReplace()
                    ZipUtils.zipFile(file, zipFile)
                    zipFile
                }

                is ByteArray -> ZipUtils.zipByteArray(file, fileName)
                is String -> ZipUtils.zipByteArray(file.toByteArray(), fileName)
                else -> ZipUtils.zipByteArray(GSON.toJson(file).toByteArray(), fileName)
            }
        }
        val analyzeUrl = AnalyzeUrl(url)
        val res = analyzeUrl.upload(mFileName, mFile, mContentType)
        if (mFile is File) {
            mFile.delete()
        }
        val analyzeRule = AnalyzeRule().setContent(res.body, res.url)
            .setCoroutineContext(coroutineContext)
        val downloadUrl = analyzeRule.getString(downloadUrlRule)
        if (downloadUrl.isBlank()) {
            throw NoStackTraceException("上传失败,${res.body}")
        }
        return downloadUrl
    }

    val defaultRules: List<Rule> by lazy {
        val json = String(
            appCtx.assets.open("defaultData${File.separator}directLinkUpload.json")
                .readBytes()
        )
        GSON.fromJsonArray<Rule>(json).getOrThrow()
    }

    fun getRule(): Rule {
        return getConfig() ?: defaultRules[0]
    }

    fun getConfig(): Rule? {
        val json = ACache.get(cacheDir = false).getAsString(ruleFileName)
        return GSON.fromJsonObject<Rule>(json).getOrNull()
    }

    fun putConfig(rule: Rule) {
        ACache.get(cacheDir = false).put(ruleFileName, GSON.toJson(rule))
    }

    fun delConfig() {
        ACache.get(cacheDir = false).remove(ruleFileName)
    }

    fun getSummary(): String {
        return getRule().summary
    }

    @Keep
    data class Rule(
        var uploadUrl: String, //创建分享链接
        var downloadUrlRule: String, //下载链接规则
        var summary: String, //注释
        var compress: Boolean = false, //是否压缩
    ) {

        override fun toString(): String {
            return summary
        }

    }

}
