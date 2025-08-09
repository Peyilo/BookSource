package org.peyilo.booksource

import java.io.File
import java.io.InputStream

private val classLoader = Thread.currentThread().contextClassLoader

fun getResourceAsStream(name: String): InputStream {
    val stream = classLoader.getResourceAsStream(name)
    requireNotNull(stream) { "Resource $name not found" }
    return stream
}

fun getResourceFile(name: String): File {
    val url = classLoader.getResource(name)
    requireNotNull(url) { "Resource $name not found" }
    return File(url.toURI())
}