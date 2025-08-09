package org.peyilo.booksource;

import com.google.gson.Gson;
import org.junit.jupiter.api.Test;
import org.peyilo.booksource.data.entities.BookSource;

import java.io.*;
import java.net.CookieManager

class TestBookSource {


    @Test
    fun testHello() {
        println("Hello TestBookSource")
    }

    @Test
    fun test1() {
        val gson = Gson()
        getResourceAsStream("b778fe6b.json").use { inputStream ->
            val reader = BufferedReader(InputStreamReader(inputStream))
            val bookSources: Array<BookSource> = gson.fromJson(reader, Array<BookSource>::class.java)
            println("book sources size: ${bookSources.size}")
        }
    }

    @Test
    fun test2() {
        val bookSource = getResourceAsStream("demo.json").use { inputStream ->
             parseBookSource(inputStream)
        }
        requireNotNull(bookSource) { "Book source not found" }
        println(bookSource.toString())

    }


}
