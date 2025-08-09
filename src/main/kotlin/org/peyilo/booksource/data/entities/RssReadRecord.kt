package org.peyilo.booksource.data.entities

data class RssReadRecord(
    val record: String,
    val title: String? = null,
    val readTime: Long? = null,
    val read: Boolean = true
)