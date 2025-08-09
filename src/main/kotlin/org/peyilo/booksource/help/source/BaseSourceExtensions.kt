package org.peyilo.booksource.help.source

import org.peyilo.booksource.constant.SourceType
import org.peyilo.booksource.data.entities.BaseSource
import org.peyilo.booksource.data.entities.BookSource
import org.peyilo.booksource.data.entities.RssSource
import org.peyilo.booksource.model.SharedJsScope
import org.mozilla.javascript.Scriptable
import kotlin.coroutines.CoroutineContext

fun BaseSource.getShareScope(coroutineContext: CoroutineContext? = null): Scriptable? {
    return SharedJsScope.getScope(jsLib, coroutineContext)
}

fun BaseSource.getSourceType(): Int {
    return when (this) {
        is BookSource -> SourceType.book
        is RssSource -> SourceType.rss
        else -> error("unknown source type: ${this::class.simpleName}.")
    }
}
