package org.peyilo.booksource.data.entities

import org.peyilo.booksource.model.analyzeRule.AnalyzeRule
import org.peyilo.booksource.model.analyzeRule.AnalyzeRule.Companion.setCoroutineContext
import org.peyilo.booksource.model.analyzeRule.AnalyzeUrl
import kotlin.coroutines.coroutineContext

/**
 * 字典规则
 */
data class DictRule(
    var name: String = "",
    var urlRule: String = "",
    var showRule: String = "",
    var enabled: Boolean = true,
    var sortNumber: Int = 0
) {

    override fun hashCode(): Int {
        return name.hashCode()
    }

    override fun equals(other: Any?): Boolean {
        if (other is DictRule) {
            return name == other.name
        }
        return false
    }

    /**
     * 搜索字典
     */
    suspend fun search(word: String): String {
        val analyzeUrl = AnalyzeUrl(urlRule, key = word, coroutineContext = coroutineContext)
        val body = analyzeUrl.getStrResponseAwait().body
        if (showRule.isBlank()) {
            return body!!
        }
        val analyzeRule = AnalyzeRule().setCoroutineContext(coroutineContext)
        return analyzeRule.getString(showRule, mContent = body)
    }

}