package org.peyilo.booksource.help.http

import org.peyilo.booksource.lib.cronet.CronetInterceptor
import org.peyilo.booksource.lib.cronet.CronetLoader
import okhttp3.Interceptor

object Cronet {

    val loader: LoaderInterface? by lazy {
        CronetLoader
    }

    fun preDownload() {
        loader?.preDownload()
    }

    val interceptor: Interceptor? by lazy {
        CronetInterceptor(cookieJar)
    }

    interface LoaderInterface {

        fun install(): Boolean

        fun preDownload()

    }

}