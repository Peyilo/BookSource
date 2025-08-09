package org.peyilo.booksource.help.http

import org.peyilo.booksource.constant.AppLog

object OkhttpUncaughtExceptionHandler : Thread.UncaughtExceptionHandler {

    override fun uncaughtException(t: Thread, e: Throwable) {
        AppLog.put("Okhttp Dispatcher中的线程执行出错\n${e.localizedMessage}", e)
    }

}
