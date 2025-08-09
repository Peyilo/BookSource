package org.peyilo.booksource.data.entities

data class ReadRecord(
    var deviceId: String = "",
    var bookName: String = "",
    var readTime: Long = 0L,
    var lastRead: Long = System.currentTimeMillis()
)