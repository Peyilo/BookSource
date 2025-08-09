package org.peyilo.booksource.data.entities

import org.peyilo.booksource.constant.BookType
import org.peyilo.booksource.utils.GSON
import org.peyilo.booksource.utils.fromJsonObject

data class SearchBook(
    override var bookUrl: String = "",
    /** 书源 */
    var origin: String = "",
    var originName: String = "",
    /** BookType */
    var type: Int = BookType.text,
    override var name: String = "",
    override var author: String = "",
    override var kind: String? = null,
    var coverUrl: String? = null,
    var intro: String? = null,
    override var wordCount: String? = null,
    var latestChapterTitle: String? = null,
    /** 目录页Url (toc=table of Contents) */
    var tocUrl: String = "",
    var time: Long = System.currentTimeMillis(),
    override var variable: String? = null,
    var originOrder: Int = 0,
    var chapterWordCountText: String? = null,
    var chapterWordCount: Int = -1,
    var respondTime: Int = -1
) :  BaseBook, Comparable<SearchBook> {

    override var infoHtml: String? = null

    override var tocHtml: String? = null

    override fun equals(other: Any?) = other is SearchBook && other.bookUrl == bookUrl

    override fun hashCode() = bookUrl.hashCode()

    override fun compareTo(other: SearchBook): Int {
        return other.originOrder - this.originOrder
    }

    override val variableMap: HashMap<String, String> by lazy {
        GSON.fromJsonObject<HashMap<String, String>>(variable).getOrNull() ?: HashMap()
    }

    val origins: LinkedHashSet<String> by lazy { linkedSetOf(origin) }

    fun addOrigin(origin: String) {
        origins.add(origin)
    }

    fun getDisplayLastChapterTitle(): String {
        latestChapterTitle?.let {
            if (it.isNotEmpty()) {
                return it
            }
        }
        return "无最新章节"
    }

    fun trimIntro(): String {
        val trimIntro = intro?.trim()
        return if (trimIntro.isNullOrEmpty()) {
            "Description: no introduction"
        } else {
            "Description:${trimIntro}"
        }
    }

    fun releaseHtmlData() {
        infoHtml = null
        tocHtml = null
    }

    fun primaryStr(): String {
        return origin + bookUrl
    }

    fun sameBookTypeLocal(bookType: Int): Boolean {
        return type and BookType.allBookTypeLocal == bookType and BookType.allBookTypeLocal
    }

}