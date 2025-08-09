package org.peyilo.booksource.data.entities

import org.peyilo.booksource.help.config.AppConfig

@Suppress("ConstPropertyName")
data class BookGroup(
    val groupId: Long = 0b1,
    var groupName: String = "",
    var cover: String? = null,
    var order: Int = 0,
    var enableRefresh: Boolean = true,
    var show: Boolean = true,
    var bookSort: Int = -1
) {

    companion object {
        const val IdRoot = -100L
        const val IdAll = -1L
        const val IdLocal = -2L
        const val IdAudio = -3L
        const val IdNetNone = -4L
        const val IdLocalNone = -5L
        const val IdError = -11L
    }

    fun getManageName(): String {
        return when (groupId) {
            IdAll -> "$groupName(All)"
            IdAudio -> "$groupName(audio)"
            IdLocal -> "$groupName(local})"
            IdNetNone -> "$groupName(net_no_group})"
            IdLocalNone -> "$groupName(local_no_group})"
            IdError -> "$groupName(update_book_fail})"
            else -> groupName
        }
    }

    fun getRealBookSort(): Int {
        if (bookSort < 0) {
            return AppConfig.bookshelfSort
        }
        return bookSort
    }

    override fun hashCode(): Int {
        return groupId.hashCode()
    }

    override fun equals(other: Any?): Boolean {
        if (other is BookGroup) {
            return other.groupId == groupId
                    && other.groupName == groupName
                    && other.cover == cover
                    && other.bookSort == bookSort
                    && other.enableRefresh == enableRefresh
                    && other.show == show
                    && other.order == order
        }
        return false
    }

}