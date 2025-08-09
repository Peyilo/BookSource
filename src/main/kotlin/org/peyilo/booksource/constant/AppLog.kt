package org.peyilo.booksource.constant

object AppLog {

    private val mLogs = arrayListOf<Triple<Long, String, Throwable?>>()

    val logs get() = mLogs.toList()

    @Synchronized
    fun clear() {
        mLogs.clear()
    }
}