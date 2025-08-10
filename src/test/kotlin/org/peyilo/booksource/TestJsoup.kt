package org.peyilo.booksource

import kotlinx.coroutines.async
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import org.peyilo.libreadview.loader.BiqugeBookLoader
import org.peyilo.libreadview.loader.BookLoaderCoroutineWarp
import org.peyilo.libreadview.utils.save
import java.io.File
import kotlin.test.Test

class TestJsoup {

    @Test
    fun test() {
        val loader = BiqugeBookLoader(185030)
        val book = loader.initToc()
        loader.loadChap(book.getChap(1))
        println(book)
    }





}