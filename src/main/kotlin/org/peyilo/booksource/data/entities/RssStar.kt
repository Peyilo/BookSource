package org.peyilo.booksource.data.entities

import org.peyilo.booksource.utils.GSON
import org.peyilo.booksource.utils.fromJsonObject

data class RssStar(
    override var origin: String = "",
    var sort: String = "",
    var title: String = "",
    var starTime: Long = 0,
    override var link: String = "",
    var pubDate: String? = null,
    var description: String? = null,
    var content: String? = null,
    var image: String? = null,
    var group: String = "默认分组",
    override var variable: String? = null
) : BaseRssArticle {

    override val variableMap by lazy {
        GSON.fromJsonObject<HashMap<String, String>>(variable).getOrNull() ?: hashMapOf()
    }

    fun toRssArticle() = RssArticle(
        origin = origin,
        sort = sort,
        title = title,
        link = link,
        pubDate = pubDate,
        description = description,
        content = content,
        image = image,
        group = group,
        variable = variable
    )
}
