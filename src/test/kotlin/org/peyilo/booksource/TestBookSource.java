package org.peyilo.booksource;

import com.google.gson.Gson;
import org.junit.jupiter.api.Test;
import org.peyilo.booksource.entities.BookSource;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;

public class TestBookSource {

    @Test
    public void test2() throws IOException {
        Gson gson = new Gson();
        BookSource[] bookSources = gson.fromJson(new BufferedReader(new FileReader("F:\\Code\\IDEA\\BookSource\\src\\main\\resources\\b778fe6b.json")), BookSource[].class);
        System.out.println();
    }

    @Test
    public void test3() throws IOException {
        BookSource[] bookSources = ParseBookSourceKt.parseBookSource("F:\\Code\\IDEA\\BookSource\\src\\main\\resources\\shareBookSource.json");
        System.out.println(Arrays.toString(bookSources));
    }
}
