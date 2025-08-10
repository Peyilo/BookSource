package org.peyilo.booksource

import kotlinx.coroutines.async
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import org.peyilo.libreadview.loader.BiqugeBookLoader
import org.peyilo.libreadview.loader.BookLoaderCoroutineWarp
import org.peyilo.libreadview.utils.save
import java.io.File

// 打印进度条
private fun printProgress(done: Int, total: Int) {
    val width = 40
    val ratio = done.toDouble() / total.coerceAtLeast(1)
    val filled = (ratio * width).toInt()
    val bar = buildString {
        append('[')
        repeat(filled) { append('=') }
        repeat(width - filled) { append(' ') }
        append(']')
    }
    val percent = (ratio * 100).toInt()
    print("\r$bar $percent% ($done/$total)")
    if (done == total) println()
}

fun main() = runBlocking {
    val loaderCoroutineWarp = BookLoaderCoroutineWarp(BiqugeBookLoader(185030))

    // 先完成目录初始化
    val book = async { loaderCoroutineWarp.initToc() }.await()

    // 加载全部章节
    val job = launch {
        loaderCoroutineWarp.loadAllChapters(book, onProgress = ::printProgress)
    }
    job.join()

    val savedFolder = File(resourceRoot, "books")
    if (!savedFolder.exists()) {
        savedFolder.mkdirs()
    }
    val savedFile = book.save(savedFolder)

    println("Saved $savedFile")
}