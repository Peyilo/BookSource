package org.peyilo.booksource.help.source

import com.script.rhino.runScriptWithContext
import org.peyilo.booksource.constant.BookSourceType
import org.peyilo.booksource.constant.BookType
import org.peyilo.booksource.data.entities.BookSource
import org.peyilo.booksource.data.entities.BookSourcePart
import org.peyilo.booksource.data.entities.rule.ExploreKind
import org.peyilo.booksource.utils.ACache
import org.peyilo.booksource.utils.GSON
import org.peyilo.booksource.utils.MD5Utils
import org.peyilo.booksource.utils.fromJsonArray
import org.peyilo.booksource.utils.isJsonArray
import org.peyilo.booksource.utils.printOnDebug
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import kotlinx.coroutines.withContext
import java.util.concurrent.ConcurrentHashMap

/**
 * 采用md5作为key可以在分类修改后自动重新计算,不需要手动刷新
 */

private val mutexMap by lazy { hashMapOf<String, Mutex>() }
private val exploreKindsMap by lazy { ConcurrentHashMap<String, List<ExploreKind>>() }
private val aCache by lazy { ACache.get("explore") }

private fun BookSource.getExploreKindsKey(): String {
    return MD5Utils.md5Encode(bookSourceUrl + exploreUrl)
}

private fun BookSourcePart.getExploreKindsKey(): String {
    return getBookSource()!!.getExploreKindsKey()
}

suspend fun BookSourcePart.exploreKinds(): List<ExploreKind> {
    return getBookSource()!!.exploreKinds()
}

suspend fun BookSource.exploreKinds(): List<ExploreKind> {
    val exploreKindsKey = getExploreKindsKey()
    exploreKindsMap[exploreKindsKey]?.let { return it }
    val exploreUrl = exploreUrl
    if (exploreUrl.isNullOrBlank()) {
        return emptyList()
    }
    val mutex = mutexMap[bookSourceUrl] ?: Mutex().apply { mutexMap[bookSourceUrl] = this }
    mutex.withLock {
        exploreKindsMap[exploreKindsKey]?.let { return it }
        val kinds = arrayListOf<ExploreKind>()
        withContext(Dispatchers.IO) {
            kotlin.runCatching {
                var ruleStr = exploreUrl
                if (exploreUrl.startsWith("<js>", true)
                    || exploreUrl.startsWith("@js:", true)
                ) {
                    ruleStr = aCache.getAsString(exploreKindsKey)
                    if (ruleStr.isNullOrBlank()) {
                        val jsStr = if (exploreUrl.startsWith("@")) {
                            exploreUrl.substring(4)
                        } else {
                            exploreUrl.substring(4, exploreUrl.lastIndexOf("<"))
                        }
                        ruleStr = runScriptWithContext {
                            evalJS(jsStr).toString().trim()
                        }
                        aCache.put(exploreKindsKey, ruleStr)
                    }
                }
                if (ruleStr.isJsonArray()) {
                    GSON.fromJsonArray<ExploreKind>(ruleStr).getOrThrow().let {
                        kinds.addAll(it)
                    }
                } else {
                    ruleStr.split("(&&|\n)+".toRegex()).forEach { kindStr ->
                        val kindCfg = kindStr.split("::")
                        kinds.add(ExploreKind(kindCfg.first(), kindCfg.getOrNull(1)))
                    }
                }
            }.onFailure {
                kinds.add(ExploreKind("ERROR:${it.localizedMessage}", it.stackTraceToString()))
                it.printOnDebug()
            }
        }
        exploreKindsMap[exploreKindsKey] = kinds
        return kinds
    }
}

suspend fun BookSourcePart.clearExploreKindsCache() {
    withContext(Dispatchers.IO) {
        val exploreKindsKey = getExploreKindsKey()
        aCache.remove(exploreKindsKey)
        exploreKindsMap.remove(exploreKindsKey)
    }
}

suspend fun BookSource.clearExploreKindsCache() {
    withContext(Dispatchers.IO) {
        val exploreKindsKey = getExploreKindsKey()
        aCache.remove(exploreKindsKey)
        exploreKindsMap.remove(exploreKindsKey)
    }
}

fun BookSource.exploreKindsJson(): String {
    val exploreKindsKey = getExploreKindsKey()
    return aCache.getAsString(exploreKindsKey)?.takeIf { it.isJsonArray() }
        ?: exploreUrl.takeIf { it.isJsonArray() }
        ?: ""
}

fun BookSource.getBookType(): Int {
    return when (bookSourceType) {
        BookSourceType.file -> BookType.text or BookType.webFile
        BookSourceType.image -> BookType.image
        BookSourceType.audio -> BookType.audio
        else -> BookType.text
    }
}
