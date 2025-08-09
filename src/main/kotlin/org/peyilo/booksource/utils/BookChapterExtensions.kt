package org.peyilo.booksource.utils

import org.peyilo.booksource.data.entities.BookChapter

fun BookChapter.internString() {
    title = title.intern()
    bookUrl = bookUrl.intern()
}
