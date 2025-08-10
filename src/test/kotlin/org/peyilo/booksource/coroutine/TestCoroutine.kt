package org.peyilo.booksource.coroutine

import kotlinx.coroutines.CoroutineName
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.async
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.joinAll
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import org.junit.jupiter.api.Test
import java.util.concurrent.atomic.AtomicInteger

class TestCoroutine {

    @Test
    fun test() = runBlocking {
        // context默认使用EmptyCoroutineContext，且在这个block中包含CoroutineScope上下文

        // launch会启动一个suspend block，并且block本身不返回值
        val job = launch {
            delay(1000)
            println("Hello launch!")
        }

        // async也会启动一个suspend block，但是block拥有返回值，返回值可以通过deferred.await()获取
        val deferred = async {
            delay(1000)
            "Hello async"
        }

        println(deferred.await())
        job.join()
    }

    @Test
    fun test2() {
        // CoroutineContext无处不在
        val ctx = Job() + Dispatchers.IO + CoroutineName("Fetch")
    }

    /**
     * 挂起函数具有传染性，只有挂起函数能够调用挂起函数
     * suspend 修饰的函数只能在：
     *
     * - 另一个挂起函数里调用
     *
     * - 协程构建器（launch、async、runBlocking 等）的 block 里调用
     */
    private suspend fun hello() {
        delay(1000)
        println("Hello")
    }

    @Test
    fun test3() {
        // 挂起函数不能在非suspend函数调用
        // hello()

        // 阻塞主线程，以保证block内部能够全部完成执行
        runBlocking {
            val job = launch {
                hello()
            }
            // 等待job执行完成
            job.join()
        }
    }

    // 大规模并发测试
    @Test
    fun test4() = runBlocking {
        val counter = AtomicInteger(0)
        coroutineScope {
            repeat(1_000_0) {
                launch {
                    delay(5000)
                    print(".")
                    counter.incrementAndGet()
                }
            }
        }
        // runBlocking 会等所有子协程完成
        println("\nTotal printed: ${counter.get()}")
    }

    /**
     * runBlocking { ... } 默认使用 调用它的线程（JUnit 运行测试时就是测试线程）。
     *
     * launch { ... } 没指定调度器时，会继承父协程的上下文，所以这些协程默认全都在同一个线程执行。
     *
     * 这时没有并发写冲突，即使用普通的 var counter = 0 也不会出现数据竞争，因为调度是协作式的。
     */
    @Test
    fun test5() = runBlocking {
        var counter = 0
        val jobs = List(1_000_000) {
            launch {
                delay(5000)
                counter++
            }
        }
        jobs.joinAll()           // 等这批全部完成
        println("Total: $counter")
    }

    @Test
    fun test6() {
        var start = System.nanoTime()
        repeat(1_000_000) {
            print(".")
        }
        val duration1 = System.nanoTime() - start

        start = System.nanoTime()
        runBlocking {
            repeat(1_000_000) {
                launch {
                    print(".")
                }
            }
        }
        val duration2 = System.nanoTime() - start

        println()
        println("duration1: ${duration1 / 1e6} ms")
        println("duration2: ${duration2 / 1e6} ms")
    }

}