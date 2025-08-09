package org.peyilo.booksource.data.entities

import com.jayway.jsonpath.DocumentContext
import org.peyilo.booksource.utils.GSON
import org.peyilo.booksource.utils.jsonPath
import org.peyilo.booksource.utils.readLong
import org.peyilo.booksource.utils.readString

/**
 * 在线朗读引擎
 */
data class HttpTTS(
    val id: Long = System.currentTimeMillis(),
    var name: String = "",
    var url: String = "",
    var contentType: String? = null,
    override var concurrentRate: String? = "0",
    override var loginUrl: String? = null,
    override var loginUi: String? = null,
    override var header: String? = null,
    override var jsLib: String? = null,
    override var enabledCookieJar: Boolean? = false,
    var loginCheckJs: String? = null,
    var lastUpdateTime: Long = System.currentTimeMillis()
) : BaseSource {

    override fun getTag(): String {
        return name
    }

    override fun getKey(): String {
        return "httpTts:$id"
    }

    @Suppress("MemberVisibilityCanBePrivate")
    companion object {

        fun fromJsonDoc(doc: DocumentContext): Result<HttpTTS> {
            return kotlin.runCatching {
                val loginUi = doc.read<Any>("$.loginUi")
                HttpTTS(
                    id = doc.readLong("$.id") ?: System.currentTimeMillis(),
                    name = doc.readString("$.name")!!,
                    url = doc.readString("$.url")!!,
                    contentType = doc.readString("$.contentType"),
                    concurrentRate = doc.readString("$.concurrentRate"),
                    loginUrl = doc.readString("$.loginUrl"),
                    loginUi = if (loginUi is List<*>) GSON.toJson(loginUi) else loginUi?.toString(),
                    header = doc.readString("$.header"),
                    loginCheckJs = doc.readString("$.loginCheckJs")
                )
            }
        }

        fun fromJson(json: String): Result<HttpTTS> {
            return fromJsonDoc(jsonPath.parse(json))
        }

        fun fromJsonArray(jsonArray: String): Result<ArrayList<HttpTTS>> {
            return kotlin.runCatching {
                val sources = arrayListOf<HttpTTS>()
                val doc = jsonPath.parse(jsonArray).read<List<*>>("$")
                doc.forEach {
                    val jsonItem = jsonPath.parse(it)
                    fromJsonDoc(jsonItem).getOrThrow().let { source ->
                        sources.add(source)
                    }
                }
                return@runCatching sources
            }
        }

    }

}