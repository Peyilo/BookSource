package org.peyilo.booksource.data.entities.rule

import com.google.gson.JsonDeserializer
import org.peyilo.booksource.utils.INITIAL_GSON

/**
 * 发现结果规则
 */
data class ExploreRule(
    override var bookList: String? = null,
    override var name: String? = null,
    override var author: String? = null,
    override var intro: String? = null,
    override var kind: String? = null,
    override var lastChapter: String? = null,
    override var updateTime: String? = null,
    override var bookUrl: String? = null,
    override var coverUrl: String? = null,
    override var wordCount: String? = null
) : BookListRule {

    companion object {

        val jsonDeserializer = JsonDeserializer<ExploreRule?> { json, _, _ ->
            when {
                json.isJsonObject -> INITIAL_GSON.fromJson(json, ExploreRule::class.java)
                json.isJsonPrimitive -> INITIAL_GSON.fromJson(
                    json.asString,
                    ExploreRule::class.java
                )

                else -> null
            }
        }

    }

}
