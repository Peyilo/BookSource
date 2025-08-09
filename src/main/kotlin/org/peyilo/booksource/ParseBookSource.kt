package org.peyilo.booksource

import com.google.gson.Gson
import org.peyilo.booksource.data.entities.BookSource
import java.io.BufferedReader
import java.io.File
import java.io.FileReader
import java.io.InputStream
import java.io.InputStreamReader

fun parseBookSource(inputStream: InputStream): Array<BookSource>? {
    val gson = Gson()
    return gson.fromJson(BufferedReader(InputStreamReader(inputStream)), Array<BookSource>::class.java)
}

fun parseBookSource(file: File): Array<BookSource>? {
    val gson = Gson()
    return gson.fromJson(BufferedReader(FileReader(file)), Array<BookSource>::class.java)
}

fun parseBookSource(path: String): Array<BookSource>? {
    val gson = Gson()
    return gson.fromJson(BufferedReader(FileReader(path)), Array<BookSource>::class.java)
}
