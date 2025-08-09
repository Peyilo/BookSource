package org.peyilo.booksource.data.entities

data class Cache(
    val key: String = "",
    var value: String? = null,
    var deadline: Long = 0L
)