package org.peyilo.booksource.data.entities

data class RuleSub(
    val id: Long = System.currentTimeMillis(),
    var name: String = "",
    var url: String = "",
    var type: Int = 0,
    var customOrder: Int = 0,
    var autoUpdate: Boolean = false,
    var update: Long = System.currentTimeMillis()
)
