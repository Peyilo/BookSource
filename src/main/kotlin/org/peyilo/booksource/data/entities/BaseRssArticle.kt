package org.peyilo.booksource.data.entities

import org.peyilo.booksource.model.analyzeRule.RuleDataInterface
import org.peyilo.booksource.utils.GSON

interface BaseRssArticle : RuleDataInterface {

    var origin: String
    var link: String

    var variable: String?

    override fun putVariable(key: String, value: String?): Boolean {
        if (super.putVariable(key, value)) {
            variable = GSON.toJson(variableMap)
        }
        return true
    }

}